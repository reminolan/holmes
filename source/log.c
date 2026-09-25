/*
 * Copyright (c) 2026 Duphus, LLC
 *
 * This software is provided ‘as-is’, without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */

#include <holmes.h>

static inline const char* ConvertBoolToString(bool value)
{
  return value ? "true" : "false";
}

void InitLog()
{
  SDL_Log("holmes");

#ifdef BUILD_INTERNAL
  SDL_Log("BUILD_INTERNAL");
#endif

  SDL_Log("BUILD_NAME: %s", BUILD_NAME);
  SDL_Log("BUILD_TYPE: %s", BUILD_TYPE);
  SDL_Log("BUILD_VERSION: %s", BUILD_VERSION);

  SDL_Log("CPU Cores: %d", SDL_GetNumLogicalCPUCores());
  SDL_Log("RAM: %d GiB", SDL_GetSystemRAM() / 1024);

  SDL_Log("draw backend: %s", GetDrawBackendName());

  SDL_Log("SDL_VERSION: %d", SDL_VERSION);
  SDL_Log("SDL_GetVersion: %d", SDL_GetVersion());
  SDL_Log("SDL_GetPlatform: %s", SDL_GetPlatform());
  SDL_Log("SDL_HasSSE2: %s", ConvertBoolToString(SDL_HasSSE2()));
  SDL_Log("SDL_HasSSE42: %s", ConvertBoolToString(SDL_HasSSE42()));
  SDL_Log("SDL_HasNEON: %s", ConvertBoolToString(SDL_HasNEON()));
}

