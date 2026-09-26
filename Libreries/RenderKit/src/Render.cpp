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
Typeface::Typeface(TTF_TextEngine *e, std::string p) : eng(e), path(std::move(p)) {}

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
}

TTF_Font *Typeface::At(float size) {
      auto it = fonts.find(size);
      if (it != fonts.end()) {
            return it->second;
      }
      TTF_Font *font = path.empty() ? nullptr : TTF_OpenFont(path.c_str(), size);
      fonts[size] = font; // cache null too: don't retry a broken file per line
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
      return tag == "document" || tag == "div" || tag == "p" || tag == "h1" || tag == "h2" || tag == "h3" || tag == "h4"
            || tag == "h5" || tag == "h6" || tag == "li" || tag == "ul";
}

// Wrap one text run into page lines (layout time only).
void LayoutText(Page &page, Typeface &face, const std::string &text, float x, float &y, float max_w, float size) {
      TTF_Font *font = face.At(size);
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
                  TTF_SetTextColor(shaped, 20, 20, 20, 255);
            }
            page.lines.push_back(Line{.text = line, .x = x, .y = y, .size = size, .font = font, .shaped = shaped});
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

void LayoutChild(Page &page, Typeface &face, const Html::Document &doc, size_t idx, float x, float &y, float max_w,
                 float size) {
      const auto &node = doc.arena[idx];
      if (node.tag == "#text") {
            LayoutText(page, face, node.text, x, y, max_w, size);
            return;
      }
      float my_size = SizeFor(node.tag, size);
      if (IsBlock(node.tag) && node.tag != "document") {
            y += 4.0f;
      }
      for (size_t k : node.kids) {
            LayoutChild(page, face, doc, k, x, y, max_w, my_size);
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
      page.content_h = 0.0f;
}

void Layout(Page &page, Typeface &face, const Tess::Html::Document &doc, float x, float y, float max_w) {
      ClearPage(page);
      if (max_w <= 0.0f) {
            return;
      }
      float cursor = y;
      for (size_t k : doc.arena[0].kids) {
            LayoutChild(page, face, doc, k, x, cursor, max_w, 16.0f);
      }
      page.content_h = cursor - y;
}

void Paint(SDL_Renderer *r, Page &page) {
      (void)r;
      for (auto &line : page.lines) {
            if (!line.shaped) {
                  continue;
            }
            TTF_DrawRendererText(line.shaped, line.x, line.y);
      }
}

} // namespace Tess::Render
