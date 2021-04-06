#include "disposition_cam.h"

/*
Fonction de disposition des caméras pouvant placer de 1 à 6 caméras.
Elle demande en entrée : la largeur ainsi que la hauteur de la fenêtre et le nombre de caméras voulant être ouvert.
Elle retourne un pointeur de tableau (SDL_Rect).
*/
SDL_Rect * disposition (int width, int height, int cam_number)
{
    SDL_Rect * rects = (SDL_Rect *)malloc(sizeof(SDL_Rect)*6); // réservation d'un espace mémoire pour le tableau de disposition des caméras.
    switch (cam_number) //prise en compte du nombre de caméras souhaité
    {
        case 1 : // mise en plein écran (hauteur x largeur) de la caméra.
            rects[0].x=0;
            rects[0].y=0;
            rects[0].w=width;
            rects[0].h=height;
            break;
        case 2 : //cas pour 2 caméras
            for (int i = 0; i < cam_number; i++) //boucle permettant de disposer les deux caméras côte à côte avec la même dimension. 
            {
                rects[i].x=i*(width/2);
                rects[i].y=0;
                rects[i].w=width/2;
                rects[i].h=height;
            }
            break;
        case 3 : //cas pour 3 caméras : la première caméra est au dessus des deux autres. La disposition est un ensemble du cas 1 et du cas 2.
            rects[0].x=0;
            rects[0].y=0;
            rects[0].w=width;
            rects[0].h=height/2;
            //placement de la caméra du dessus.
            for (int i = 1; i < cam_number; i++) //boucle permettant de placer les deux caméras du dessous.
            {
                rects[i].x=(i-1)*width/2;
                rects[i].y=height/2;
                rects[i].w=width/2;
                rects[i].h=height/2;
            }
            break;
        case 4 : //cas pour 4 caméras
            for (int i = 0; i < cam_number; i++) //une unique boucle plaçant les 4 caméras.
            {
                rects[i].x=(i%2)*width/2;
                rects[i].y=floor(i/2.)*height/2;
                rects[i].w=width/2;
                rects[i].h=height/2;
            }
            break;
        case 5 : //cas pour 5 caméras 
            for (int i = 0; i < 3; i++) //placement de 3 caméras au niveau supérieur
            {
                rects[i].x=i*width/3;
                rects[i].y=0;
                rects[i].w=width/3;
                rects[i].h=height/2;
            }
            for (int i = 3; i < cam_number; i++) //placement de 2 caméras au niveau inférieur
            {
                rects[i].x=(i-3)*width/2;
                rects[i].y=height/2;
                rects[i].w=width/2;
                rects[i].h=height/2;
            }
            break;
        case 6 : //cas pour 6 caméras
            for (int i = 0; i < cam_number; i++) //boucle de placement des 6 caméras 
            {
                rects[i].x=(i%3)*width/3;
                rects[i].y=floor(i/3.)*height/2;
                rects[i].w=width/3;
                rects[i].h=height/2;
            }
            break;
    }
    return rects; // fin de la fonction et renvoie du rectangle de configuration des caméras avec le nombre de caméra souhaitées pour la largeur et la hauteur de la fenêtre fournie
}