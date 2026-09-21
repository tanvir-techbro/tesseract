/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Draw wrapper librery for helping in rendering */

#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace Tess::Draw {

/* Outline.cpp */
void DrawArc(SDL_Renderer *r, float cx, float cy, float radius, float start_deg, float end_deg);
void DrawRoundedOutline(SDL_Renderer *r, SDL_FRect rect, float radius);
void DrawText(SDL_Renderer *r, TTF_Text *txt, const std::string &str, float x, float y);
void DrawCaret(SDL_Renderer *r, TTF_Text *txt, float x, float y);
void TextSize(TTF_Text *txt, int &w, int &h);

/* Fill.cpp */
void FillCircle(SDL_Renderer *r, float cx, float cy, float radius);
void FillRoundedRect(SDL_Renderer *r, SDL_FRect rect, float radius);

} // namespace Tess::Draw
