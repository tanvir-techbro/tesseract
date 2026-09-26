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

#include <SDL3/SDL.h>
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
      std::map<float, TTF_Font *> fonts; // owned, closed in dtor

      Typeface() = default;
      Typeface(TTF_TextEngine *eng, std::string path);
      ~Typeface();
      Typeface(const Typeface &) = delete;
      Typeface &operator=(const Typeface &) = delete;

      // Close all cached fonts. Must run before TTF_Quit; the dtor
      // re-runs it harmlessly if you forget (map is cleared).
      void Close();

      // Borrowed font at exact size, opening + caching on first use.
      // Null when the file cannot be opened.
      TTF_Font *At(float size);
};

struct Line {
      std::string text; // final wrapped line
      float x = 0.0f;   // draw position
      float y = 0.0f;
      float size = 16.0f;
      TTF_Font *font = nullptr;   // borrowed from Typeface, never null
      TTF_Text *shaped = nullptr; // owned, created at layout, drawn per frame
};

struct Page {
      std::vector<Line> lines;
      float content_h = 0.0f;
};

// Free all shaped texts. Call before re-layout and at shutdown.
void ClearPage(Page &page);

// Build lines once per content/resize. Shapes every line up front;
// nothing here runs per frame.
void Layout(Page &page, Typeface &face, const Tess::Html::Document &doc, float x, float y, float max_w);

// Per frame: draw cached lines only. No shaping, no measuring,
// no font mutation.
void Paint(SDL_Renderer *r, Page &page);

} // namespace Tess::Render
