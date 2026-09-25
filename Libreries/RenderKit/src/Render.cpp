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
      return tag == "document" || tag == "div" || tag == "p" || tag == "h1" || tag == "h2" || tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6" || tag == "li" || tag == "ul";
}

// Wrap one text run into page lines (layout time only).
void LayoutText(Page &page, TTF_Text *meas, TTF_Font *font, const std::string &text,
                float x, float &y, float maxW, float size) {
      TTF_SetFontSize(font, size);
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
            page.lines.push_back(Line{.text = line, .x = x, .y = y, .size = size});
            y += (float)h + 2.0f;
            line.clear();
      };

      while (words >> word) {
            std::string trial = line.empty() ? word : line + " " + word;
            if ((float)measure(trial) > maxW && !line.empty()) {
                  flush();
            }
            line = line.empty() ? word : line + " " + word;
      }
      flush();
}

void LayoutChild(Page &page, TTF_Text *meas, TTF_Font *font, const Html::Document &doc,
                 size_t idx, float x, float &y, float maxW, float size) {
      const auto &node = doc.arena[idx];
      if (node.tag == "#text") {
            LayoutText(page, meas, font, node.text, x, y, maxW, size);
            return;
      }
      float mySize = SizeFor(node.tag, size);
      if (IsBlock(node.tag) && node.tag != "document") {
            y += 4.0f;
      }
      for (size_t k : node.kids) {
            LayoutChild(page, meas, font, doc, k, x, y, maxW, mySize);
      }
      if (IsBlock(node.tag) && node.tag != "document") {
            y += mySize * 0.4f;
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
      page.contentH = 0.0f;
}

void Layout(Page &page, TTF_TextEngine *eng, TTF_Font *font,
            const Tess::Html::Document &doc, float x, float y, float maxW) {
      ClearPage(page);
      if (maxW <= 0.0f) {
            return;
      }
      // Scratch text for measuring; shaped per-line texts are created below.
      TTF_Text *meas = TTF_CreateText(eng, font, "", 0);
      if (!meas) {
            return;
      }
      float cursor = y;
      for (size_t k : doc.arena[0].kids) {
            LayoutChild(page, meas, font, doc, k, x, cursor, maxW, 16.0f);
      }
      TTF_DestroyText(meas);

      // Shape every line once, at its own size, dark on white page.
      for (auto &line : page.lines) {
            TTF_SetFontSize(font, line.size);
            line.shaped = TTF_CreateText(eng, font, line.text.c_str(), line.text.size());
            if (line.shaped) {
                  TTF_SetTextColor(line.shaped, 20, 20, 20, 255);
            }
      }
      page.contentH = cursor - y;
}

void Paint(SDL_Renderer *r, TTF_Font *font, Page &page) {
      (void)r;
      float currentSize = -1.0f;
      for (auto &line : page.lines) {
            if (!line.shaped) {
                  continue;
            }
            if (line.size != currentSize) {
                  TTF_SetFontSize(font, line.size);
                  currentSize = line.size;
            }
            TTF_DrawRendererText(line.shaped, line.x, line.y);
      }
}

} // namespace Tess::Render
