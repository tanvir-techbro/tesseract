/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Layout.cpp - tree walk: tags to geometry. Owns block gaps, indent,
   sizes, links, and the Layout()/ClearPage() entry points. */

#include "RenderKit/Render.hpp"
#include "RenderKit/src/Internal.hpp"

#include <string>

namespace Tess::Render {

/* Helper */
namespace {

float SizeFor(const std::string &tag, float inherited) {
      if (tag == "h1") {
            return 28.0f;
      }
      if (tag == "h2") {
            return 24.0f;
      }
      if (tag == "h3") {
            return 20.0f;
      }
      if (tag == "h4") {
            return 18.0f;
      }
      if (tag == "h5") {
            return 16.0f;
      }
      if (tag == "h6") {
            return 13.0f;
      }
      return inherited;
}
bool IsBlock(const std::string &tag) {
      return tag == "document" || tag == "div" || tag == "p" || tag == "pre" || tag == "h1"
            || tag == "h2" || tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6"
            || tag == "li" || tag == "ul" || tag == "ol" || tag == "dl" || tag == "dt"
            || tag == "dd" || tag == "blockquote";
}
bool BoldFor(const std::string &tag, bool inherited) {
      if (tag == "h1" || tag == "h2" || tag == "h3" || tag == "h4" || tag == "h5" || tag == "h6"
          || tag == "b" || tag == "strong") {
            return true;
      }
      return inherited;
}
void LayoutChild(Page &page, Typeface &face, Typeface &mono, const Html::Document &doc, size_t idx,
                 float x, float &y, float max_w, float size, bool bold, const Tess::Net::Url &base,
                 const std::string &link, bool pre) {
      const auto &node = doc.arena[idx];
      if (node.tag == "#text") {
            LayoutText(page, pre ? mono : face, node.text, x, y, max_w, size, bold, link, pre);
            return;
      }
      if (node.tag == "head") {
            return; // non-visual: title/meta/link never paint
      }
      float my_size = SizeFor(node.tag, size);
      bool my_bold = BoldFor(node.tag, bold);
      bool my_pre = pre || node.tag == "pre";
      // Indented blocks: whole subtree shifts right, wrap width shrinks.
      float my_x = x;
      float my_w = max_w;
      if (node.tag == "dd" || node.tag == "blockquote") {
            my_x += 40.0f;
            my_w -= 40.0f;
            if (my_w < 40.0f) {
                  my_w = 40.0f;
            }
      }
      if (node.tag == "br") {
            y += my_size * 1.2f;
            return;
      }
      if (node.tag == "hr") {
            y += 4.0f;
            page.lines.push_back(Line{
                  .x = my_x,
                  .y = y,
                  .w = my_w,
                  .h = 2.0f,
                  .size = my_size,
                  .rule = true,
            });
            y += 6.0f;
            return;
      }
      std::string my_link = link;
      if (node.tag == "a") {
            auto it = node.attributes.find("href");
            if (it != node.attributes.end()) {
                  my_link = Tess::Net::Resolve(base, it->second);
            }
      }
      // Compact definition lists: dt/dd skip the generic block gaps.
      bool compact = (node.tag == "dt" || node.tag == "dd");
      if (!compact && IsBlock(node.tag) && node.tag != "document") {
            y += 4.0f;
      }
      for (size_t k : node.kids) {
            LayoutChild(page, face, mono, doc, k, my_x, y, my_w, my_size, my_bold, base, my_link,
                        my_pre);
      }
      if (compact) {
            if (node.tag == "dd") {
                  y += 2.0f;
            }
      } else if (IsBlock(node.tag) && node.tag != "document") {
            y += my_size * 0.4f;
      }
}

} // namespace

/* Main */
void ClearPage(Page &page) {
      for (auto &line : page.lines) {
            if (line.shaped) {
                  TTF_DestroyText(line.shaped);
                  line.shaped = nullptr;
            }
      }
      page.lines.clear();
      page.hits.clear();
      page.content_h = 0.0f;
}

void Layout(Page &page, Typeface &face, Typeface &mono, const Tess::Html::Document &doc, float x,
            float y, float max_w, const Tess::Net::Url &base) {
      ClearPage(page);
      if (max_w <= 0.0f) {
            return;
      }
      float cursor = y;
      for (size_t k : doc.arena[0].kids) {
            LayoutChild(page, face, mono, doc, k, x, cursor, max_w, 16.0f, false, base, "", false);
      }
      page.content_h = cursor - y;
}

} // namespace Tess::Render
