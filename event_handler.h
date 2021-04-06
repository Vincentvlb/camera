#ifndef __EVENT_HANDLER__
#define __EVENT_HANDLER__

#include "app.h"

int8_t event_handler(SDL_Event e, bool *quit, bool *paused, int8_t actual_cam, uint8_t cam_number, int height, int width);

#endif