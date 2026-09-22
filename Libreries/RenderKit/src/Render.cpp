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

/* Generic Helpers */
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
      return tag == "document" || tag == "div" || tag == "p" || tag == "h1" || tag == "h2" ||
             tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6" || tag == "li" ||
             tag == "ul";
}

/* Rendering Helpers */
void RenderText(SDL_Renderer *r, TTF_Text *txt, TTF_Font *font,
                const std::string &text, float x, float &y, float maxW, float size) {
      TTF_SetFontSize(font, size);
      std::string line;
      std::istringstream words(CleanText(text));
      std::string word;

      auto flush = [&] {
            if (line.empty()) {
                  return;
            }
            Tess::Draw::DrawText(r, txt, line, x, y);
            int w = 0, h = 0;
            Tess::Draw::TextSize(txt, w, h);
            y += (float)h + 2.0f;
            line.clear();
      };

      while (words >> word) {
            std::string trial = line.empty() ? word : line + " " + word;
            TTF_SetTextString(txt, trial.c_str(), 0);
            int w = 0, h = 0;
            Tess::Draw::TextSize(txt, w, h);
            if ((float)w > maxW && !line.empty())
                  flush();
            line = line.empty() ? word : line + " " + word;
      }

      flush();
}
void RenderChild(SDL_Renderer *r, TTF_Text *txt, TTF_Font *font, const Html::Document &doc,
                 size_t idx, float x, float &y, float maxW, float size) {
      const auto &node = doc.arena[idx];
      if (node.tag == "#text") {
            RenderText(r, txt, font, node.text, x, y, maxW, size);
            return;
      }
      float mySize = SizeFor(node.tag, size);
      if (IsBlock(node.tag) && node.tag != "document") {
            y += 4.0f;
      }
      for (size_t k : node.kids) {
            RenderChild(r, txt, font, doc, k, x, y, maxW, mySize);
      }
      if (IsBlock(node.tag) && node.tag != "document") {
            y += mySize * 0.4f;
      }
}

} // namespace

/* Main */
void Render(SDL_Renderer *r, TTF_Text *txt, TTF_Font *font,
            const Tess::Html::Document &doc, SDL_FRect page) {
      TTF_SetTextColor(txt, 20, 20, 20, 255);
      float y = page.y + 8;

      // recursively render child
      for (size_t k : doc.arena[0].kids) {
            RenderChild(r, txt, font, doc, k, page.x + 8, y, page.w - 16, 16.0f);
      }
}

} // namespace Tess::Render
