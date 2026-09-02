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
  static float direction = 1.0f;
  static float temp = 0.0f;
  temp += 0.1f * direction;
  if (temp > 32.0f || temp < 0) {
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
  DrawCommand commands[] = {
    {
      .pixel = {
        .header.type = DRAW_COMMAND_PIXEL,
        .header.red = 1.0f,
        .header.green = 0.0f,
        .header.blue = 0.0f,
        .header.alpha = 1.0f,
        .header.depth = 16,
        .x = 32,
        .y = 32
      }
    },
    {
      .line = {
        .header.type = DRAW_COMMAND_LINE,
        .header.red = 0.0f,
        .header.green = 1.0f,
        .header.blue = 0.0f,
        .header.alpha = 1.0f,
        .header.depth = 16,
        .x1 = 64,
        .y1 = 64,
        .x2 = 128 + temp,
        .y2 = 96
      }
    },

    {
      .rect = {
        .header.type = DRAW_COMMAND_RECT_OUTLINE,
        .header.red = 1.0f,
        .header.green = 0.0f,
        .header.blue = 0.0f,
        .header.alpha = 1.0f,
        .header.depth = 128,
        .x = 32,
        .y = 128,
        .width = 64,
        .height = 32,
      }
    },
    {
      .rect = {
        .header.type = DRAW_COMMAND_RECT,
        .header.red = 0.0f,
        .header.green = 0.0f,
        .header.blue = 1.0f,
        .header.alpha = 1.0f,
        .header.depth = 32,
        .x = 32,
        .y = 128,
        .width = 64,
        .height = 32,
      }

    },
    {
      .sprite = {
        .header.type = DRAW_COMMAND_SPRITE,
        .header.depth = 0,
        .x = 0,
        .y = 0,
        .texture = test.sprite,
      }
    }
  };

  for (int index = 0; index < SDL_arraysize(commands); ++index) {
    PushDrawCommandToQueue(test.queue, commands[index]);
  }

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

