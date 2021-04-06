#include "event_handler.h"

/*
Fonction de traitement d'évènements SDL.
Elle demande en entrée : 
Elle peut modifier les variables quit et paused.
Elle retourne un uint8_t :
* -2 : Le nombre de caméra affiché est maximum.
* -1 : Aucune caméra a été changé.
* 0 : La caméra 1 qui est en évidence.
* 1 : La caméra 2 qui est en évidence.
* 2 : La caméra 3 qui est en évidence.
* 3 : La caméra 4 qui est en évidence.
* 4 : La caméra 5 qui est en évidence.
* 5 : La caméra 6 qui est en évidence.
*/

int8_t event_handler(SDL_Event e, bool *quit, bool *paused, int8_t actual_cam, uint8_t cam_number, int height, int width)
{
    switch (e.type)
    {
        case SDL_QUIT: //l'utilisateur a souhaité fermer l'application par la bouton de fermeture de la fenêtre.
            *quit = true;
            return -1;

        case SDL_KEYDOWN: //
            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE: //l'utilisateur a souhaité fermer l'application par la touche échap.
                *quit = true;
                return -1;

            case SDLK_p: //l'utilisateur a souhaité mettre en pause les caméras par la touche p.
                *paused = !(*paused);
                return -1;
            }
        case SDL_MOUSEBUTTONUP:
            //std::cout << "Le clic souris est appuyé " << "au coordonnées x: " << e.button.x << " et y : " << e.button.y << std::endl;
            switch (cam_number) //prise en compte du nombre de caméra au total.
            {
                case 1: //cas de 1 caméra.
                    return -1;
                case 2: //cas de 2 caméras.
                    if (actual_cam != -2) //vérification si toutes les caméras sont actives.
                        return -2;
                    else
                    {
                        if (e.button.x < width / 2) //caméra de gauche détectée.
                            return 0;
                        else // sinon caméra de droite détectée.
                            return 1; 
                    }
                case 3: //cas de 3 caméras.
                    if (actual_cam != -2) //vérification si toutes les caméras sont actives.
                        return -2;
                    else
                    {
                        if (e.button.y < height / 2) //détection de la caméra du haut (caméra 1).
                            return 0;

                        else
                        {
                            if (e.button.x < width / 2) // détection de la caméra en bas à gauche (2ème caméra).
                                return 1;
                            else // détection de la caméra en bas à droite (3ème caméra).
                                return 2;
                        }
                    }
                case 4: //cas de 4 caméras.
                    if (actual_cam != -2) //vérification si toutes les caméras sont actives.
                        return -2;
                    else
                    {
                        if (e.button.x < width / 2) // la caméra est à gauche de la fenêtre.
                        {
                            if (e.button.y < height / 2) // la caméra est en haut à gauche (caméra 1).
                                return 0;
                            else // caméra en bas à gauche (caméra 3).
                                return 2;
                        }
                        else //la caméra est à droite.
                        {
                            if (e.button.y < height / 2) // la caméra est en haut à droite (caméra 2).
                                return 1;
                            else // la caméra est en bas à droite (caméra 4).
                                return 3;
                        }
                    }
                case 5: //cas de 5 caméras.
                    if (actual_cam != -2) //vérification si toutes les caméras sont actives.
                        return -2;
                    else
                    {
                        if (e.button.y > height / 2) // la caméra est en bas.
                        {
                            if (e.button.x < width / 2) //la caméra est en bas à gauche (caméra 4).
                                return 3;
                            else //la caméra est en bas à droite (caméra 5).
                                return 4;
                        }
                        else //la caméra est en haut.
                        {
                            if (e.button.x < width / 3) // la caméra est en haut à gauche (caméra 1).
                                return 0;
                            else if (e.button.x < 2 * width / 3) // la caméra est en haut au centre (caméra 2).
                                return 1;
                            else // la caméra est en haut à droite (caméra 3).
                                return 2;
                        }
                    }
                case 6: //cas de 6 caméras.
                    if (actual_cam != -2) //vérification si toutes les caméras sont actives.
                        return -2;
                    else 
                    {
                        if (e.button.y < height / 2) // la caméra est en haut.
                        {
                            if (e.button.x < width / 3) //la caméra est en haut à gauche (caméra 1).
                                return 0;
                            else if (e.button.x < 2 * width / 3) //la caméra est en haut au centre (caméra 2).
                                return 1;
                            else //la caméra est en haut à droite (caméra 3).
                                return 2;
                        }
                        else
                        {
                            if (e.button.x < width / 3) //la caméra est en bas à gauche (caméra 4).
                                return 3;
                            else if (e.button.x < 2 * width / 3) //la caméra est en  bas au centre (caméra 5).
                                return 4;
                            else //la caméra est en bas à droite (caméra 6).
                                return 5;
                        }
                    }
            }
        default: //valeur de défaut si la fonction rencontre un évènement non traité. (Elle ne change rien au programme principal dans ce cas là).
            return -1;
    }
}