#ifndef ASSETS_H
#define ASSETS_H

#include <SDL3/SDL.h>

bool InitAssetsSystem();

void QuitAssetsSystem();

SDL_Surface* LoadSurface(const char* name);

#endif//ASSETS_H
