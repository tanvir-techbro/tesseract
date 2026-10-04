/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Paint.cpp - per-frame replay: blit cached lines, rules, underlines.
   No shaping, no measuring, no font mutation here ever. */

#include "RenderKit/Render.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Tess::Render {

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
