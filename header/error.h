#ifndef ERROR_H
#define ERROR_H

#define SetErrorString(message, ...) \
  SDL_SetError("%s (%s:%d):" message, \
               __func__, __FILE__, __LINE__ __VA_OPT__(,) ##__VA_ARGS__)

#endif//ERROR_H
