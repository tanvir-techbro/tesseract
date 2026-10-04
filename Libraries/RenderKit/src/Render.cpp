/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

#include "RenderKit/Render.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstddef>
#include <sstream>
#include <string>

namespace Tess::Render {

/* Typeface */
Typeface::Typeface(TTF_TextEngine *e, std::string p, std::string bp)
    : eng(e), path(std::move(p)), bold_path(std::move(bp)) {}

Typeface::~Typeface() {
      Close();
}

void Typeface::Close() {
      for (auto &[size, font] : fonts) {
            if (font) {
                  TTF_CloseFont(font);
            }
      }
      fonts.clear();
      for (auto &[size, font] : bold_fonts) {
            if (font) {
                  TTF_CloseFont(font);
            }
      }
      bold_fonts.clear();
}

TTF_Font *Typeface::At(float size, bool bold) {
      auto &cache = bold ? bold_fonts : fonts;
      const std::string &file = bold ? bold_path : path;
      auto it = cache.find(size);
      if (it != cache.end()) {
            if (it->second) {
                  return it->second;
            }
            // Bold file missing/broken: fall back to regular below.
            if (bold) {
                  return At(size, false);
            }
            return nullptr;
      }
      TTF_Font *font = file.empty() ? nullptr : TTF_OpenFont(file.c_str(), size);
      cache[size] = font; // cache null too: don't retry a broken file per line
      if (!font && bold) {
            return At(size, false);
      }
      return font;
}

/* Helper */
namespace {

// collapse all whitespaces
std::string CleanText(const std::string &s) {
      std::string out;
      out.reserve(s.size());
      bool space = true; // trim leading
      for (char c : s) {
            bool ws = c == ' ' || c == '\n' || c == '\r' || c == '\t';
            if (ws) {
                  if (!space) {
                        out += ' ';
                        space = true;
                  }
            } else {
                  out += c;
                  space = false;
            }
      }
      if (!out.empty() && out.back() == ' ') {
            out.pop_back();
      }
      return out;
}
float SizeFor(const std::string &tag, float inherited) {
      if (tag == "h1") {
            return 28.0f;
      }
      if (tag == "h2") {
            return 24.0f;
      }
      if (tag == "h3") {
            return 20.0f;
      }
      if (tag == "h4") {
            return 18.0f;
      }
      if (tag == "h5") {
            return 16.0f;
      }
      if (tag == "h6") {
            return 13.0f;
      }
      return inherited;
}
bool IsBlock(const std::string &tag) {
      return tag == "document" || tag == "div" || tag == "p" || tag == "h1" || tag == "h2"
            || tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6" || tag == "li"
            || tag == "ul" || tag == "ol" || tag == "dl" || tag == "dt" || tag == "dd"
            || tag == "blockquote";
}
bool BoldFor(const std::string &tag, bool inherited) {
      if (tag == "h1" || tag == "h2" || tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6") {
            return true;
      }
      return inherited;
}

// Wrap one text run into page lines (layout time only).
void LayoutText(Page &page, Typeface &face, const std::string &text, float x, float &y, float max_w,
                float size, bool bold, const std::string &link) {
      TTF_Font *font = face.At(size, bold);
      if (!font) {
            return;
      }
      TTF_Text *meas = TTF_CreateText(face.eng, font, "", 0);
      if (!meas) {
            return;
      }
      std::string line;
      std::istringstream words(CleanText(text));
      std::string word;

      auto measure = [&](const std::string &s) {
            int w = 0, h = 0;
            TTF_SetTextString(meas, s.c_str(), s.size());
            Tess::Draw::TextSize(meas, w, h);
            return w;
      };
      auto flush = [&] {
            if (line.empty()) {
                  return;
            }
            int w = 0, h = 0;
            TTF_SetTextString(meas, line.c_str(), line.size());
            Tess::Draw::TextSize(meas, w, h);
            TTF_Text *shaped = TTF_CreateText(face.eng, font, line.c_str(), line.size());
            if (shaped) {
                  if (link.empty()) {
                        if (page.dark) {
                              TTF_SetTextColor(shaped, 220, 220, 220, 255);
                        } else {
                              TTF_SetTextColor(shaped, 20, 20, 20, 255);
                        }
                  } else if (page.dark) {
                        TTF_SetTextColor(shaped, 120, 170, 255, 255);
                  } else {
                        TTF_SetTextColor(shaped, 20, 60, 200, 255);
                  }
            }
            page.lines.push_back(Line{
                  .text = line,
                  .x = x,
                  .y = y,
                  .w = (float)w,
                  .h = (float)h,
                  .size = size,
                  .link = link,
                  .font = font,
                  .shaped = shaped,
            });
            if (!link.empty()) {
                  page.hits.push_back(LinkRect{
                        {x, y, (float)w, (float)h + 2.0f},
                        link,
                  });
            }
            y += (float)h + 2.0f;
            line.clear();
      };

      while (words >> word) {
            std::string trial = line.empty() ? word : line + " " + word;
            if ((float)measure(trial) > max_w && !line.empty()) {
                  flush();
            }
            line = line.empty() ? word : line + " " + word;
      }
      flush();
      TTF_DestroyText(meas);
}

void LayoutChild(Page &page, Typeface &face, const Html::Document &doc, size_t idx, float x,
                 float &y, float max_w, float size, bool bold, const Tess::Net::Url &base,
                 const std::string &link) {
      const auto &node = doc.arena[idx];
      if (node.tag == "#text") {
            LayoutText(page, face, node.text, x, y, max_w, size, bold, link);
            return;
      }
      if (node.tag == "head") {
            return; // non-visual: title/meta/link never paint
      }
      float my_size = SizeFor(node.tag, size);
      bool my_bold = BoldFor(node.tag, bold);
      // Indented blocks: whole subtree shifts right, wrap width shrinks.
      float my_x = x;
      float my_w = max_w;
      if (node.tag == "dd" || node.tag == "blockquote") {
            my_x += 40.0f;
            my_w -= 40.0f;
            if (my_w < 40.0f) {
                  my_w = 40.0f;
            }
      }
      if (node.tag == "br") {
            y += my_size * 1.2f;
            return;
      }
      if (node.tag == "hr") {
            y += 4.0f;
            page.lines.push_back(Line{
                  .x = my_x,
                  .y = y,
                  .w = my_w,
                  .h = 2.0f,
                  .size = my_size,
                  .rule = true,
            });
            y += 6.0f;
            return;
      }
      std::string my_link = link;
      if (node.tag == "a") {
            auto it = node.attributes.find("href");
            if (it != node.attributes.end()) {
                  my_link = Tess::Net::Resolve(base, it->second);
            }
      }
      if (IsBlock(node.tag) && node.tag != "document") {
            y += 4.0f;
      }
      for (size_t k : node.kids) {
            LayoutChild(page, face, doc, k, my_x, y, my_w, my_size, my_bold, base, my_link);
      }
      if (IsBlock(node.tag) && node.tag != "document") {
            y += my_size * 0.4f;
      }
}

} // namespace

/* Main */
void ClearPage(Page &page) {
      for (auto &line : page.lines) {
            if (line.shaped) {
                  TTF_DestroyText(line.shaped);
                  line.shaped = nullptr;
            }
      }
      page.lines.clear();
      page.hits.clear();
      page.content_h = 0.0f;
}

void Layout(Page &page, Typeface &face, const Tess::Html::Document &doc, float x, float y,
            float max_w, const Tess::Net::Url &base) {
      ClearPage(page);
      if (max_w <= 0.0f) {
            return;
      }
      float cursor = y;
      for (size_t k : doc.arena[0].kids) {
            LayoutChild(page, face, doc, k, x, cursor, max_w, 16.0f, false, base, "");
      }
      page.content_h = cursor - y;
}

void Paint(SDL_Renderer *r, Page &page, float scroll) {
      for (auto &line : page.lines) {
            if (line.rule) {
                  SDL_SetRenderDrawColor(r, 150, 150, 150, 255);
                  float uy = line.y - scroll;
                  SDL_RenderLine(r, line.x, uy, line.x + line.w, uy);
                  continue;
            }
            if (!line.shaped) {
                  continue;
            }
            TTF_DrawRendererText(line.shaped, line.x, line.y - scroll);
            if (!line.link.empty()) {
                  if (page.dark) {
                        SDL_SetRenderDrawColor(r, 120, 170, 255, 255);
                  } else {
                        SDL_SetRenderDrawColor(r, 20, 60, 200, 255);
                  }
                  float uy = line.y - scroll + line.h;
                  SDL_RenderLine(r, line.x, uy, line.x + line.w, uy);
            }
      }
}

} // namespace Tess::Render
