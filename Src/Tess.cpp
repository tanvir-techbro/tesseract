/*
** Tesseract - A simple browser engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* Tess.cpp file - Main */

#include "DrawKit/Draw.hpp"
#include "HtmlKit/Html.hpp"
#include "NetKit/Net.hpp"
#include "Pages/CanNotReach.hpp"
#include "RenderKit/Render.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {

      /* Window Initialization */
      if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::println(stderr, "SDL Init failed: {}", SDL_GetError());
            return 1;
      }
      if (!TTF_Init()) {
            std::println(stderr, "TTF Init failed: {}", SDL_GetError());
            return 1;
      }

      int window_width = 800;
      int window_height = 600;

      SDL_Window *window
            = SDL_CreateWindow("tesseract", window_width, window_height, SDL_WINDOW_RESIZABLE);
      if (!window) {
            std::println(stderr, "Window creation failed: {}", SDL_GetError());
            SDL_Quit();
            return 1;
      }
      SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
      if (!renderer) {
            std::println(stderr, "Renderer creation failed: {}", SDL_GetError());
            SDL_Quit();
            return 1;
      }

      // Resource lookup: exe dir first (installed / any CWD), then dev fallbacks.
      // rel includes "Assets/", e.g. "Assets/fonts/sans.ttf".
      auto AssetPath = [](const std::string &rel) {
            // NOTE: SDL3 GetBasePath returns a borrowed pointer here; do NOT
            // free it (freeing crashes on second call). If a future SDL
            // version documents ownership, revisit.
            if (const char *base = SDL_GetBasePath()) {
                  std::string p = std::string(base) + rel;
                  std::ifstream probe(p, std::ios::binary);
                  if (probe) {
                        return p;
                  }
            }
            for (const std::string &cand : {rel, std::string("../") + rel}) {
                  std::ifstream probe(cand, std::ios::binary);
                  if (probe) {
                        return cand;
                  }
            }
            return rel; // let the opener report it
      };

      TTF_Font *font
            = TTF_OpenFont(AssetPath("Assets/fonts/CaskaydiaCoveNerdFontMono-Regular.ttf").c_str(), 16);
      if (!font) {
            std::println(stderr, "font failed: {}", SDL_GetError());
      }
      TTF_TextEngine *eng = TTF_CreateRendererTextEngine(renderer);
      TTF_Text *txt = TTF_CreateText(eng, font, "", 0);
      TTF_SetTextColor(txt, 255, 255, 255, 255);
      // Page prose typeface: sans fallback (no CSS yet, monospace stays in chrome)
      std::string page_font_path = AssetPath("Assets/fonts/NotoSans-Regular.ttf");
      std::string page_bold_path = AssetPath("Assets/fonts/NotoSans-Bold.ttf");
      {
            std::ifstream probe(page_font_path, std::ios::binary);
            if (!probe) { // sans missing: fall back to mono for everything
                  std::println(stderr, "sans font missing, using mono: {}", SDL_GetError());
                  page_font_path = AssetPath("Assets/fonts/CaskaydiaCoveNerdFontMono-Regular.ttf");
                  page_bold_path = AssetPath("Assets/fonts/CaskaydiaCoveNerdFontMono-Bold.ttf");
            }
      }
      Tess::Render::Typeface page_face(eng, page_font_path, page_bold_path);

      /* Main drawing and events and element stuff */
      bool running = true;
      SDL_Event event;
      SDL_StartTextInput(window);

      Tess::Draw::TextField url_bar;
      url_bar.focused = true;
      float scroll_y = 0.0f;

      // Page source: navigated from the URL bar on Enter (sample first)
      Tess::Html::Document doc = Tess::Html::Parse(
            Tess::Html::Tokenize("<h1>tesseract</h1><p>type a file:// url, hit enter</p>"));
      Tess::Render::Page content;
      bool page_dirty = true;
      Tess::Net::Url base_url; // page links resolve against this
      std::string base_raw;
      std::vector<std::string> back_hist;
      std::vector<std::string> fwd_hist;

      // Returns the navigated-to raw URL, empty when it failed.
      // Failures paint a dark error page instead of only logging.
      auto ShowError = [&](const std::string &raw, Tess::Error::Err err, int http_status) {
            doc = Tess::Html::Parse(
                  Tess::Html::Tokenize(Tess::Pages::CannotReachHtml(raw, err, http_status)));
            content.dark = true;
            page_dirty = true;
            scroll_y = 0.0f;
      };
      // Window title follows <title>, falling back to the app name.
      auto SyncTitle = [&] {
            std::string title = Tess::Html::TitleOf(doc);
            if (title.empty()) {
                  title = "tesseract";
            }
            SDL_SetWindowTitle(window, title.c_str());
      };
      auto Navigate = [&](const std::string &raw, bool push_hist) -> std::string {
            std::string target = raw;
            if (target.find("://") == std::string::npos) {
                  std::error_code ec;
                  target = "file://" + std::filesystem::absolute(target, ec).string();
            }
            auto u = Tess::Net::ParseUrl(target);
            if (!u) {
                  std::println(stderr, "bad url: {}", raw);
                  ShowError(raw, Tess::Error::Err::ERR_INVALID_URL, 0);
                  return "";
            }
            Tess::Error::Err err = Tess::Error::Err::ERR_UNKNOWN;
            auto res = Tess::Net::FetchResponse(*u, err);
            if (!res) {
                  std::println(stderr, "fetch failed ({}): {}", Tess::Error::Name(err), raw);
                  ShowError(raw, err, 0);
                  return "";
            }
            if (res->status != 200) {
                  std::println(stderr, "http {}: {}", res->status, raw);
                  Tess::Error::Err status_err = res->status == 404 ? Tess::Error::Err::ERR_NOT_FOUND
                                                                   : Tess::Error::Err::ERR_UNKNOWN;
                  ShowError(raw, status_err, res->status);
                  return "";
            }
            if (push_hist && !base_raw.empty()) {
                  back_hist.push_back(base_raw);
                  fwd_hist.clear();
            }
            doc = Tess::Html::Parse(Tess::Html::Tokenize(res->body));
            content.dark = false;
            base_url = *u;
            base_raw = target;
            page_dirty = true;
            scroll_y = 0.0f;
            SyncTitle();
            return target;
      };
      SyncTitle(); // sample page title at startup

      while (running) {
            // Chrome slots: [back][forward] url... [menu], 24px bar at y=5
            SDL_FRect back_box = {5.0f, 5.0f, 24.0f, 24.0f};
            SDL_FRect fwd_box = {31.0f, 5.0f, 24.0f, 24.0f};
            SDL_FRect menu_box = {(float)window_width - 39.0f, 5.0f, 34.0f, 24.0f};
            SDL_FRect url_box = {
                  57.0f,
                  5.0f,
                  (float)window_width - 57.0f - 41.0f,
                  28.0f,
            };
            auto HitBox = [](SDL_FRect b, float mx, float my) {
                  return mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h;
            };

            while (SDL_PollEvent(&event)) {
                  if (event.type == SDL_EVENT_QUIT) {
                        running = false;
                  }
                  // 1. Update window dimensions when resized
                  else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                        window_width = event.window.data1;
                        window_height = event.window.data2;
                        page_dirty = true;
                  } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                        scroll_y -= event.wheel.y * 40.0f;
                  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                        float mx = event.button.x;
                        float my = event.button.y;
                        if (HitBox(back_box, mx, my)) {
                              if (!back_hist.empty()) {
                                    fwd_hist.push_back(base_raw);
                                    std::string prev = back_hist.back();
                                    back_hist.pop_back();
                                    url_bar.value = prev;
                                    Navigate(prev, false);
                              }
                        } else if (HitBox(fwd_box, mx, my)) {
                              if (!fwd_hist.empty()) {
                                    back_hist.push_back(base_raw);
                                    std::string next = fwd_hist.back();
                                    fwd_hist.pop_back();
                                    url_bar.value = next;
                                    Navigate(next, false);
                              }
                        } else if (HitBox(menu_box, mx, my)) {
                              // TODO(settings): open menu when settings exist
                        } else {
                              // Page links first (layout coords), then the field.
                              bool linked = false;
                              float ly = my + scroll_y;
                              for (const auto &hit : content.hits) {
                                    if (mx >= hit.rect.x && mx <= hit.rect.x + hit.rect.w
                                        && ly >= hit.rect.y && ly <= hit.rect.y + hit.rect.h) {
                                          url_bar.value = hit.target;
                                          Navigate(hit.target, true);
                                          linked = true;
                                          break;
                                    }
                              }
                              if (!linked) {
                                    Tess::Draw::FieldEvent(url_bar, event, url_box, txt);
                              }
                        }
                  } else if (Tess::Draw::FieldEvent(url_bar, event, url_box, txt)) {
                        if (url_bar.submitted) {
                              url_bar.submitted = false;
                              if (!url_bar.value.empty()) {
                                    Navigate(url_bar.value, true);
                              }
                        }
                  }
            }

            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
            SDL_RenderClear(renderer);

            // URL bar owns white/16pt; page text uses its own Typeface
            TTF_SetFontSize(font, 16);
            // Chrome buttons: lit when their history stack is non-empty
            if (back_hist.empty()) {
                  TTF_SetTextColor(txt, 100, 100, 100, 255);
            } else {
                  TTF_SetTextColor(txt, 255, 255, 255, 255);
            }
            Tess::Draw::DrawText(renderer, txt, "←", back_box.x + 6, back_box.y + 2);
            if (fwd_hist.empty()) {
                  TTF_SetTextColor(txt, 100, 100, 100, 255);
            } else {
                  TTF_SetTextColor(txt, 255, 255, 255, 255);
            }
            Tess::Draw::DrawText(renderer, txt, "→", fwd_box.x + 6, fwd_box.y + 2);
            TTF_SetFontSize(font, 20);
            TTF_SetTextColor(txt, 255, 255, 255, 255);
            Tess::Draw::DrawText(renderer, txt, "≡", menu_box.x + 9, menu_box.y + 1);
            TTF_SetFontSize(font, 16);
            TTF_SetTextColor(txt, 255, 255, 255, 255);
            Tess::Draw::FieldDraw(url_bar, renderer, txt, url_box);

            // Page viewport: 5px under URL bar, 5px off left/right/bottom
            float page_y = url_box.y + url_box.h + 5.0f;
            SDL_FRect page = {
                  5.0f,
                  page_y,
                  (float)window_width - 10.0f,
                  (float)window_height - page_y - 5.0f,
            };
            if (page.w < 0.0f) {
                  page.w = 0.0f;
            }
            if (page.h < 0.0f) {
                  page.h = 0.0f;
            }
            // Error pages go dark grey like chrome, normal pages white.
            if (content.dark) {
                  SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            } else {
                  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            SDL_RenderFillRect(renderer, &page);

            // Clip all page content to the viewport
            SDL_Rect clip = {(int)page.x, (int)page.y, (int)page.w, (int)page.h};
            SDL_SetRenderClipRect(renderer, &clip);

            // Layout only on content/resize; paint replays cached lines
            if (page_dirty) {
                  Tess::Render::Layout(content, page_face, doc, page.x + 8, page.y + 8, page.w - 16,
                                       base_url);
                  page_dirty = false;
            }
            // Clamp scroll to content (8px pads top/bottom)
            {
                  float max_scroll = content.content_h + 16.0f - page.h;
                  if (max_scroll < 0.0f) {
                        max_scroll = 0.0f;
                  }
                  scroll_y = std::clamp(scroll_y, 0.0f, max_scroll);
            }
            Tess::Render::Paint(renderer, content, scroll_y);

            SDL_SetRenderClipRect(renderer, nullptr);

            // Display the rendered frame
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
      }

      Tess::Render::ClearPage(content);
      TTF_DestroyText(txt);
      page_face.Close(); // before TTF_Quit: stack dtor would run after it
      TTF_DestroyRendererTextEngine(eng);
      TTF_CloseFont(font);
      TTF_Quit();
      SDL_StopTextInput(window);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();

      return 0;
}
