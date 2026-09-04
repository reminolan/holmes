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

#ifndef DRAW_H
#define DRAW_H

#include <SDL3/SDL.h>

/* --- @DrawSystem ---------------------------------------------------------- */
#define DRAW_WIDTH (640)
#define DRAW_HEIGHT (360)

enum DrawFlags {
  DRAW_FLAGS_FULLSCREEN_SHIFT,
  DRAW_FLAGS_VSYNC_SHIFT,
  DRAW_FLAGS_INTEGER_SCALE_SHIFT,

  DRAW_FLAGS_FULLSCREEN    = 1 << DRAW_FLAGS_FULLSCREEN_SHIFT,
  DRAW_FLAGS_VSYNC         = 1 << DRAW_FLAGS_VSYNC_SHIFT,
  DRAW_FLAGS_INTEGER_SCALE = 1 << DRAW_FLAGS_INTEGER_SCALE_SHIFT,
};

struct DrawConfig {
  int flags;

  Uint16 window_scale;

  char* backend_name;
};

bool InitDrawSystem(DrawConfig config);

void QuitDrawSystem();

void TickDrawSystem();

const char* GetDrawBackendName();

SDL_Renderer* GetDrawBackend();

void ToggleFullscreen();

/* --- @DrawCommand --------------------------------------------------------- */
enum DrawCommandType: Uint8 {
  DRAW_COMMAND_NONE,

  /* Drawing Primitives */
  DRAW_COMMAND_PIXEL,
  DRAW_COMMAND_LINE,
  DRAW_COMMAND_RECT,
  DRAW_COMMAND_RECT_OUTLINE,
  DRAW_COMMAND_SPRITE,
};

struct DrawCommandHeader {
  DrawCommandType type;

  Uint32 depth;

  SDL_Texture* target;

  float red;
  float green;
  float blue;
  float alpha;
};

struct DrawPixelCommand {
  DrawCommandHeader header;

  Uint32 x;
  Uint32 y;
};

struct DrawLineCommand {
  DrawCommandHeader header;

  Uint32 x1;
  Uint32 y1;

  Uint32 x2;
  Uint32 y2;
};

struct DrawRectCommand {
  DrawCommandHeader header;

  Uint32 x;
  Uint32 y;

  Uint32 width;
  Uint32 height;
};

struct DrawSpriteCommand {
  DrawCommandHeader header;

  Uint32 x;
  Uint32 y;

  SDL_Texture* texture;

  const SDL_FRect* region;
};

union DrawCommand {
  DrawCommandType type;
  DrawCommandHeader header;

  DrawPixelCommand pixel;
  DrawLineCommand line;
  DrawRectCommand rect;
  DrawSpriteCommand sprite;
};

void ExecuteDrawCommand(DrawCommand target);

/* --- @DrawCommandQueue ---------------------------------------------------- */
enum DrawCommandQueueStatus: Uint8 {
  DRAW_COMMAND_QUEUE_INVALID,
  DRAW_COMMAND_QUEUE_EMPTY,
  DRAW_COMMAND_QUEUE_WRITING,
  DRAW_COMMAND_QUEUE_FINISHED,
};

typedef void DrawCommandQueue;

DrawCommandQueue* CreateDrawCommandQueue(Uint32 max_commands);

void DeleteDrawCommandQueue(DrawCommandQueue* target);

bool PushDrawCommandToQueue(DrawCommandQueue* target, DrawCommand command);

bool PushPixelCommandToQueue(DrawCommandQueue* target, DrawPixelCommand pixel);

bool PushLineCommandToQueue(DrawCommandQueue* target, DrawLineCommand command);

bool PushRectCommandToQueue(DrawCommandQueue* target,
                            DrawRectCommand rect,
                            bool filled = true);

bool PushSpriteCommandToQueue(DrawCommandQueue* target,
                              DrawSpriteCommand sprite);

bool PopDrawCommandFromQueue(DrawCommandQueue* target, DrawCommand* out);

void FinishDrawCommandQueue(DrawCommandQueue* target);

int CompareDrawCommand(DrawCommand a, DrawCommand b);

void SortDrawCommandQueue(DrawCommandQueue* target);

DrawCommandQueueStatus GetDrawCommandQueueStatus(DrawCommandQueue* target);

#endif//DRAW_H
