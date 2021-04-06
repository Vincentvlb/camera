#ifndef __APP__
#define __APP__

#include <array>
#include <cmath> 
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

//Utilisation de ces deux inclusions pour la librairie gérant la fenêtre.
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
//Utilisation de ces quatres inclusions pour la librairie gérant les flux caméras.
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "json.h"

//Structure contenant tous les paramètres d'un thread.
typedef struct thread_params_s {
    int id; //numéro d'identifiaction du "thread".
    SDL_Rect *dst; //rectangle de destination de la caméra quand nous somme en mosaïque.
    SDL_Texture *texture; //texture du "thread".
    SDL_Rect *txt_dst; //rectangle de la destination du label de la caméra en mosaïque.
    SDL_Texture *txt_texture; //texture du label de la caméra.
    SDL_Rect *no_camera_txt_dst; //rectangle de la destination du texte d'erreur de connection du flux du "thread" en mosaïque.
    SDL_Texture *no_camera_txt_texture; //texture du texte d'erreur de connection du flux du "thread" en mosaïque.
    SDL_Renderer *renderer; //variable de rendu de l'aplication.
    SDL_mutex *mutex; //"mutex" permettant de vérouiller l'utilisation de la texture de la caméra du "thread".
    const std::string *cam_url; //url du flux ou fichier de la caméra.
    bool alive; //variable pour arreter ou non la vie du "thread".
    bool stream_start; //variable pour attester que le flux est fonctionnel et lancé.
    SDL_Rect *frame_size; //taille de l'image du flux du "thread".
    cv::Mat frame; //image du flux du "thread".
    cv::VideoCapture *cap; // flux du "thread".
} thread_params_t; //Création d'un type avec cette structure.

//Structure contenant tous les paramètres du programme.
typedef struct app_params_s {
    uint8_t number_of_cam; //Nombre de caméra dans l'application.
    int8_t actual_cam; //Caméra afficher actuelement (0-5 et -2 pour mosaïque).
    bool is_in_full; //booléen présisant si l'animation est fini.
    uint number_animation_frame; //entier repésentant le nombre d'image composant une animation.
    uint width; //largeur de la fenêre de l'application.
    uint heigth; //hauteur de la fenêre de l'application.
    thread_params_t *thread_params; //pointeur dirigeant vers un tableau contenant les paramètres des "threads".
    SDL_Thread **threads; //pointeur dirigeant vers un tableau contenant les "threads".
    SDL_Window *window; //pointeur sur la fenêtre.
    SDL_Renderer *renderer; //pointeur verds la variable gérant le rendu.
    SDL_Rect rectFull; //rectangle permettant le déplacement de l'image de gérer l'animation de mosaïque vers plein écran.
    SDL_Rect txt_rectFull; //rectangle permettant le déplacement du label l'image de gérer l'animation de mosaïque vers plein écran.
    TTF_Font * font; //police utilisé pour tous les textes de l'application.
    uint index_render_frame; //entier repésentant l'index de l'image de l'animation en cours.
} app_params_t; //Création d'un type avec cette structure.

bool init_app(app_params_t *pt_app_params);
void free_all(app_params_t *pt_app_params);
void render(app_params_t *pt_app_params);
void render_frame(app_params_t *pt_app_params, int cam_idx);
void renderNoCameraTxt(app_params_t *pt_app_params, int cam_idx, int X, int Y, int W, int H);
SDL_Texture * noCameraTxt(app_params_t *pt_app_params, SDL_Rect *txt_dst, int cam_idx);
void fillTexture(SDL_Texture * texture, cv::Mat mat);
SDL_Texture *create_texture(SDL_Rect *rect, SDL_Renderer *renderer);

#endif