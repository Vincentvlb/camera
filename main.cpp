#include "app.h"
#include "cam_thread.h"
#include "event_handler.h"

/*
 * Fonction principale du programme, lancée lors de l'exécution de notre application.
 * Argument(s) : 
 *   - int argc : variable contenant le nombre d'argument présent dans argv.
 *   - char * argv[] : pointeur d'un tableau de caractère contenant les arguments d'execution du programme.
 * Retour :
 *   - int : retourne un entier représentant si notre application s'est arrêtée normalement. 
 */
int main(int argc, char * argv[])
{

    //Affichage dans le terminal des touches de notre programme.
    std::cout << "Hot keys:" << std::endl;
    std::cout << "\tESC - quit the program" << std::endl;
    std::cout << "\tp - pause video" << std::endl;

    //Variable permettant de mettre en pause le rendu des caméras.
    bool paused = false;
    //Variable permettant de quitter le programme.
    bool quit = false;
    //Variable stockant le résultat de la fonction capturant les évènements.
    int8_t resEvent;

    //Variable contenant les événements de la librairie SDL durant l'exécution de notre programme.
    SDL_Event e;

    //Variable contenant les paramètres important de notre application.
    app_params_t app_params;
    app_params_t *pt_app_params = &app_params;

    //Initialisation de la structure de ces paramètres.
    if(!init_app(pt_app_params)) return -1;
    //Création des "threads" et lancements de ceux-ci en fonction des paramètres de notre application.
    create_threads(pt_app_params);

    //Boucle infini qui s'arrete lorsque la variable quit est vrai.
    while (!quit)
    {
        //Boucle qui s'active lors d'un évenement SDL.
        while (SDL_PollEvent(&e))
        {
            //Demande de traitement de cette évènement via notre gestionnaire d'évènement.
            resEvent = event_handler(e, &quit, &paused, pt_app_params->actual_cam, pt_app_params->number_of_cam, pt_app_params->heigth, pt_app_params->width);
            //Si celui si ne renvoi pas -1 alors nous devons changer la caméra affiché sur l'écran. 
            if(resEvent != -1) pt_app_params->actual_cam = resEvent;
        }

        //Vérification si le rendu des caméra n'est pas mis en pause.
        if (!paused)
        {
            //Rendu des images fournis par les "threads" au bon endroit.
            render(pt_app_params);
        }
    }

    //Libération de tous les espaces mémoires réservés.
    free_all(pt_app_params);

    //Renvoi 0 car l'application c'est finie normalement.
    return 0;
}