/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* LexDump - print the HtmlKit token stream for a file, one token per line */

#include "HtmlKit/Html.hpp"

#include <fstream>
#include <print>
#include <sstream>

static const char *KindName(Tess::Html::TokenKind kind) {
      using Tess::Html::TokenKind;
      switch (kind) {
      case TokenKind::TK_Text:
            return "Text";
      case TokenKind::TK_TagOpen:
            return "TagOpen";
      case TokenKind::TK_TagClose:
            return "TagClose";
      }
      return "?";
}

int main(int argc, char *argv[]) {
      if (argc != 2) {
            std::println(stderr, "usage: lexdump <file.html>");
            return 1;
      }
      std::ifstream file(argv[1]);
      if (!file) {
            std::println(stderr, "cannot open {}", argv[1]);
            return 1;
      }
      std::ostringstream stream;
      stream << file.rdbuf();

      for (const auto &tok : Tess::Html::Tokenize(stream.str())) {
            std::println("{} : {:?}", KindName(tok.kind), tok.text);
      }
      return 0;
}
