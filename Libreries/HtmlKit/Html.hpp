/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Html.hpp - include for Html rendering engine */

#pragma once

#include <string>
#include <vector>

namespace Tess::Html {

/* Tokenize */
enum class TokenKind {
      TK_Text,     // plain text
      TK_TagOpen,  // <TAG>, opening tag
      TK_TagClose, // </TAG>, closing tag
};

struct Token {
      TokenKind kind;
      std::string text;
};
std::vector<Token> Tokenize(std::string_view src);

/* Parse  */
struct Node {
      std::string tag;          // element name, "#text" for text
      std::string text;         // text content (tags leave empty)
      std::vector<size_t> kids; // indices into Document::arena
};
struct Document {
      std::vector<Node> arena; // arena[0] is always the "document" root
};
Document Parse(const std::vector<Token> &toks);

} // namespace Tess::Html
