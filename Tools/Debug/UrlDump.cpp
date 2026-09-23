/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* UrlDump - print Tess::Net::ParseUrl fields, one URL per argv */

#include "NetKit/Net.hpp"

#include <print>

static void Dump(const char *raw) {
      auto url = Tess::Net::ParseUrl(raw);
      if (!url) {
            std::println("{} -> <invalid>", raw);
            return;
      }
      std::println("{} -> scheme={:?} host={:?} port={} path={:?} query={:?} fragment={:?}",
                   raw, url->scheme, url->host, url->port, url->path, url->query,
                   url->fragment);
}

int main(int argc, char *argv[]) {
      if (argc < 2) {
            std::println(stderr, "usage: UrlDump <url>...");
            return 1;
      }
      for (int i = 1; i < argc; ++i) {
            Dump(argv[i]);
      }
      return 0;
}
