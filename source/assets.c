#include <holmes.h>

static struct {
  SDL_Storage* storage;
} assets;

bool InitAssetsSystem(const char* first_command_line_argument)
{
  assets.storage = SDL_OpenTitleStorage(NULL, 0);
  if (!assets.storage) {
    return false;
  }

  return true;
}

void QuitAssetsSystem()
{
  if (assets.storage) {
    SDL_CloseStorage(assets.storage);
  }
}

static SDL_IOStream* IOFromStorage(char* name,
                                   void** out_buffer,
                                   size_t* out_length)
{
  void* buffer = NULL;
  size_t length = 0;

  bool success = SDL_GetStorageFileSize(assets.storage, name, (Uint64*)&length);
  if (!success || length == 0) {
    return NULL;
  }

  buffer = SDL_malloc(length);
  if (!buffer) {
    SDL_SetError("out of memory!!!!");
    return NULL;
  }

  SDL_IOStream* result = SDL_IOFromMem(buffer, (size_t)length);

  if (!result) {
    SDL_free(buffer);
  }

  if (out_buffer) {
    *out_buffer = buffer;
  }

  if (out_length) {
    *out_length = length;
  }

  return result;
}

SDL_Surface* LoadSurfaceFromAssets(const char* name)
{
  SDL_Surface* result = NULL;

  char* filename = NULL;
  SDL_asprintf(&filename, "sprites/%s.bmp", name);

  if (!filename) {
    return NULL;
  }

  void* buffer;
  size_t length;

  SDL_IOStream* source_stream = IOFromStorage(filename, &buffer, &length);

  if (source_stream) {
    result = SDL_LoadBMP_IO(source_stream, true);
    SDL_free(buffer);
  }

  SDL_free(filename);

  return result;
}

