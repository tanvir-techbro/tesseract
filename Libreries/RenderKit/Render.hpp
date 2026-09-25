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

#include <string>
#include <vector>

namespace Tess::Render {

struct Line {
      std::string text;       // final wrapped line
      float x = 0.0f;         // draw position
      float y = 0.0f;
      float size = 16.0f;     // font size this line was shaped at
      TTF_Text *shaped = nullptr; // owned, created at layout, drawn per frame
};

struct Page {
      std::vector<Line> lines;
      float contentH = 0.0f;
};

// Free all shaped texts. Call before re-layout and at shutdown.
void ClearPage(Page &page);

// Build lines once per content/resize. Shapes every line up front;
// nothing here runs per frame.
void Layout(Page &page, TTF_TextEngine *eng, TTF_Font *font,
            const Tess::Html::Document &doc, float x, float y, float maxW);

// Per frame: draw cached lines only. No shaping, no measuring.
void Paint(SDL_Renderer *r, TTF_Font *font, Page &page);

} // namespace Tess::Render
