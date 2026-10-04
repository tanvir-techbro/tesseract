/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Internal.hpp - RenderKit-internal seam, NOT public API.
   Text.cpp defines LayoutText, Layout.cpp calls it. Anything here
   is free to change shape without touching Render.hpp. */

#pragma once

#include "RenderKit/Render.hpp"

#include <string>

namespace Tess::Render {

// Wrap one text run into page lines (layout time only).
void LayoutText(Page &page, Typeface &face, const std::string &text, float x, float &y, float max_w,
                float size, bool bold, const std::string &link, bool pre);

} // namespace Tess::Render
