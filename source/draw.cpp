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

/* --- @DrawSystem ---------------------------------------------------------- */
static struct {
  DrawConfig config;

  SDL_Window* window;
  SDL_Renderer* backend;
} draw;

static SDL_WindowFlags GetSDLWindowFlags(int flags)
{
  SDL_WindowFlags result = 0;

  if (flags & DRAW_FLAGS_FULLSCREEN) {
    result |= SDL_WINDOW_FULLSCREEN;
  } else {
    result |= SDL_WINDOW_RESIZABLE;
  }

  return result;
}

bool InitDrawSystem(DrawConfig config)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return false;
  }

  draw.config = config;
  draw.config.window_scale = (config.window_scale) ? config.window_scale : 1;
  draw.config.backend_name = config.backend_name;

  SDL_WindowFlags flags = GetSDLWindowFlags(draw.config.flags);
  draw.window = SDL_CreateWindow("Project Holmes",
                                 DRAW_WIDTH * draw.config.window_scale,
                                 DRAW_HEIGHT * draw.config.window_scale,
                                 flags | SDL_WINDOW_HIDDEN);
  if (!draw.window) {
    return false;
  }
  SDL_SetWindowMinimumSize(draw.window, DRAW_WIDTH, DRAW_HEIGHT);

  draw.backend = SDL_CreateRenderer(draw.window, draw.config.backend_name);
  if (!draw.backend) {
    return false;
  }

  SDL_SetDefaultTextureScaleMode(draw.backend, SDL_SCALEMODE_PIXELART);

  if (draw.config.flags & DRAW_FLAGS_VSYNC) {
    /*
     * NOTE:
     *  If the user requests VSync, we first attempt to enable adaptive sync.
     *  In the event that is unavailable, attempt sync on every frame.
     *  Otherwise just give up and run without vsync.
     *    Remi 2026.08.19
     */
    if (!SDL_SetRenderVSync(draw.backend, SDL_RENDERER_VSYNC_ADAPTIVE)) {
      SDL_Log("Adaptive Sync is unavailable, settings VSync to 1");
      if (!SDL_SetRenderVSync(draw.backend, 1)) {
        SDL_Log("VSync unavailable");
      }
    }
  }

  SDL_RendererLogicalPresentation presentation_mode;

  if (draw.config.flags & DRAW_FLAGS_INTEGER_SCALE) {
    presentation_mode = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE;
  } else {
    presentation_mode = SDL_LOGICAL_PRESENTATION_LETTERBOX;
  }

  SDL_SetRenderLogicalPresentation(draw.backend,
                                   DRAW_WIDTH,
                                   DRAW_HEIGHT,
                                   presentation_mode);

  SDL_ShowWindow(draw.window);
  return true;
}

void QuitDrawSystem()
{
  if (draw.backend) {
    SDL_DestroyRenderer(draw.backend);
    draw.backend = NULL;
  }

  if (draw.window) {
    SDL_DestroyWindow(draw.window);
    draw.window = NULL;
  }
}

void TickDrawSystem()
{
  SDL_RenderPresent(draw.backend);

  SDL_SetRenderDrawColorFloat(draw.backend, 0.0f, 0.0f, 0.0f, 1.0f);
  SDL_RenderClear(draw.backend);
}

const char* GetDrawBackendName()
{
  if (draw.backend) {
    return SDL_GetRendererName(draw.backend);
  } else {
    return "no renderer";
  }
}

SDL_Renderer* GetDrawBackend()
{
  return draw.backend;
}

void ToggleFullscreen()
{
  draw.config.flags ^= DRAW_FLAGS_FULLSCREEN;
  bool fullscreen = draw.config.flags & DRAW_FLAGS_FULLSCREEN;
  SDL_SetWindowFullscreen(draw.window, draw.config.flags & fullscreen);
}

/* --- @DrawCommand --------------------------------------------------------- */
void ExecuteDrawCommand(DrawCommand target)
{
  SDL_SetRenderDrawColorFloat(draw.backend,
                              target.header.red,
                              target.header.green,
                              target.header.blue,
                              target.header.alpha);

  switch(target.type) {
    default:
      break;
    case DRAW_COMMAND_NONE:
      break;

    case DRAW_COMMAND_PIXEL: {
      SDL_RenderPoint(draw.backend, target.pixel.x, target.pixel.y);
    } break;

    case DRAW_COMMAND_LINE: {
      SDL_RenderLine(draw.backend,
                     target.line.x1, target.line.y1,
                     target.line.x2, target.line.y2);
    } break;

    case DRAW_COMMAND_RECT: {
      SDL_FRect rect = {
        .x = (float)target.rect.x,
        .y = (float)target.rect.y,
        .w = (float)target.rect.width,
        .h = (float)target.rect.height
      };
      SDL_RenderFillRect(draw.backend, (const SDL_FRect*)&rect);
    } break;
    case DRAW_COMMAND_RECT_OUTLINE: {
      SDL_FRect rect = {
        .x = (float)target.rect.x,
        .y = (float)target.rect.y,
        .w = (float)target.rect.width,
        .h = (float)target.rect.height
      };
      SDL_RenderRect(draw.backend, (const SDL_FRect*)&rect);
    } break;

    case DRAW_COMMAND_SPRITE: {
      SDL_FRect destination_rect = {
        .x = (float)target.sprite.x,
        .y = (float)target.sprite.y,
      };
      
      if (target.sprite.region) {
        destination_rect.w = target.sprite.region->w;
        destination_rect.h = target.sprite.region->h;
      } else {
        SDL_GetTextureSize(target.sprite.texture,
                           &destination_rect.w,
                           &destination_rect.h);
      }

      SDL_RenderTexture(draw.backend,
                        target.sprite.texture,
                        target.sprite.region,
                        (const SDL_FRect*)&destination_rect);
    } break;
  }
}

/* --- @DrawCommandQueue ---------------------------------------------------- */
/*
 * NOTE:
 *   The DrawCommandQueue is an opaque pointer to a data structure.
 *   Right now it is just a simple array but this may change in the future.
 *   Particularly I'm thinking of doing a list of buckets,
 *   where each bucket discriminated by RenderTarget.
 *   This is because it removes the need for a separate Sort function because
 *   that sorting can happen at Push time instead.
 *
 *   The memory is allocated in bulk to contain both the structure and data.
 *   This is done to ensure some amount of memory (and cache) coherency.
 *   However it prevents the array from being truly dynamically sized,
 *   unless the resulting pointer is changed to be a double-indirection.
 *   However, I would rather avoid this as it requires me to maintain two lists
 *   of pointers in this file, obscuring the intended drawing system.
 *
 *     - Remi 2026.08.23
 *
 *   Maybe if I implement a separate (very simple) HashMap?
 * - Remi 2026.09.01
 */
typedef struct QueueData {
  DrawCommandQueueStatus status;

  Uint32 length;
  Uint32 count;
  Uint32 index;
  DrawCommand* array;
} QueueData;

DrawCommandQueue* CreateDrawCommandQueue(Uint32 max_commands)
{
  if (max_commands == 0) {
    SetErrorString("max_commands must be nonzero");
    return NULL;
  }

  const Uint32 ARRAY_SIZE = (max_commands * sizeof(DrawCommand));
  const Uint32 ALLOCATION_SIZE = sizeof(QueueData) + ARRAY_SIZE;

  QueueData* result = (QueueData*)SDL_malloc(ALLOCATION_SIZE);

  if (result) {
    result->status = DRAW_COMMAND_QUEUE_EMPTY;
    result->length = max_commands;
    result->count = 0;
    result->index = 0;
    result->array = (DrawCommand*)((Uint8*)result + sizeof(QueueData));
  }

  return result;
}

void DeleteDrawCommandQueue(DrawCommandQueue* target)
{
  /*
   * Right now we don't have any child allocations of QueueData
   * Therefore we can just deallocate the DrawCommandQueue* (void*).
   *
   * This function is in place in case there are child allocs in the future.
   */
  if (target) {
    SDL_free(target);
  }
}

bool PushDrawCommandToQueue(DrawCommandQueue* target, DrawCommand command)
{
  if (!target) {
    SetErrorString("target was NULL");
    return false;
  }

  QueueData* queue = (QueueData*)target;

  if (queue->status == DRAW_COMMAND_QUEUE_EMPTY) {
    queue->status = DRAW_COMMAND_QUEUE_WRITING;
  } else if (queue->status == DRAW_COMMAND_QUEUE_FINISHED) {
    SetErrorString("attempt to write to finished queue");
    return false;
  }

  if (queue->count + 1 < queue->length) {
    SDL_memcpy(&queue->array[queue->count],
               &command,
               sizeof(DrawCommand));

    queue->count++;
  }
  return true;
}

bool PopDrawCommandFromQueue(DrawCommandQueue* target, DrawCommand* out)
{
  if (!target) {
    SetErrorString("target was NULL");
    return false;
  }
  if (!out) {
    SetErrorString("out was NULL");
    return false;
  }

  QueueData* queue = (QueueData*)target;

  if (queue->status != DRAW_COMMAND_QUEUE_FINISHED) {
    SetErrorString("PopCommandFromQueue: DrawCommandQueue is not finished");
    return false;
  }

  if (queue->count == 0) {
    out->type = DRAW_COMMAND_NONE;
    queue->status = DRAW_COMMAND_QUEUE_EMPTY;
  } else {
    SDL_memcpy(out, &queue->array[queue->index], sizeof(DrawCommand));

    queue->index++;

    if (queue->index >= queue->count) {
      queue->count = 0;
      queue->index = 0;
      queue->status = DRAW_COMMAND_QUEUE_EMPTY;
    }
  }

  return true;
}

void FinishDrawCommandQueue(DrawCommandQueue* target)
{
  QueueData* queue = (QueueData*)target;

  if (queue) {
    queue->status = DRAW_COMMAND_QUEUE_FINISHED;
  }
}

int CompareDrawCommand(DrawCommand a, DrawCommand b)
{
  /* compare the RenderTargets first */
  int result = (int)((Sint64)a.header.target - (Sint64)b.header.target);

  /* if the RenderTargets are equal, compare the depths */
  if (result == 0) {
    result = (int)(a.header.depth - b.header.depth);
  }

  /* normalize the result to -1, 0, or 1 */
  return (result >= 0) - (result <= 0);
}

static int CompareDrawCommandCallback(const void* a, const void* b)
{
  DrawCommand command_a = *(DrawCommand*)a;
  DrawCommand command_b = *(DrawCommand*)b;

  return CompareDrawCommand(command_a, command_b);
}

void SortDrawCommandQueue(DrawCommandQueue* target)
{
  QueueData* queue = (QueueData*)target;

  if (queue) {
    SDL_qsort(queue->array,
              queue->count,
              sizeof(DrawCommand),
              CompareDrawCommandCallback);
  }
}

DrawCommandQueueStatus GetDrawCommandQueueStatus(DrawCommandQueue* target)
{
  QueueData* queue = (QueueData*)target;

  if (queue) {
    return queue->status;
  } else {
    return DRAW_COMMAND_QUEUE_INVALID;
  }
}

/*
 * NOTE:
 *  I'm debating with myself whether or not this should be implemented directly
 *  or using the functions (as written).
 *
 *  An optimizing compiler would probably inline all of this.
 *  However, I don't want to rely too heavily on the optimizer.
 *  Largely because it's a program and therefore can't reason about the code.
 *
 *  I don't know.
 *  It's at least worth further research.
 *
 *    - Remi 2026.08.23
 */
void ExecuteDrawCommandQueue(DrawCommandQueue* target)
{
  DrawCommand command;
  while (GetDrawCommandQueueStatus(target) != DRAW_COMMAND_QUEUE_EMPTY) {
    if (PopDrawCommandFromQueue(target, &command)) {
      ExecuteDrawCommand(command);
    }
  }
}

