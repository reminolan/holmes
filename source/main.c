#include <holmes.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "testbed.h"

SDL_AppResult SDL_AppInit(void** app_state,
                          int arguments_count,
                          char** arguments_array)
{
  (void)app_state;
  (void)arguments_count;
  (void)arguments_array;

  /*
   * TODO:
   *  This should not be hardcoded but instead come from a settings file.
   *    Remi 2026.08.20
   */
  DrawConfig draw_config = {
    .flags = DRAW_FLAGS_VSYNC,
    .window_scale = 2,
  };
  if (!InitDrawSystem(draw_config)) {
    SDL_Log("Draw init failed! Error: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!InitTestbed()) {
    SDL_Log("Failed to init testbed! Error: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* app_state, SDL_Event* event)
{
  (void)app_state;

  switch (event->type) {
    case SDL_EVENT_QUIT:
      return SDL_APP_SUCCESS;

    case SDL_EVENT_KEY_UP: {
      bool f11 = event->key.key == SDLK_F11;
      bool alt = event->key.mod & SDL_KMOD_ALT;
      bool enter = event->key.key == SDLK_RETURN;

      if (f11 || (alt && enter)) {
        ToggleFullscreen();
      }
    } break;

    default:
      break;
  }

  HandleTestbedEvent(event);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* app_state)
{
  (void)app_state;

  TickTestbed();
  TickDrawSystem();

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* app_state, SDL_AppResult app_result)
{
  (void)app_state;
  (void)app_result;

  QuitTestbed();
  QuitDrawSystem();
}

