#include <holmes.h>

static struct {
  DrawCommandQueue* queue;
} test;

bool InitTestbed()
{
  test.queue = CreateDrawCommandQueue(12);
  if (!test.queue) {
    return false;
  }

  return true;
}

void QuitTestbed()
{
  DeleteDrawCommandQueue(test.queue);
}

bool TickTestbed()
{
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
        .x1 = 64,
        .y1 = 64,
        .x2 = 128,
        .y2 = 96
      }
    },
    {
      .rect = {
        .header.type = DRAW_COMMAND_RECT,
        .header.red = 0.0f,
        .header.green = 0.0f,
        .header.blue = 1.0f,
        .header.alpha = 1.0f,
        .x = 32,
        .y = 128,
        .width = 64,
        .height = 32,
      }
    },
    {
      .rect = {
        .header.type = DRAW_COMMAND_RECT_OUTLINE,
        .header.red = 1.0f,
        .header.green = 0.0f,
        .header.blue = 0.0f,
        .header.alpha = 1.0f,
        .x = 32,
        .y = 128,
        .width = 64,
        .height = 32,
      }
    }
  };

  for (int index = 0; index < SDL_arraysize(commands); ++index) {
    PushDrawCommandToQueue(test.queue, commands[index]);
  }

  FinishDrawCommandQueue(test.queue);

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

