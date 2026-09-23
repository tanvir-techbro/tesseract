/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Net.cpp - NetKit networking stack */

#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace Tess::Net {

/* URL
  https://user:pass@example.com:8443/path/to/page?query=1&foo=bar#section
  \___/   \_______/ \_________/ \__/\____________/ \____________/ \_____/
  scheme   userinfo    host     port     path          query      fragment
*/
struct Url {
      std::string scheme;
      std::string host;
      std::string path;
      std::string query;
      std::string fragment;
      uint16_t port = 0; // 0 = "not specified"
};
std::optional<Url> ParseUrl(const std::string &raw);

/* Fetch Response */
struct Response {
      int status;
      std::string body;
};
std::optional<Response> FetchResponse(const Url &url);

} // namespace Tess::Net
