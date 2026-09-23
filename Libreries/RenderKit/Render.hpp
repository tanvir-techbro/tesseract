/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Render.hpp - Rendering kit */

#pragma once

#include "DrawKit/Draw.hpp"
#include "HtmlKit/Html.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Tess::Render {

void Render(SDL_Renderer *r, TTF_Text *txt, TTF_Font *font,
            const Tess::Html::Document &doc, SDL_FRect page);

} // namespace Tess::Render
