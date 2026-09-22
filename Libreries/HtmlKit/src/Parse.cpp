/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Parse.cpp - Html parser: token stream -> arena document tree */

#include <HtmlKit/Html.hpp>
#include <algorithm>
#include <cctype>
#include <vector>

namespace Tess::Html {

Document Parse(const std::vector<Token> &toks) {
      Document doc;
      doc.arena.push_back(Node{.tag = "document"});
      auto IsBlank = [](const std::string &s) {
            return std::all_of(s.begin(), s.end(), [](unsigned char c) {
                  return std::isspace(c) != 0;
            });
      };

      std::vector<size_t> stack;
      stack.push_back(0);

      for (auto &t : toks) {
            if (t.kind == TokenKind::TK_Text) {
                  if (IsBlank(t.text)) {
                        continue;
                  }
                  size_t idx = doc.arena.size();
                  doc.arena.push_back(Node{.tag = "#text", .text = t.text});
                  doc.arena[stack.back()].kids.push_back(idx);
            }
            // <>
            else if (t.kind == TokenKind::TK_TagOpen) {
                  size_t idx = doc.arena.size();
                  doc.arena.push_back(Node{.tag = t.text});
                  doc.arena[stack.back()].kids.push_back(idx);
                  stack.push_back(idx);
            }
            // </>: pop to nearest matching open, ignore if none
            else if (t.kind == TokenKind::TK_TagClose) {
                  for (size_t d = stack.size(); d-- > 1;) {
                        if (doc.arena[stack[d]].tag == t.text) {
                              stack.resize(d);
                              break;
                        }
                  }
            }
      }

      return doc;
}

} // namespace Tess::Html
