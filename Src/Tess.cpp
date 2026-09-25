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
#include "RenderKit/Render.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <filesystem>
#include <print>
#include <string>

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

      int windowWidth = 800;
      int windowHeight = 600;

      SDL_Window *window = SDL_CreateWindow("tesseract", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
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

      TTF_Font *font = TTF_OpenFont("Assets/fonts/CaskaydiaCoveNerdFontMono-Regular.ttf", 16);
      if (!font) {
            font = TTF_OpenFont("../Assets/fonts/CaskaydiaCoveNerdFontMono-Regular.ttf", 16);
      }
      if (!font) {
            std::println(stderr, "font failed: {}", SDL_GetError());
      }
      // Page prose font: sans fallback (no CSS yet, monospace stays in chrome)
      TTF_Font *pageFont = TTF_OpenFont("Assets/fonts/NotoSans-Regular.ttf", 16);
      if (!pageFont) {
            pageFont = TTF_OpenFont("../Assets/fonts/NotoSans-Regular.ttf", 16);
      }
      if (!pageFont) {
            std::println(stderr, "page font failed, falling back to mono: {}", SDL_GetError());
            pageFont = font;
      }
      TTF_TextEngine *eng = TTF_CreateRendererTextEngine(renderer);
      TTF_Text *txt = TTF_CreateText(eng, font, "", 0);
      TTF_SetTextColor(txt, 255, 255, 255, 255);

      /* Main drawing and events and element stuff */
      bool running = true;
      SDL_Event event;
      SDL_StartTextInput(window);

      std::string url = "";
      bool focused = true;

      // Page source: navigated from the URL bar on Enter (sample first)
      Tess::Html::Document doc = Tess::Html::Parse(
            Tess::Html::Tokenize("<h1>tesseract</h1><p>type a file:// url, hit enter</p>"));
      Tess::Render::Page content;
      bool pageDirty = true;

      while (running) {
            SDL_FRect urlBox = {
                  .x = windowWidth * 0.05f, // Starts 10% from left border
                  .y = 6.0f,                // 6 pixels from the top
                  .w = windowWidth * 0.9f,  // Spans 80% of window width
                  .h = 30.0f                // Fixed height of 36 pixels
            };

            while (SDL_PollEvent(&event)) {
                  if (event.type == SDL_EVENT_QUIT) {
                        running = false;
                  }
                  // 1. Update window dimensions when resized
                  else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                        windowWidth = event.window.data1;
                        windowHeight = event.window.data2;
                        pageDirty = true;
                  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                        float mx = event.button.x;
                        float my = event.button.y;
                        focused = (mx >= urlBox.x && mx <= urlBox.x + urlBox.w && my >= urlBox.y && my <= urlBox.y + urlBox.h);
                  } else if (focused && event.type == SDL_EVENT_TEXT_INPUT) {
                        url += event.text.text;
                  } else if (focused && event.type == SDL_EVENT_KEY_DOWN) {
                        if (event.key.key == SDLK_BACKSPACE && !url.empty()) {
                              url.pop_back();
                        } else if (event.key.key == SDLK_RETURN && !url.empty()) {
                              std::string raw = url;
                              // bare path -> absolute file:// URL
                              if (raw.find("://") == std::string::npos) {
                                    std::error_code ec;
                                    raw = "file://" + std::filesystem::absolute(raw, ec).string();
                              }
                              if (auto u = Tess::Net::ParseUrl(raw)) {
                                    if (auto res = Tess::Net::FetchResponse(*u)) {
                                          doc = Tess::Html::Parse(Tess::Html::Tokenize(res->body));
                                          pageDirty = true;
                                    } else {
                                          std::println(stderr, "fetch failed: {}", url);
                                    }
                              } else {
                                    std::println(stderr, "bad url: {}", url);
                              }
                        }
                  }
            }

            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
            SDL_RenderClear(renderer);

            // Draw URL Box Background
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderFillRect(renderer, &urlBox);

            // Draw URL Box Border (blue when focused)
            if (focused) {
                  SDL_SetRenderDrawColor(renderer, 80, 160, 255, 255);
            } else {
                  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            }
            SDL_RenderRect(renderer, &urlBox);

            // URL bar owns white/16pt; Render() mutates both for page text
            TTF_SetFontSize(font, 16);
            TTF_SetTextColor(txt, 255, 255, 255, 255);
            Tess::Draw::DrawText(renderer, txt, url, urlBox.x + 8, urlBox.y + 5);

            if (focused) {
                  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                  Tess::Draw::DrawCaret(renderer, txt, urlBox.x + 8, urlBox.y + 5);
            }

            // Page viewport: 5px under URL bar, 5px off left/right/bottom
            float pageY = urlBox.y + urlBox.h + 5.0f;
            SDL_FRect page = {
                  5.0f,
                  pageY,
                  (float)windowWidth - 10.0f,
                  (float)windowHeight - pageY - 5.0f,
            };
            if (page.w < 0.0f) {
                  page.w = 0.0f;
            }
            if (page.h < 0.0f) {
                  page.h = 0.0f;
            }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &page);

            // Clip all page content to the viewport
            SDL_Rect clip = {(int)page.x, (int)page.y, (int)page.w, (int)page.h};
            SDL_SetRenderClipRect(renderer, &clip);

            // Layout only on content/resize; paint replays cached lines
            if (pageDirty) {
                  Tess::Render::Layout(content, eng, pageFont, doc,
                                       page.x + 8, page.y + 8, page.w - 16);
                  pageDirty = false;
            }
            Tess::Render::Paint(renderer, pageFont, content);

            SDL_SetRenderClipRect(renderer, nullptr);

            // Display the rendered frame
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
      }

      Tess::Render::ClearPage(content);
      TTF_DestroyText(txt);
      TTF_DestroyRendererTextEngine(eng);
      TTF_CloseFont(font);
      if (pageFont != font) {
            TTF_CloseFont(pageFont);
      }
      TTF_Quit();
      SDL_StopTextInput(window);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();

      return 0;
}
