#include "app.h"
#include "full_animation.h"

//Variable global contenant les "urls" des caméras.
std::string s_cam_url[6];

/*
 * Fonction initialisant les paramètre du programme.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètres du programme.
 * Retour :
 *   - bool : retourne si l’initialisation c'est bien passée. 
 */
bool init_app(app_params_t *pt_app_params)
{
    //Message dans le terminal prévenant l'initialisation de l'application.
    std::cout << "Creation of app..." << std::endl;

    //Variable contenant le nombre de caméra de notre programme.
    uint number_of_cam;
    //Variable contenant la taille de la largeur de la fenêtre de notre programme.
    uint width;
    //Variable contenant la taille de la hauteur de la fenêtre de notre programme.
    uint heigth;

    //Vérification que le fichier de configuration est bien présent.
    if( access( "config.json", F_OK ) == 0 ) {
        //Création de la variable qui contient la configuration lu dans le fichier.
        JSONValue json;
        //Lecture du fichier json et décodage du json pour des types c++.
        json = JSON::load("config.json");
        //Récupération du nombre de caméra spécifier dans le fichier de configuration.
        number_of_cam = json["number_of_cam"].asInt();
        //Récupération de la taille de la largeur de la fenêtre de l'application spécifier dans le fichier de configuration.
        width = json["width"].asInt();
        //Récupération de la taille de la hauteur de la fenêtre de l'application spécifier dans le fichier de configuration.
        heigth = json["heigth"].asInt();
        //Object contenant les "urls" des caméras spécifier dans le fichier de configuration.
        JSONObject cam_url_map = json["cam_url"].asObject();
        //Boucle permettant de stocker ces "urls" dans un tableau de chaîne de caractère.
        for (JSONObject::iterator it = cam_url_map.begin(); it != cam_url_map.end(); ++it){
            s_cam_url[std::stoi(it->first)-1] = it->second.asString();
        }
        //Message dans le terminal prévenant la bonne lecture du fichier de configuration.
        std::cout << "File read ok." << std::endl;      
    } else {
        //Message dans le terminal prévenant que le fichier de configuration n'a pas été trouvé. Un par défaut sera généré.
        std::cout << "\"config.txt\" file are not found, default config created !" << std::endl;     
        
        //Par défaut le nombre de caméra sera a 2.
        number_of_cam = 2;
        //Par défaut la taille de la largeur de la fenêtre sera à 1080 pixels.
        width = 1080;
        //Par défaut la taille de la hauteur de la fenêtre sera à 720 pixels.
        heigth = 720;
        //Création des "urls" des deux caméras par défaut, ils seront nul.
        for(uint8_t i=0 ; i<2;i++){
            s_cam_url[i] = "";
        } 

        //Création d'un objet json contenant la configuration par défaut.
        JSONValue json;
        
        //Initialisation de cette object en fonction des valeurs choisi auparavant.
        json["number_of_cam"] = JSONValue((int)number_of_cam);
        json["width"] = JSONValue((int)width);
        json["heigth"] = JSONValue((int)heigth);
        JSONObject url;
        for(uint8_t i=1 ; i<=2;i++){
            url[std::to_string(i)] = "";
        }
        json["cam_url"] = url;
        
        //Sauvegarde de cette objet json dans le fichier config.json qui sera lu lors du prochain démarrage de l'application.
        json.save("config.json");
    }
    
    //Tous les paramètres récolté dans le fichier ou mis par défaut sont mis dans la structure contenant les paramètre de l'application.
    pt_app_params->number_of_cam = number_of_cam;
    pt_app_params->width = width;
    pt_app_params->heigth = heigth;
    std::cout << number_of_cam << " camera configured for " << width << "x" << heigth << " windows size !" << std::endl;

    //Par défaut toutes les caméra seront affichées car nous mettons cette variable à -2.
    pt_app_params->actual_cam = -2;
    //Paramètre nous indiquant si l'animation est fini ou pas.
    pt_app_params->is_in_full = false;

    //Création des espaces mémoires des paramètre de chaque "thread" ainsi que de l'espace mémoire de ceux-ci.
    pt_app_params->thread_params = (thread_params_t *)malloc(sizeof(thread_params_t) * number_of_cam);
    memset(pt_app_params->thread_params, '\0', (sizeof(thread_params_t) * number_of_cam));
    pt_app_params->threads = (SDL_Thread **)malloc(sizeof(SDL_Thread *) * number_of_cam);
    //Création des deux paramètres qui contiendrons les variables principale de SDL.
    pt_app_params->window = NULL; 
    pt_app_params->renderer = NULL;

    //Création d'un rectangle SDL qui nous servira pour l'animation de la caméra de la mise en pleine écran de celle-ci.
    pt_app_params->rectFull.x = 0;
    pt_app_params->rectFull.y = 0;
    pt_app_params->rectFull.h = 0;
    pt_app_params->rectFull.w = 0;

    //Création d'un rectangle SDL qui nous servira pour l'animation du texte lors de la mise en pleine écran de la caméra.
    pt_app_params->txt_rectFull.x = 0;
    pt_app_params->txt_rectFull.y = 0;
    pt_app_params->txt_rectFull.h = 0;
    pt_app_params->txt_rectFull.w = 0;

    //Nombre de image composant l'animation de mise en plein écran.
    pt_app_params->number_animation_frame = 60;

    //Message dans le terminal prévenant l'initialisation de la librairie SDL2.
    std::cout << "Init sdl2..." << std::endl;
    //Initialisation de la librairie SDL qui gère l'affichage de la fenêtre de notre application.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    //Message dans le terminal prévenant l'initialisation de la librairie ttf.
    std::cout << "Init ttf_font..." << std::endl;
    //Initialisation de la librairie SDL qui gère l'affichage des textes de notre application.
    TTF_Init();
    if((pt_app_params->font = TTF_OpenFont("./ressources/arial.ttf", 18)) == NULL)
        printf("TTF_OpenFont: %s\n", TTF_GetError());

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;

    //Initialisation des deux variables gérants la fenêtre de l'application.
    if (SDL_CreateWindowAndRenderer(width, heigth, 0, &window, &renderer) < 0)
    {
        std::cerr << "Error creating window or renderer: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    //Stockage de ces deux variables dans nos paramètres d'application.
    pt_app_params->window = window;
    pt_app_params->renderer = renderer;

    //Mise en place de l'image de l'icône de notre application.
    SDL_Surface* iconSurface;
    iconSurface = SDL_LoadBMP("./ressources/icon.bmp");
    SDL_SetWindowIcon(window, iconSurface);

    //Retour du bon fonctionnemnt de cette fonction.
    return true;
    
}

/*
 * Fonction supprimant les espaces mémoires allouée par notre programme.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètres du programme.
 * Retour :
 *   - void : ne retourne rien. 
 */
void free_all(app_params_t *pt_app_params)
{
    //Variable contenant la valeur de retour des "threads".
    int thread_res = 0;
    //Créations d'un pointeur donnant l'adresse du tableau des paramètres des "threads".
    thread_params_t *threads_params = pt_app_params->thread_params;

    //Boucle sur les paramètres des "threads".
    for (uint i = 0; i < pt_app_params->number_of_cam; i++)
    {
        //Mise a faux du paramètre du "thread" qui fait qu'il s'arrête
        threads_params[i].alive = false;
        //Attente de la fin de vie du "thread".
        SDL_WaitThread(pt_app_params->threads[i], &thread_res);
        //Destruction de la texture de la caméra relié au "thread".
        SDL_DestroyTexture(threads_params[i].texture);
        //Destruction de la texture du texte relié au "thread".
        SDL_DestroyTexture(threads_params[i].txt_texture);
        //Destruction du mutex relié au "thread".
        SDL_DestroyMutex(threads_params[i].mutex);
        //Libération de la mémoire du rectangle permettant de placé le texte du "thread".
        free(threads_params[i].txt_dst);
        //Libération de la mémoire du rectangle permettant de placé le texte d'erreur du "thread".
        free(threads_params[i].no_camera_txt_dst);
        //Libération de la mémoire du rectangle permettant de savoir la taille de la caméra du "thread".
        free(threads_params[i].frame_size);
    }
    //Libération de la mémoire du rectangle de la destination des caméras.
    free(threads_params[0].dst);
    //Libération de l'espace mémoire pris par les "threads".
    free(pt_app_params->threads);
    //Libération de l'espace mémoire pris par les paramètres des "threads".
    free(threads_params);
    //Destruction de l'objet de rendu de SDL2.
    SDL_DestroyRenderer(pt_app_params->renderer);
    //Destruction de l'objet gérant la fenêtre de SDL2.
    SDL_DestroyWindow(pt_app_params->window);
    //Fermeture de la police permettant d'afficher les textes.
    TTF_CloseFont(pt_app_params->font);

    //Fermeture des deux librairies (SDL2 et ttf)
    TTF_Quit();
    SDL_Quit();
}

/*
 * Fonction gérant le rendu de le fenêtre.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètres du programme.
 * Retour :
 *   - void : ne retourne rien. 
 */
void render(app_params_t *pt_app_params)
{
    //Récupération de la variable qui nous permet de géré le rendu de la fenêtre.
    SDL_Renderer *renderer = pt_app_params->renderer;
    //Créations d'un pointeur donnant l'adresse du tableau des paramètres des "threads".
    thread_params_t *threads_params = pt_app_params->thread_params;
    //Créations d'une variable contenant le nombre de caméra de notre caméra.
    uint number_of_cam = pt_app_params->number_of_cam;

    //Vérification si le programme doit ou non afficher toutes les caméras.
    if(pt_app_params->actual_cam == -2){

        //Mise à faux la variable prévenant l'animation.
        pt_app_params->is_in_full = false;
        //Mise à un de l'index qui nous indiquera l'image en cours de l'animation.
        pt_app_params->index_render_frame = 1;        

        //Nettoyage de la variable de rendu.
        SDL_RenderClear(renderer);
        
        //Boucles sur les index des caméras.
        for (uint i = 0; i < number_of_cam; i++)
        {
            //Vérification si le "thread" à réussi à ce connecter au flux de la caméra.
            if (threads_params[i].stream_start)
            {
                //Rendu de la l'image de la caméra de la boucle.
                render_frame(pt_app_params, i);
            }
            else
            {
                //Rendu du texte indiquant que le "thread" ne ce connecte pas au flux de la caméra.
                renderNoCameraTxt(pt_app_params, i,threads_params[i].dst->x,threads_params[i].dst->y,threads_params[i].dst->w,threads_params[i].dst->h);
            }
        }

        //Mise à jour de la fenêtre avec le contenue de la variable de rendu.
        SDL_RenderPresent(renderer);

    }else{
        //S'exécute si une caméra est sélectionner pour être rendu dans toute la fenêtre.
        //Vérification que l'animation n'as pas déjà été faite.
        if(!pt_app_params->is_in_full){
            //Initialisation des variables pour bien commencer l'animation. 
            if(pt_app_params->index_render_frame == 1){
                pt_app_params->rectFull.x = 0;
                pt_app_params->rectFull.y = 0;
                pt_app_params->rectFull.w = pt_app_params->thread_params[pt_app_params->actual_cam].dst->w;
                pt_app_params->rectFull.h = pt_app_params->thread_params[pt_app_params->actual_cam].dst->h;
                pt_app_params->txt_rectFull.x = 0;
                pt_app_params->txt_rectFull.y = 0;
            }
            //Vérification que l'animation n'as pas fait sa dernière image d'animation.
            if(pt_app_params->index_render_frame <= pt_app_params->number_animation_frame){
                //Demande à la fonction d'animation de calculer la position de l'image de la caméra et du texte en fonction de l'index de l'image de l'animation. 
                animation(pt_app_params,pt_app_params->index_render_frame);
                //Ajout un à l'index de l'animation de l'image de la caméra.
                pt_app_params->index_render_frame+=1;
            }else{
                //Mise à vrai de la variable indiquant que l'animation est fini.
                pt_app_params->is_in_full = true;
            }
        }
        //Nettoyage de la variable de rendu.
        SDL_RenderClear(renderer);
        //Rendu de la l'image de la caméra de la boucle.
        render_frame(pt_app_params, pt_app_params->actual_cam);
        //Mise à jour de la fenêtre avec le contenue de la variable de rendu.
        SDL_RenderPresent(renderer);
    }
}

/*
 * Fonction gérant le rendu d'une texture d'une caméra.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètre du programme.
 *   - int cam_idx : variable contenant l'index de la caméra à rendre.
 * Retour :
 *   - void : ne retourne rien. 
 */
void render_frame(app_params_t *pt_app_params, int cam_idx)
{
    //Créations d'un pointeur donnant l'adresse du paramètre du "thread" correspondant.
    thread_params_t * thread_params = &pt_app_params->thread_params[cam_idx];

    //Vérification que la texture est vide et que le "thread" n'est pas lancé.
    if (!thread_params->texture && thread_params->stream_start){
        //Message dans le terminal prévenant la création de la texture pour le "thread".
        std::cout << "Creation texture" << std::endl;
        //Création d'une texture pour le "thread".
        thread_params->texture = create_texture(thread_params->frame_size, pt_app_params->renderer);
    }

    //Verrouillage du "mutex" pour que l'image ne soit pas modifier par le "thread" au même moment.
    SDL_LockMutex(thread_params->mutex);

    //Vérification que le "thread" est bien connecter à un flux.
    if(thread_params->stream_start){
        //Remplissage de la image dans la texture associé. 
        fillTexture(thread_params->texture, thread_params->frame);
    }else{
        //Remplissage du message d'erreur dans la texture associé.
        renderNoCameraTxt(pt_app_params, cam_idx,0,0,pt_app_params->width,pt_app_params->heigth);
    }
    
    //Vérification que le "thread" est bien connecter à un flux.
    if(thread_params->stream_start){ 
        //Vérification que les caméra sont toutes affichées.
        if(pt_app_params->actual_cam == -2){
            //Rendu de la texture de texture de la caméra dans la variable de rendu.
            SDL_RenderCopy(pt_app_params->renderer, thread_params->texture, thread_params->frame_size, thread_params->dst);
            //Rendu de la texture de texture du texte de la caméra dans la variable de rendu.
            SDL_RenderCopy(pt_app_params->renderer, thread_params->txt_texture, NULL, thread_params->txt_dst);
        }else if (cam_idx == pt_app_params->actual_cam){ 
            //Rendu de la texture de texture de la caméra dans la variable de rendu en prenant en compte la potentiel animation.
            SDL_RenderCopy(pt_app_params->renderer, thread_params->texture, thread_params->frame_size, &pt_app_params->rectFull);
            //Rendu de la texture de texture du texte de la caméra dans la variable de rendu en prenant en compte la potentiel animation.
            SDL_RenderCopy(pt_app_params->renderer, thread_params->txt_texture, NULL, &pt_app_params->txt_rectFull);
        } 
    }

    //Déverrouillage du "mutex" pour que le "thread" puisse remettre à jour l'image de la caméra.
    SDL_UnlockMutex(thread_params->mutex);
    
}

/*
 * Fonction plaçant le texte prévenant l'absence/la perte de connexion du flux vidéo.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètre du programme.
 *   - int cam_idx : index de la caméra prise en compte pour le message d'erreur.
 *   - int X : location à l'horizontal au coin supérieur gauche du texte de la zone de la caméra.
 *   - int Y : location à la verticale au coin supérieur gauche du texte de la zone de la caméra.
 *   - int W : largeur de la zone de la caméra.
 *   - int H : hauteur de la zone de la caméra.
 * Retour :
 *   - void : ne retourne rien. 
 */
void renderNoCameraTxt(app_params_t *pt_app_params, int cam_idx, int X, int Y, int W, int H){

    //Créations d'un pointeur donnant l'adresse du paramètre du "thread" correspondant.
    thread_params_t * thread_params = &pt_app_params->thread_params[cam_idx];

    int w = 0;
    int h = 0;
    //Récupération de la largueur et hauteur du texte.
    SDL_QueryTexture(thread_params->no_camera_txt_texture, NULL, NULL, &w, &h);

    //Placement du texte au centre la zone de la caméra.
    thread_params->no_camera_txt_dst->w = w;
    thread_params->no_camera_txt_dst->h = h;
    thread_params->no_camera_txt_dst->x = X+(W/2-w/2);
    thread_params->no_camera_txt_dst->y = Y+(H/2-h/2); 

    //Rendu de la texture de ce texte dans la variable de rendu.
    SDL_RenderCopy(pt_app_params->renderer, thread_params->no_camera_txt_texture, NULL, thread_params->no_camera_txt_dst);

}

/*
 * Fonction générant la texture du texte prévenant l'absence/la perte de connexion du flux vidéo d'une caméra.
 * Argument(s) : 
 *   - app_params_t *pt_app_params : variable contenant les paramètre du programme.
 *   - SDL_Rect *txt_dst : rectangle représentant la destination du texte.
 *   - int cam_idx : index de la caméra prise en compte pour le message d'erreur.
 * Retour :
 *   - SDL_Texture * : retourne un pointeur de la texture générée. 
 */
SDL_Texture * noCameraTxt(app_params_t *pt_app_params, SDL_Rect *txt_dst, int cam_idx){
    //Variable stockant le texte affiché.
    char erreurNoCamera[45];
    //Pointeur vers la future surface et texture du texte.
    SDL_Texture * txt_texture;
    SDL_Surface * txt_surface; 
    //Variable contenant la couleur blanche pour la librairie SDL2.
    SDL_Color couleurBlanc = {255,255,255};

    //Ajout du message d'erreur dans la variable.
    snprintf(erreurNoCamera, sizeof(erreurNoCamera), "Caméra n°%d non détéctée !", cam_idx+1);

    //Création de la surface correspondante avec la librairie ttf.
    txt_surface = TTF_RenderUTF8_Solid(pt_app_params->font, erreurNoCamera, couleurBlanc);

    //Création de la texture qui contiendera le surface.
    txt_texture = SDL_CreateTextureFromSurface(pt_app_params->renderer, txt_surface);
    
    //Libération de l'espace mémoire pris par la surface qui à été imprimé dans la texture.
    SDL_FreeSurface(txt_surface);

    //Renvoi de la texture contenant le message.
    return txt_texture;
}

/*
 * Fonction remplissant une texture à partir d'une matrice de la librairie opencv.
 * Argument(s) : 
 *   - SDL_Texture * texture : pointeur de la texture à remplir.
 *   - cv::Mat mat : matrice opencv à copier dans la texture.
 * Retour :
 *   - void : ne retourne rien. 
 */
void fillTexture(SDL_Texture * texture, cv::Mat mat)
{
    //Création d'un pointeur donnant les données de la texture.
    unsigned char * texture_data = NULL;
    //Création d'un variable pour contenir la hauteur des pixels vérouillé.
    int texture_pitch = 0;

    //Verrouillage de la texture donnée et récupération du pointeur de ces données.
    SDL_LockTexture(texture, 0, (void **)&texture_data, &texture_pitch);
    //Copie des données de la matrice dans les données de la texture.
    memcpy(texture_data, (void *)mat.data, mat.rows * mat.cols * mat.channels());
    //Dévérouillage de la texture.
    SDL_UnlockTexture(texture);
}

/*
 * Fonction initialisant les paramètre du programme.
 * Argument(s) : 
 *   - SDL_Rect *rect : pointeur vers un rectangle représentant l'emplacement de la futur texture.
 *   - SDL_Renderer *renderer : pointeur vers la variable de rendu.
 * Retour :
 *   - SDL_Texture : retourne un pointeur de la texture générée.
 */
SDL_Texture *create_texture(SDL_Rect *rect, SDL_Renderer *renderer)
{
    //Renvoi la création d'une texture relié au rendu donné en paramètre avec les bonnes configurations.
    return SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_BGR24,
        SDL_TEXTUREACCESS_STREAMING,
        rect->w,
        rect->h);
}

