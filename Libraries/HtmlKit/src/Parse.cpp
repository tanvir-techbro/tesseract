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
            return std::all_of(s.begin(), s.end(),
                               [](unsigned char c) { return std::isspace(c) != 0; });
      };

      std::vector<size_t> stack;
      stack.push_back(0);

      // Raw-text elements: contents are code, never page text.
      auto IsRaw = [](const std::string &tag) {
            return tag == "style" || tag == "script" || tag == "noscript";
      };
      // Optional end tags: opening one auto-closes an open sibling.
      auto AutoCloses = [](const std::string &open, const std::string &tag) {
            if (open == "dt" || open == "dd") {
                  return tag == "dt" || tag == "dd";
            }
            if (open == "li") {
                  return tag == "li";
            }
            if (open == "p") {
                  return tag == "p";
            }
            return false;
      };
      // Void elements: no close tag, never take kids.
      auto IsVoid = [](const std::string &tag) {
            return tag == "br" || tag == "hr" || tag == "img" || tag == "meta" || tag == "link"
                  || tag == "input" || tag == "isindex";
      };

      for (size_t i = 0; i < toks.size(); ++i) {
            const auto &t = toks[i];
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
                  if (IsRaw(t.text)) {
                        // Skip to matching close (depth-counted for safety).
                        size_t depth = 1;
                        while (++i < toks.size() && depth > 0) {
                              if (toks[i].kind == TokenKind::TK_TagOpen && toks[i].text == t.text) {
                                    ++depth;
                              } else if (toks[i].kind == TokenKind::TK_TagClose
                                         && toks[i].text == t.text) {
                                    --depth;
                              }
                        }
                        continue;
                  }
                  // Implicit close first: <dt> ends an open <dd>, etc.
                  while (stack.size() > 1 && AutoCloses(t.text, doc.arena[stack.back()].tag)) {
                        stack.pop_back();
                  }
                  size_t idx = doc.arena.size();
                  doc.arena.push_back(Node{.tag = t.text, .attributes = t.attributes});
                  doc.arena[stack.back()].kids.push_back(idx);
                  if (!IsVoid(t.text)) {
                        stack.push_back(idx);
                  }
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

std::string TitleOf(const Document &doc) {
      // Depth-first, first title. Text may arrive split across nodes.
      std::string out;
      bool found = false;
      auto visit = [&](auto &&self, size_t idx) -> void {
            if (found) {
                  return;
            }
            const Node &node = doc.arena[idx];
            if (node.tag == "title") {
                  found = true;
                  for (size_t k : node.kids) {
                        if (doc.arena[k].tag == "#text") {
                              out += doc.arena[k].text;
                        }
                  }
                  return;
            }
            for (size_t k : node.kids) {
                  self(self, k);
            }
      };
      if (!doc.arena.empty()) {
            visit(visit, 0);
      }
      // Collapse whitespace runs: titles span source lines.
      std::string clean;
      clean.reserve(out.size());
      bool space = true;
      for (char c : out) {
            bool ws = c == ' ' || c == '\n' || c == '\r' || c == '\t';
            if (ws) {
                  if (!space) {
                        clean += ' ';
                        space = true;
                  }
            } else {
                  clean += c;
                  space = false;
            }
      }
      if (!clean.empty() && clean.back() == ' ') {
            clean.pop_back();
      }
      return clean;
}

} // namespace Tess::Html
