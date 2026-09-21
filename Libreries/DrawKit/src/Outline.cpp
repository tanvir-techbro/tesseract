/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

#include "DrawKit/Draw.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <cmath>

namespace Tess::Draw {

void DrawArc(SDL_Renderer *r, float cx, float cy, float radius, float start_deg, float end_deg) {
      for (float a = start_deg; a <= end_deg; a += 2.0f) {
            float rad = a * 3.14159265f / 180.0f;
            SDL_RenderPoint(r, cx + cosf(rad) * radius, cy + sinf(rad) * radius);
      }
}

void DrawRoundedOutline(SDL_Renderer *r, SDL_FRect rect, float radius) {
      float rad = std::min({radius, rect.w * 0.5f, rect.h * 0.5f});

      SDL_RenderLine(r, rect.x + rad, rect.y, rect.x + rect.w - rad, rect.y);
      SDL_RenderLine(r, rect.x + rad, rect.y + rect.h, rect.x + rect.w - rad, rect.y + rect.h);
      SDL_RenderLine(r, rect.x, rect.y + rad, rect.x, rect.y + rect.h - rad);
      SDL_RenderLine(r, rect.x + rect.w, rect.y + rad, rect.x + rect.w, rect.y + rect.h - rad);

      DrawArc(r, rect.x + rad, rect.y + rad, rad, 180.0f, 270.0f);
      DrawArc(r, rect.x + rect.w - rad, rect.y + rad, rad, 270.0f, 360.0f);
      DrawArc(r, rect.x + rect.w - rad, rect.y + rect.h - rad, rad, 0.0f, 90.0f);
      DrawArc(r, rect.x + rad, rect.y + rect.h - rad, rad, 90.0f, 180.0f);
}

void DrawText(SDL_Renderer *r, TTF_Text *txt, const std::string &str, float x, float y) {
      TTF_SetTextString(txt, str.c_str(), 0);
      TTF_DrawRendererText(txt, x, y);
}

void DrawCaret(SDL_Renderer *r, TTF_Text *txt, float x, float y) {
      if ((SDL_GetTicks() / 530) % 2 != 0) {
            return;
      }
      int tw = 0, th = 0;
      TextSize(txt, tw, th);
      SDL_FRect caret = {x + (float)tw, y, 2.0f, (float)th};
      SDL_RenderFillRect(r, &caret);
}

void TextSize(TTF_Text *txt, int &w, int &h) {
      TTF_GetTextSize(txt, &w, &h);
}

} // namespace Tess::Draw
