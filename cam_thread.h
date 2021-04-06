#ifndef __CAM_THREAD__
#define __CAM_THREAD__

#include "app.h"
#include "disposition_cam.h"

void create_threads(app_params_t *pt_ctx);
SDL_Texture* createTextureLabelCam(app_params_t *pt_app_params, int cam_idx);
int update_cam_texture(void *pt_param);
SDL_Texture *create_texture(SDL_Rect *rect, SDL_Renderer *renderer);

#endif