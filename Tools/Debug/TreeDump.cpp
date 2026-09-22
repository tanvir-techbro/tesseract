/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* TreeDump - print the HtmlKit document tree for a file, indented */

#include "HtmlKit/Html.hpp"

#include <fstream>
#include <print>
#include <sstream>
#include <string>

static void Dump(const Tess::Html::Document &doc, size_t idx, int depth) {
      const auto &node = doc.arena[idx];
      std::string pad((size_t)depth * 2, ' ');
      std::println("{}<{}> {:?}", pad, node.tag, node.text);
      for (size_t kid : node.kids) {
            Dump(doc, kid, depth + 1);
      }
}

int main(int argc, char *argv[]) {
      if (argc != 2) {
            std::println(stderr, "usage: TreeDump <file.html>");
            return 1;
      }
      std::ifstream file(argv[1]);
      if (!file) {
            std::println(stderr, "cannot open {}", argv[1]);
            return 1;
      }
      std::ostringstream stream;
      stream << file.rdbuf();

      auto toks = Tess::Html::Tokenize(stream.str());
      auto doc = Tess::Html::Parse(toks);
      Dump(doc, 0, 0);
      return 0;
}
