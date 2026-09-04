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

static struct {
  DrawCommandQueue* queue;
  SDL_Texture* sprite;
} test;

bool InitTestbed()
{
  test.queue = CreateDrawCommandQueue(12);
  if (!test.queue) {
    return false;
  }

  SDL_Surface* sprite_surface = LoadSurfaceFromAssets("test");
  if (!sprite_surface) {
    return false;
  }

  test.sprite = SDL_CreateTextureFromSurface(GetDrawBackend(), sprite_surface);

  return true;
}

void QuitTestbed()
{
  SDL_DestroyTexture(test.sprite);
  DeleteDrawCommandQueue(test.queue);
}

bool TickTestbed()
{
  static Sint32 direction = 1.0f;
  static Sint32 temp = 0.0f;
  temp += 1 * direction;
  if (temp > 32 || temp < 0) {
    direction *= -1;
  }

  /*
   * NOTE:
   *  Drawing graphics this way defeats the point of this setup.
   *  Instead the draw commands should be pushed on the tick thread.
   *  Then popped on the main (render) thread.
   *  By flipping between two queues you can draw and tick simultaneously.
   *    Remi 2026.08.20
   */
  PushDrawCommandToQueue(test.queue, {
    .pixel = {
      .header = {
        .type = DRAW_COMMAND_PIXEL,

        .depth = 16,

        .target = NULL,

        .red = 1.0f,
        .green = 0.0f,
        .blue = 0.0f,
        .alpha = 1.0f,
      },
      .x = 32,
      .y = 32
    }
  });
  PushDrawCommandToQueue(test.queue, {
    .line = {
      .header = {
        .type = DRAW_COMMAND_LINE,

        .depth = 16,

        .target = NULL,

        .red = 0.0f,
        .green = 1.0f,
        .blue = 0.0f,
        .alpha = 1.0f,
      },
      .x1 = 64,
      .y1 = 64,
      .x2 = 128,
      .y2 = 96
    }
  });
  PushDrawCommandToQueue(test.queue, {
    .rect = {
      .header = {
        .type = DRAW_COMMAND_RECT_OUTLINE,

        .depth = 128,

        .target = NULL,

        .red = 1.0f,
        .green = 0.0f,
        .blue = 0.0f,
        .alpha = 1.0f,
      },
      .x = 32,
      .y = 128,
      .width = 64,
      .height = 32,
    }
  });
  PushDrawCommandToQueue(test.queue, {
    .rect = {
      .header = {
        .type = DRAW_COMMAND_RECT,

        .depth = 32,

        .target = NULL,

        .red = 0.0f,
        .green = 0.0f,
        .blue = 1.0f,
        .alpha = 1.0f,
      },
      .x = 32,
      .y = 128,
      .width = 64,
      .height = 32,
    }
  });
  PushDrawCommandToQueue(test.queue, {
    .sprite = {
      .header = {
        .type = DRAW_COMMAND_SPRITE,

        .depth = 0,

        .target = NULL,

        .red = 0.0f,
        .green = 0.0f,
        .blue = 0.0f,
        .alpha = 0.0f,
      },
      .x = 0,
      .y = 0,
      .texture = test.sprite,
    }
  });

  FinishDrawCommandQueue(test.queue);

  SortDrawCommandQueue(test.queue);

  while (GetDrawCommandQueueStatus(test.queue) != DRAW_COMMAND_QUEUE_EMPTY) {
    DrawCommand command;

    if (PopDrawCommandFromQueue(test.queue, &command)) {
      ExecuteDrawCommand(command);
    }
  }

  return true;
}

bool HandleTestbedEvent(SDL_Event* event)
{
  return true;
}

