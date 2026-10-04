/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Text.cpp - word wrap + pre verbatim. Turns one text run into
   shaped page Lines. Called only from Layout.cpp via Internal.hpp. */

#include "RenderKit/Render.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <sstream>
#include <string>

namespace Tess::Render {

/* Helper */
namespace {

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

} // namespace

void LayoutText(Page &page, Typeface &face, const std::string &text, float x, float &y, float max_w,
                float size, bool bold, const std::string &link, bool pre) {
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

      if (!pre) {
            while (words >> word) {
                  std::string trial = line.empty() ? word : line + " " + word;
                  if ((float)measure(trial) > max_w && !line.empty()) {
                        flush();
                  }
                  line = line.empty() ? word : line + " " + word;
            }
            flush();
      } else {
            // Verbatim: one visual line per source line, spaces kept.
            std::string cur;
            auto emit = [&] {
                  // Strip a single trailing '\r' (CRLF sources).
                  if (!cur.empty() && cur.back() == '\r') {
                        cur.pop_back();
                  }
                  int w = 0, h = 0;
                  TTF_SetTextString(meas, cur.c_str(), cur.size());
                  Tess::Draw::TextSize(meas, w, h);
                  TTF_Text *shaped = TTF_CreateText(face.eng, font, cur.c_str(), cur.size());
                  if (shaped) {
                        if (page.dark) {
                              TTF_SetTextColor(shaped, 220, 220, 220, 255);
                        } else {
                              TTF_SetTextColor(shaped, 20, 20, 20, 255);
                        }
                  }
                  page.lines.push_back(Line{.text = cur,
                                            .x = x,
                                            .y = y,
                                            .w = (float)w,
                                            .h = (float)h,
                                            .size = size,
                                            .font = font,
                                            .shaped = shaped});
                  y += (float)h + 2.0f;
            };
            for (char c : text) {
                  if (c == '\n') {
                        emit();
                        cur.clear();
                  } else {
                        cur += c;
                  }
            }
            emit();
      }
      TTF_DestroyText(meas);
}

} // namespace Tess::Render
