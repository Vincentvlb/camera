#include "full_animation.h"

/*
Fonction de mise en plein écran de la caméra souhaitée.
Elle demande en entrée la structure de des paramètres du programme ainsi que la caméra à agrandir.
Elle retourne un pointeur de tableau (SDL_Rect).
*/
void animation(app_params_t *pt_app_params, int index_frame)
{

    int x = pt_app_params->thread_params[pt_app_params->actual_cam].dst->x; //emplacement de la caméra quand plusieurs caméra sont actives.
    int y = pt_app_params->thread_params[pt_app_params->actual_cam].dst->y; //emplacement de la caméra quand plusieurs caméra sont actives.
    int w = pt_app_params->thread_params[pt_app_params->actual_cam].dst->w; //emplacement de la caméra quand plusieurs caméra sont actives.
    int h = pt_app_params->thread_params[pt_app_params->actual_cam].dst->h; //emplacement de la caméra quand plusieurs caméra sont actives.
    int txt_w = pt_app_params->thread_params[pt_app_params->actual_cam].txt_dst->w; // emplacement du texte lorsque plusieurs caméras sont actives.
    int txt_h = pt_app_params->thread_params[pt_app_params->actual_cam].txt_dst->h; // emplacement du texte lorsque plusieurs caméras sont actives.
    int W = pt_app_params->width; //largeur de la fenêtre de l'application.
    int H = pt_app_params->heigth; //longueur de la fenêtre de l'application.
    int number_animation_frame = pt_app_params->number_animation_frame; //initialisation du nombre d'animation.

    if(w!=W){
        pt_app_params->rectFull.w = w+((W-w)*index_frame/number_animation_frame); //position de la caméra en cours d'animation (largeur).
    }

    if(h!=H){
        pt_app_params->rectFull.h = h+((H-h)*index_frame/number_animation_frame); //position de la caméra en cours d'animation (hauteur).
    }

    if(x!=0){
        pt_app_params->rectFull.x = x-(x*index_frame/number_animation_frame); //position de la caméra en cours d'animation (emplacement horizontal : x).
    }

    if(y!=0){
        pt_app_params->rectFull.y = y-(y*index_frame/number_animation_frame); //position de la caméra en cours d'animation (emplacement vertical : y).
    }

    pt_app_params->txt_rectFull.x = pt_app_params->rectFull.x + (pt_app_params->rectFull.w - (txt_w+5)); //position du texte lors de l'animation.
    pt_app_params->txt_rectFull.y = pt_app_params->rectFull.y + (pt_app_params->rectFull.h - (txt_h+5)); //position du texte lors de l'animation.

}