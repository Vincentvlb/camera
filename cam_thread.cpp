#include "cam_thread.h"

//Récupération de la variable global contenant les "urls" des caméras
extern std::string s_cam_url[6];

/*
 * Fonction qui créer les "threads" du programme.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètres du programme.
 * Retour :
 *   - void : ne retourne rien. 
 */
void create_threads(app_params_t *pt_app_params)
{
    //Variable contenant le nombre de caméra de notre programme.
    uint number_of_cam = pt_app_params->number_of_cam;
    //Créations d'un pointeur donnant l'adresse du tableau des paramètres des "threads".
    thread_params_t *thread_params = pt_app_params->thread_params;

    //Récupération d'un pointeur donnant l'adresse du tableau de SDL_Rect qui représente la localisation des caméras dans la fenêtre.
    SDL_Rect *dsts = disposition(pt_app_params->width,pt_app_params->heigth,pt_app_params->number_of_cam);

    //Message dans le terminal prévenant la création des "threads" de l'application.
    std::cout << "Creation of thread..." << std::endl;
    //Boucles sur les futurs "threads" de notre applications.
    for (uint i = 0; i < number_of_cam; i++)
    {
        //Définitions de l'id du "thread" en fonction de l'index de la boucle.
        thread_params[i].id = i;
        
        //Préparation de la variable qui accueille la texture du "thread".
        thread_params[i].texture = NULL;

        //Stockage de l'emplacement de la caméra du "thread".
        thread_params[i].dst = &dsts[i];

        //Création de la texture du label de la caméra du "thread".
        thread_params[i].txt_texture = createTextureLabelCam(pt_app_params,i);
        //Calcule du placement de cette texture.
        int w = 0;
        int h = 0;
        SDL_QueryTexture(thread_params->txt_texture, NULL, NULL, &w, &h);
        SDL_Rect *txt_dst = (SDL_Rect *)malloc(sizeof(SDL_Rect));
        //Stockage du calcule dans le rectangle txt_dst qui vient d'être alloué.
        txt_dst->x = dsts[i].x  + (dsts[i].w - (w+5));
        txt_dst->y = dsts[i].y  + (dsts[i].h - (h+5));
        txt_dst->w = w;
        txt_dst->h = h;
        pt_app_params->txt_rectFull.w = w;
        pt_app_params->txt_rectFull.h = h;
        thread_params[i].txt_dst = txt_dst;

        //Allocation et création de la texture du texte si le flux n'existe pas ou est hors ligne.
        thread_params[i].no_camera_txt_dst = (SDL_Rect *)malloc(sizeof(SDL_Rect));
        thread_params[i].no_camera_txt_texture = noCameraTxt(pt_app_params, thread_params[i].no_camera_txt_dst, i);

        //Récupération de la variable de rendu.
        thread_params[i].renderer = pt_app_params->renderer;
        //Création du "mutex".
        thread_params[i].mutex = SDL_CreateMutex();
        //Récupération de la bonne "url" de caméra en fonction de l'id du "thread".
        thread_params[i].cam_url = new std::string(s_cam_url[i]);
        thread_params[i].stream_start = false; 
        thread_params[i].alive = true;
        //Mise en route de la fonction du thread.
        pt_app_params->threads[i] = SDL_CreateThread(update_cam_texture, "cam_texture", (void *)&thread_params[i]);   
    }

}

/*
 * Fonction qui créer les "threads" du programme.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètres du programme.
 * Retour :
 *   - SDL_Texture * : renvoi la texture du label de la caméra.
 */
SDL_Texture* createTextureLabelCam(app_params_t *pt_app_params, int cam_idx)
{
    //Variable pour stocker le label de la caméra.
    char camLabel[20];
    SDL_Texture * txt_texture;
    SDL_Surface * txt_surface; 
    //Création du la couleur blanche pour ce label.
    SDL_Color couleurBlanc = {255,255,255};

    //Initialisation du label de la caméra dans la variable.
    snprintf(camLabel, sizeof(camLabel), "Caméra n°%d", cam_idx+1);
    
    //Création de la surface correspondante avec la librairie ttf.
    txt_surface = TTF_RenderUTF8_Solid(pt_app_params->font, camLabel, couleurBlanc);

    //Création de la texture qui contiendera le surface.
    txt_texture = SDL_CreateTextureFromSurface(pt_app_params->renderer, txt_surface);

    //Libération de l'espace mémoire pris par la surface qui à été imprimé dans la texture.
    SDL_FreeSurface(txt_surface);

    //Renvoi de la texture contenant le message.
    return txt_texture;
}

/*
 * Fonction initialisant les paramètre du programme.
 * Argument(s) : 
 *   - void *pt_param : variable contenant les paramètres du "thread".
 * Retour :
 *   - int : retour du "thread" quand celui-ci sera arrêté.
 */
int update_cam_texture(void *pt_param)
{
    //Création d'un pointeur pour que le programme sache le type du paramètre.
    thread_params_t *thread = (thread_params_t *)pt_param;
    //Variable contenant l'object de capture de la librairie opencv.
    cv::VideoCapture vcap;

    //Message dans le terminal prévenant le lancement du "thread".
    std::cout << "[Thread n°" << thread->id << "] : " << "I'm start." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : '" << *thread->cam_url << "' url." << std::endl;
    thread->cap = &vcap;

    //Création d'une variable pour récupère la taille de l'image du flux.
    thread->frame_size = (SDL_Rect *)malloc(sizeof(SDL_Rect));

    //Vérification si la librairie opencv n'a pas réussi à ouvrir le flux vidéo.
    if (!vcap.open(*thread->cam_url))
    {
        //Message dans le terminal prévenant que la librairie n'as pas réussi à ouvrir le stream ou le fichier..
        std::cout << "Error opening video stream or file" << std::endl;
        thread->alive = false;
        return -1;
    }

    //Récupération de la taille de l'image du flux et stockage.
    thread->frame_size->x = 0;
    thread->frame_size->y = 0;
    thread->frame_size->w = (int)vcap.get(cv::CAP_PROP_FRAME_WIDTH);
    thread->frame_size->h = (int)vcap.get(cv::CAP_PROP_FRAME_HEIGHT);

    //Message dans le terminal montrant la bonne initialisation de celui-ci.
    std::cout << "[Thread n°" << thread->id << "] : " << thread->frame_size->x << " frame size x." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : " << thread->frame_size->y << " frame size y." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : " << thread->frame_size->w << " frame size width." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : " << thread->frame_size->h << " frame size height." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : " << thread->dst->x << " dst size x." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : " << thread->dst->y << " dst size y." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : " << thread->dst->w << " dst size width." << std::endl;
    std::cout << "[Thread n°" << thread->id << "] : " << thread->dst->h << " dst size height." << std::endl;

    std::cout << "[Thread n°" << thread->id << "] : " << *thread->cam_url << " is start." << std::endl;
    double fps = vcap.get(cv::CAP_PROP_FPS);
    std::cout << "[Thread n°" << thread->id << "] : " << "Fps of camera source : " << fps  << "." << std::endl;

    //Verrouillage du "mutex" pour changer l'image du flux.
    SDL_LockMutex(thread->mutex);
    //Mise à jour de l'image du flux.
    vcap >> thread->frame;
    //Déverrouillage du "mutex".
    SDL_UnlockMutex(thread->mutex);
    //Changement de la variable pour indiquer au programme principale que le flux à commencé.
    thread->stream_start = true;

    while (thread->alive)
    {
        //Verrouillage du "mutex" pour changer l'image du flux.
        SDL_LockMutex(thread->mutex);
        //Mise à jour de l'image du flux.
        vcap >> thread->frame;
        //Déverrouillage du "mutex".
        SDL_UnlockMutex(thread->mutex);

        //Vérification si l'image du flux est vide.
        if (thread->frame.empty())
        {
            continue;
        }
        //Pause de 10ms.
        SDL_Delay(10);
    }

    //Libération de la mémoire qu'utilise l'image du flux.
    thread->frame.release();
    //Libération de la mémoire qu'utilise le flux.
    vcap.release();

    //Renvoi que tous c'est bien passé au programme principale.
    return 0;
}