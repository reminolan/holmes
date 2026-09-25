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

