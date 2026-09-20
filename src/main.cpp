/*
** Tesseract - A simple 2D browserlike rendering engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* main.c file - Main */

#include "../lib/draw/draw.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
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

      TTF_Font *font = TTF_OpenFont("assets/fonts/CaskaydiaCoveNerdFontMono-Regular.ttf", 16);
      if (!font) {
            font = TTF_OpenFont("../assets/fonts/CaskaydiaCoveNerdFontMono-Regular.ttf", 16);
      }
      if (!font) {
            std::println(stderr, "font failed: {}", SDL_GetError());
      }
      TTF_TextEngine *eng = TTF_CreateRendererTextEngine(renderer);
      TTF_Text *txt = TTF_CreateText(eng, font, "", 0);
      TTF_SetTextColor(txt, 255, 255, 255, 255);

      /* Main drawing and events and element stuff */
      bool running = true;
      SDL_Event event;
      SDL_StartTextInput(window);

      std::string url = "";
      const std::string prefix = "http://";
      bool focused = true;

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
                  } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                        float mx = event.button.x;
                        float my = event.button.y;
                        focused = (mx >= urlBox.x && mx <= urlBox.x + urlBox.w &&
                                   my >= urlBox.y && my <= urlBox.y + urlBox.h);
                  } else if (focused && event.type == SDL_EVENT_TEXT_INPUT) {
                        url += event.text.text;
                  } else if (focused && event.type == SDL_EVENT_KEY_DOWN) {
                        if (event.key.key == SDLK_BACKSPACE && !url.empty()) {
                              url.pop_back();
                        }
                  }
            }

            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
            SDL_RenderClear(renderer);

            // Draw URL Box Background
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            tesseract::draw::FillRoundedRect(renderer, urlBox, 8.0f);

            // Draw URL Box Border
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            tesseract::draw::DrawRoundedOutline(renderer, urlBox, 8.0f);

            std::string full = prefix + url;
            TTF_SetTextString(txt, full.c_str(), 0);
            TTF_DrawRendererText(txt, urlBox.x + 8, urlBox.y + 5);

            if (focused && (SDL_GetTicks() / 530) % 2 == 0) {
                  int tw = 0, th = 0;
                  TTF_GetTextSize(txt, &tw, &th);
                  SDL_FRect caret = {urlBox.x + 8 + tw, urlBox.y + 5, 2, (float)th};
                  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                  SDL_RenderFillRect(renderer, &caret);
            }

            // Display the rendered frame
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
      }

      SDL_StopTextInput(window);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();

      return 0;
}
