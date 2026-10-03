/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Render.hpp - Rendering kit: layout once, paint per frame */

#pragma once

#include "DrawKit/Draw.hpp"
#include "HtmlKit/Html.hpp"
#include "NetKit/Net.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <map>
#include <string>
#include <vector>

namespace Tess::Render {

// One TTF_Font per size. SDL_ttf ties shaping to the font object, so
// sharing one font across sizes + SetFontSize per draw re-shapes (and
// leaks) every frame. This makes size switches free and stable.
struct Typeface {
      TTF_TextEngine *eng = nullptr;
      std::string path;
      std::string bold_path;
      std::map<float, TTF_Font *> fonts;      // owned, closed in dtor
      std::map<float, TTF_Font *> bold_fonts; // owned, closed in dtor

      Typeface() = default;
      Typeface(TTF_TextEngine *eng, std::string path, std::string bold_path = "");
      ~Typeface();
      Typeface(const Typeface &) = delete;
      Typeface &operator=(const Typeface &) = delete;

      // Close all cached fonts. Must run before TTF_Quit; the dtor
      // re-runs it harmlessly if you forget (maps are cleared).
      void Close();

      // Borrowed font at exact size + weight, opening + caching on first use.
      // Bold falls back to regular when no bold file was given.
      // Null when the file cannot be opened.
      TTF_Font *At(float size, bool bold = false);
};

struct Line {
      std::string text;
      float x = 0.0f;
      float y = 0.0f;
      float w = 0.0f; // measured width, for underlines + hit rects
      float h = 0.0f; // measured height
      float size = 16.0f;
      bool rule = false; // <hr>: draw a line, no text
      std::string link;  // resolved target, empty = plain
      TTF_Font *font = nullptr;
      TTF_Text *shaped = nullptr;
};
struct LinkRect {
      SDL_FRect rect;
      std::string target;
};
struct Page {
      std::vector<Line> lines;
      std::vector<LinkRect> hits; // rebuilt at layout, layout coords
      float content_h = 0.0f;
      bool dark = false; // error pages: light text, Tess paints dark bg
};

// Free all shaped texts. Call before re-layout and at shutdown.
void ClearPage(Page &page);

// Build lines once per content/resize. Shapes every line up front;
// nothing here runs per frame.
void Layout(Page &page, Typeface &face, const Tess::Html::Document &doc, float x, float y,
            float max_w, const Tess::Net::Url &base);

// Per frame: draw cached lines only. No shaping, no measuring,
// no font mutation. scroll offsets content upward (clipped).
void Paint(SDL_Renderer *r, Page &page, float scroll);

} // namespace Tess::Render
