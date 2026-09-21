/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Tokenize.cpp - Html tokenizer */

#include "HtmlKit/Html.hpp"
#include <cctype>
#include <cstddef>
#include <string_view>
#include <vector>

namespace Tess::Html {

/* Helper */
static bool IsName(char c) {
      return std::isalnum((unsigned char)c);
}

static std::string Lower(std::string_view s) {
      std::string r;
      r.reserve(s.size());
      for (char c : s) {
            r += (char)std::tolower((unsigned char)c);
      }
      return r;
}

/* Main */
std::vector<Token> Tokenize(std::string_view src) {
      std::vector<Token> out;

      while (!src.empty()) {
            // skip comment
            if (src.starts_with("<!--")) {
                  size_t e = src.find("-->", 4);
                  size_t skip = (e == std::string_view::npos) ? src.size() : e + 3;
                  src.remove_prefix(skip);
            }
            // closing tag
            else if (src.starts_with("</")) {
                  src.remove_prefix(2);

                  size_t name_len = 0;
                  while (name_len < src.size() && IsName(src[name_len])) {
                        ++name_len;
                  }
                  std::string_view name = src.substr(0, name_len);

                  // Skip to and past the closing '>'
                  size_t gt = src.find('>');
                  size_t skip = (gt == std::string_view::npos) ? src.size() : gt + 1;
                  src.remove_prefix(skip);

                  if (!name.empty()) {
                        out.push_back({TokenKind::TK_TagClose, Lower(name)});
                  }
            }
            // opening tag: '<' followed by a letter, else it's plain text
            else if (src.size() > 1 && src[0] == '<' && std::isalpha((unsigned char)src[1])) {
                  src.remove_prefix(1); // Skip '<'

                  size_t name_len = 0;
                  while (name_len < src.size() && IsName(src[name_len])) {
                        ++name_len;
                  }
                  std::string_view name = src.substr(0, name_len);

                  // Skip attributes to '>'
                  size_t gt = src.find('>');
                  size_t skip = (gt == std::string_view::npos) ? src.size() : gt + 1;
                  src.remove_prefix(skip);

                  if (!name.empty()) {
                        out.push_back({TokenKind::TK_TagOpen, Lower(name)});
                  }
            }
            // text
            else {
                  // text up to the next '<'; a lone '<' becomes literal text
                  size_t j = src.find('<');
                  size_t text_len = (j == std::string_view::npos) ? src.size() : j;
                  if (text_len == 0) {
                        text_len = 1;
                  }
                  out.push_back({TokenKind::TK_Text, std::string(src.substr(0, text_len))});
                  src.remove_prefix(text_len);
            }
      }

      return out;
}

} // namespace Tess::Html
