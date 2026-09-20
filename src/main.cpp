/*
** Tesseract - A simple 2D browserlike rendering engine.
** https://github.com/tanvir-techbro/tesseract
**
** Copyright (c) 2026-present tanvir-techbro (Tanvir)
** Released under the MIT License.
*/

/* main.c file - Main */

#include <SDL3/SDL.h>
#include <print>

int main(int argc, char *argv[]) {

      /* Window Initialization */
      if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::println(stderr, "SDL Init failed: {}", SDL_GetError());
            return 1;
      }

      SDL_Window *window = SDL_CreateWindow("Resizable Window", 800, 600, SDL_WINDOW_RESIZABLE);
      SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

      if (!window) {
            std::println(stderr, "Window creation failed: {}", SDL_GetError());
            SDL_Quit();
            return 1;
      }

      bool running = true;
      SDL_Event event;

      while (running) {
            while (SDL_PollEvent(&event)) {
                  if (event.type == SDL_EVENT_QUIT) {
                        running = false;
                  }
            }
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
      }

      SDL_DestroyWindow(window);
      SDL_Quit();

      return 0;
}
