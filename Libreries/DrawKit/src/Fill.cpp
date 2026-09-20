/*
** Tesseract - A simple 2D browserlike rendering engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

#include "DrawKit/Draw.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace Tess::Draw {

void FillCircle(SDL_Renderer *r, float cx, float cy, float radius) {
      if (radius <= 0.0f) {
            return;
      }

      int rows = (int)(radius * 2.0f + 1.0f);
      std::vector<SDL_FRect> rects;
      rects.reserve(rows);

      for (float dy = -radius; dy <= radius; dy += 1.0f) {
            float dx = sqrtf(radius * radius - dy * dy);
            rects.push_back({cx - dx, cy + dy, dx * 2.0f, 1.0f});
      }
      SDL_RenderFillRects(r, rects.data(), (int)rects.size());
}

void FillRoundedRect(SDL_Renderer *r, SDL_FRect rect, float radius) {
      float rad = std::min({radius, rect.w * 0.5f, rect.h * 0.5f});
      SDL_FRect midH = {rect.x + rad, rect.y, rect.w - 2.0f * rad, rect.h};
      SDL_FRect midV = {rect.x, rect.y + rad, rect.w, rect.h - 2.0f * rad};
      SDL_RenderFillRect(r, &midH);
      SDL_RenderFillRect(r, &midV);
      FillCircle(r, rect.x + rad, rect.y + rad, rad);
      FillCircle(r, rect.x + rect.w - rad, rect.y + rad, rad);
      FillCircle(r, rect.x + rad, rect.y + rect.h - rad, rad);
      FillCircle(r, rect.x + rect.w - rad, rect.y + rect.h - rad, rad);
}

} // namespace Tess::Draw
