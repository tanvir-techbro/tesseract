/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

#include "NetKit/Net.hpp"
#include <fstream>
#include <optional>
#include <print>
#include <sstream>

namespace Tess::Net {

/* Helper */
static std::optional<Response> FetchFile(const Url &url) {
      if (!url.host.empty() && url.host != "localhost") {
            return std::nullopt;
      }
      std::ifstream file(url.path, std::ios::binary);
      if (!file) {
            return std::nullopt;
      }
      std::ostringstream s;
      s << file.rdbuf();

      return Response{200, s.str()};
}

/* Main */
std::optional<Response> FetchResponse(const Url &url) {
      if (url.scheme == "file") {
            return FetchFile(url);
      }

      return std::nullopt; // TODO: http/https transport
}

} // namespace Tess::Net
