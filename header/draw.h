#ifndef DRAW_H
#define DRAW_H

#include <SDL3/SDL.h>

/* --- @DrawSystem ---------------------------------------------------------- */
#define DRAW_WIDTH (640)
#define DRAW_HEIGHT (360)

typedef enum DrawFlags: Uint8 {
  DRAW_FLAGS_FULLSCREEN_SHIFT,
  DRAW_FLAGS_VSYNC_SHIFT,
  DRAW_FLAGS_INTEGER_SCALE_SHIFT,

  DRAW_FLAGS_FULLSCREEN    = 1 << DRAW_FLAGS_FULLSCREEN_SHIFT,
  DRAW_FLAGS_VSYNC         = 1 << DRAW_FLAGS_VSYNC_SHIFT,
  DRAW_FLAGS_INTEGER_SCALE = 1 << DRAW_FLAGS_INTEGER_SCALE_SHIFT,
} DrawFlags;

typedef struct DrawConfig {
  DrawFlags flags;

  Uint16 window_scale;

  char* backend_name;
} DrawConfig;

bool InitDrawSystem(DrawConfig config);

void QuitDrawSystem();

void TickDrawSystem();

const char* GetDrawBackendName();

SDL_Renderer* GetDrawBackend();

void ToggleFullscreen();

/* --- @DrawCommand --------------------------------------------------------- */
typedef enum DrawCommandType: Uint32 {
  DRAW_COMMAND_NONE,

  /* Drawing Primitives */
  DRAW_COMMAND_PIXEL,
  DRAW_COMMAND_LINE,
  DRAW_COMMAND_RECT,
  DRAW_COMMAND_RECT_OUTLINE,

  /* Drawing Sprites */
  /*
   * TODO: implement these once sprites are available
   * DRAW_COMMAND_SPRITE,
   * DRAW_COMMAND_SPRITE_REGION,
   */
} DrawCommandType;

typedef struct DrawCommandHeader {
  DrawCommandType type;

  Uint32 depth;

  SDL_Texture* target;

  float red;
  float green;
  float blue;
  float alpha;
} DrawCommandHeader;

typedef struct DrawPixelCommand {
  DrawCommandHeader header;

  Uint32 x;
  Uint32 y;
} DrawPixelCommand;

typedef struct DrawLineCommand {
  DrawCommandHeader header;

  Uint32 x1;
  Uint32 y1;

  Uint32 x2;
  Uint32 y2;
} DrawLineCommand;

typedef struct DrawRectCommand {
  DrawCommandHeader header;

  Uint32 x;
  Uint32 y;

  Uint32 width;
  Uint32 height;
} DrawRectCommand;

typedef struct DrawTargetCommand {
  DrawCommandHeader header;

  //TODO: We shouldn't pass the texture directly.
  //      Instead it should be an abstracted RenderTarget.
  //        - Remi 2026-08-19
  SDL_Texture* target;
} DrawTargetCommand;

typedef union DrawCommand {
  DrawCommandType type;
  DrawCommandHeader header;

  DrawPixelCommand pixel;
  DrawLineCommand line;
  DrawRectCommand rect;
} DrawCommand;

void ExecuteDrawCommand(DrawCommand target);

/* --- @DrawCommandQueue ---------------------------------------------------- */
typedef enum DrawCommandQueueStatus: Uint8 {
  DRAW_COMMAND_QUEUE_INVALID,
  DRAW_COMMAND_QUEUE_EMPTY,
  DRAW_COMMAND_QUEUE_WRITING,
  DRAW_COMMAND_QUEUE_FINISHED,
} DrawCommandQueueStatus;

typedef void DrawCommandQueue;

DrawCommandQueue* CreateDrawCommandQueue(Uint32 max_commands);

void DeleteDrawCommandQueue(DrawCommandQueue* target);

bool PushDrawCommandToQueue(DrawCommandQueue* target, DrawCommand command);

bool PopDrawCommandFromQueue(DrawCommandQueue* target, DrawCommand* out);

void FinishDrawCommandQueue(DrawCommandQueue* target);

DrawCommandQueueStatus GetDrawCommandQueueStatus(DrawCommandQueue* target);

#endif//DRAW_H
