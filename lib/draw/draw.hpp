/*
** Tesseract - A simple 2D browserlike rendering engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Draw wrapper librery for helping in rendering */

#pragma once

#include <SDL3/SDL.h>

namespace tesseract::draw {
/* drawDraw.cpp */
void DrawArc(SDL_Renderer *r, float cx, float cy, float radius, float start_deg, float end_deg);
void DrawRoundedOutline(SDL_Renderer *r, SDL_FRect rect, float radius);
/* drawFill.cpp */
void FillCircle(SDL_Renderer *r, float cx, float cy, float radius);
void FillRoundedRect(SDL_Renderer *r, SDL_FRect rect, float radius);
} // namespace tesseract::draw
