/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

#include "RenderKit/Render.hpp"
#include <string>

namespace Tess::Render {

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

} // namespace Tess::Render
