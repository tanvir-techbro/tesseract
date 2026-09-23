/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

#include "NetKit/Net.hpp"
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ranges>
#include <string>

namespace Tess::Net {

static std::optional<uint16_t> ParsePort(const std::string &s) {
      if (s.empty() || s.size() > 5) {
            return std::nullopt;
      }
      for (char c : s) {
            if (!std::isdigit((unsigned char)c)) {
                  return std::nullopt;
            }
      }
      long port = std::stol(s);
      if (port <= 0 || port > 65535) {
            return std::nullopt;
      }
      return (uint16_t)port;
}

std::optional<Url> ParseUrl(const std::string &raw) {
      Url url;
      size_t pos = 0;

      // scheme
      auto scheme_end = raw.find("://");
      if (scheme_end == std::string::npos) {
            return std::nullopt;
      }
      url.scheme = raw.substr(0, scheme_end)
            | std::views::transform([](unsigned char c) { return std::tolower(c); })
            | std::ranges::to<std::string>();
      pos = scheme_end + 3;

      // authority (upto / ? # or end)
      auto auth_end = raw.find_first_of("/?#", pos);
      if (auth_end == std::string::npos) {
            auth_end = raw.size();
      }
      std::string authority = raw.substr(pos, auth_end - pos);
      pos = auth_end;

      // strip userinfo if present
      auto at_pos = authority.find('@');
      if (at_pos != std::string::npos) {
            authority = authority.substr(at_pos + 1); // discarding user:pass for now
      }

      // host + optional port, handling [ipv6] (brackets stripped)
      if (!authority.empty() && authority[0] == '[') {
            auto close_bracket = authority.find(']');
            if (close_bracket == std::string::npos) {
                  return std::nullopt;
            }
            url.host = authority.substr(1, close_bracket - 1);
            if (close_bracket + 1 < authority.size() && authority[close_bracket + 1] == ':') {
                  auto port = ParsePort(authority.substr(close_bracket + 2));
                  if (!port) {
                        return std::nullopt;
                  }
                  url.port = *port;
            }
      } else {
            auto colon_pos = authority.find(':');
            if (colon_pos != std::string::npos) {
                  url.host = authority.substr(0, colon_pos);
                  auto port = ParsePort(authority.substr(colon_pos + 1));
                  if (!port) {
                        return std::nullopt;
                  }
                  url.port = *port;
            } else {
                  url.host = authority;
            }
      }

      // default ports when unspecified
      if (url.port == 0) {
            if (url.scheme == "http") {
                  url.port = 80;
            } else if (url.scheme == "https") {
                  url.port = 443;
            }
      }

      // path
      if (pos < raw.size() && raw[pos] == '/') {
            size_t pathEnd = raw.find_first_of("?#", pos);
            if (pathEnd == std::string::npos)
                  pathEnd = raw.size();
            url.path = raw.substr(pos, pathEnd - pos);
            pos = pathEnd;
      }

      // query
      if (pos < raw.size() && raw[pos] == '?') {
            size_t queryEnd = raw.find('#', pos);
            if (queryEnd == std::string::npos)
                  queryEnd = raw.size();
            url.query = raw.substr(pos + 1, queryEnd - pos - 1);
            pos = queryEnd;
      }

      // fragment
      if (pos < raw.size() && raw[pos] == '#') {
            url.fragment = raw.substr(pos + 1);
      }

      return url;
}

std::optional<Response> FetchResponse(const std::string &url) {
      (void)url;
      return std::nullopt; // TODO: picks file/http/https transport
}

} // namespace Tess::Net
