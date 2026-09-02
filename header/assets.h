#ifndef ASSETS_H
#define ASSETS_H

#include <SDL3/SDL.h>

bool InitAssetsSystem(const char* first_command_line_argument);

void QuitAssetsSystem();

SDL_Surface* LoadSurfaceFromAssets(const char* name);

#endif//ASSETS_H
