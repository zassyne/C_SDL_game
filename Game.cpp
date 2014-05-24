#include "Game.h"
#include <iostream>


void Game::getInput()
 {
    SDL_Event event;

    /* Keymapping : gère les appuis sur les touches et les enregistre
    dans la structure input */

    while (SDL_PollEvent(&event))
    {
        switch ( event.type)
        {

            case SDL_QUIT:
                exit(0);
            break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        exit(0);
                    break;

                    case SDLK_DELETE:
                        input.erase = 1;
                    break;

                    case SDLK_c:
                        input.jump = 1;
                    break;

                      case SDLK_v:
                        input.attack = 1;
                    break;

                    case SDLK_LEFT:
                        input.left = 1;
                    break;

                    case SDLK_RIGHT:
                        input.right = 1;
                        std::cout <<"Right"<<std::endl;

                    break;

                    case SDLK_DOWN:
                        input.down = 1;
                    break;

                    case SDLK_UP:
                        input.up = 1;
                    break;

                    case SDLK_RETURN:
                        input.enter = 1;
                    break;

                    default:
                    break;
                }
            break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    case SDLK_DELETE:
                        input.erase = 0;
                    break;

                    case SDLK_c:
                        input.jump = 0;
                    break;

                    case SDLK_LEFT:
                        input.left = 0;
                    break;

                    case SDLK_RIGHT:
                        input.right = 0;
                    break;

                    case SDLK_DOWN:
                        input.down = 0;
                    break;

                    case SDLK_UP:
                        input.up = 0;
                    break;

                    case SDLK_RETURN:
                        input.enter = 0;
                    break;

                    default:
                    break;
                }
            break;

        }

    }
 }

Game::Game(const char* title)
{
    init(title);
    running = true;
}

bool Game::isRunning()
{
    return running;
}


 void Game::drawMap(void)
 {


    int x, y, mapX, x1, x2, mapY, y1, y2, xsource, ysource, a;

    mapX = startX / TILE_SIZE;

    /* Coordonnées de départ pour l'affichage de la map : permet
    de déterminer à quels coordonnées blitter la 1ère colonne de tiles au pixel près
    (par exemple, si la 1ère colonne n'est visible qu'en partie, on devra commencer à blitter
    hors écran, donc avoir des coordonnées négatives - d'où le -1). */
    x1 = (startX % TILE_SIZE) * -1;

    /* Calcul des coordonnées de la fin de la map : jusqu'où doit-on blitter ?
    Logiquement, on doit aller à x1 (départ) + SCREEN_WIDTH (la largeur de l'écran).
    Mais si on a commencé à blitter en dehors de l'écran la première colonne, il
    va falloir rajouter une autre colonne de tiles sinon on va avoir des pixels
    blancs. C'est ce que fait : x1 == 0 ? 0 : TILE_SIZE qu'on pourrait traduire par:
    if(x1 != 0)
        x2 = x1 + SCREEN_WIDTH + TILE_SIZE , mais forcément, c'est plus long ;)*/
    x2 = x1 + SCREEN_WIDTH + (x1 == 0 ? 0 : TILE_SIZE);

    /* On fait exactement pareil pour calculer y */
    mapY = startY / TILE_SIZE;
    y1 = (startY % TILE_SIZE) * -1;
    y2 = y1 + SCREEN_HEIGHT + (y1 == 0 ? 0 : TILE_SIZE);


    /* Dessine la carte en commençant par startX et startY */

    /* On dessine ligne par ligne en commençant par y1 (0) jusqu'à y2 (480)
    A chaque fois, on rajoute TILE_SIZE (donc 32), car on descend d'une ligne
    de tile (qui fait 32 pixels de hauteur) */

    for (y = y1; y < y2; y += TILE_SIZE)
    {
        /* A chaque début de ligne, on réinitialise mapX qui contient la colonne
        (0 au début puisqu'on ne scrolle pas) */

        mapX = startX / TILE_SIZE;

        /* A chaque colonne de tile, on dessine la bonne tile en allant
        de x = 0 à x = 640 */

        for (x = x1; x < x2; x += TILE_SIZE)
        {

            /* Suivant le numéro de notre tile, on découpe le tileset */

            a = tile[mapY][mapX];

            /* Calcul pour obtenir son y (pour un tileset de 10 tiles
            par ligne, d'où le 10 */

            ysource = a / 10 * TILE_SIZE;

            /* Et son x */

            xsource = a % 10 * TILE_SIZE;

            /* Fonction qui blitte la bonne tile au bon endroit */

            drawTile(tileSet, x, y, xsource, ysource);

            mapX++;
        }

        mapY++;
    }
  }
void Game::init(const char* title)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Couldnt initialize SDL:"<<SDL_GetError()<<std::endl;
        return;
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0,SDL_DOUBLEBUF);

    if(screen ==  0)
    {
        std::cout << "Couldnt set the screen:"<<SDL_GetError()<<std::endl;
        return;
    }

    SDL_WM_SetCaption(title, NULL);

    background = loadImage("graphics/background.bmp");
    tileSet = loadImage("graphics/tileset.bmp");

    loadMap("Map/map1.txt");
}


void Game::loadMap(const char* path)
 {
    int x, y;
    FILE *fp;

    fp = fopen(path, "rb");

    //Si on ne peut pas ouvrir le fichier, on quitte

    if (fp == NULL)
    {
        printf("Failed to open map %s\n", path);
        exit(1);
    }

    /* Lit les données du fichier dans la map*/

    maxX = maxY = 0;

    for (y = 0; y < MAX_MAP_Y; y++)
    {
        for (x = 0; x < MAX_MAP_X; x++)
        {
            /* On lit le numéro de la tile et on le copie dans notre tableau*/

            fscanf(fp, "%d", &tile[y][x]);

            /* Permet de déterminer la taille de la map (voir plus bas)*/

            if (tile[y][x] > 0)
            {
                if (x > maxX)
                {
                    maxX = x;
                }

                if (y > maxY)
                {
                    maxY = y;
                }
            }

        }
    }
    std::cout << maxX <<std::endl;
    std::cout << maxY <<std::endl;

    maxX = (maxX + 1) * TILE_SIZE;
    maxY = (maxY + 1) * TILE_SIZE;

    startX = startY = 0;


    /* Et on referme le fichier*/

    fclose(fp);
}


void Game::update(void)
{
    if (input.left == 1)
    {
        startX -= TILE_SIZE;
        if (startX < 0)
        {
            startX = 0;
        }
    }

    if (input.right == 1)
    {
        startX += TILE_SIZE;

        if (startX + SCREEN_WIDTH >= maxX)
        {
            startX = maxX - SCREEN_WIDTH;
        }
    }

    if (input.up == 1)
    {
        startY -= TILE_SIZE;

        if (startY < 0)
        {
            startY = 0;
        }
    }

    if (input.down == 1)
    {
        startY += TILE_SIZE;

        if (startY + SCREEN_HEIGHT >= maxY)
        {
            startY = maxY - SCREEN_HEIGHT;
        }
    }

}


SDL_Surface* Game::loadImage(const char* path)
{
    SDL_Surface* tmp = SDL_LoadBMP(path);
    SDL_Surface* image;

    if(tmp == NULL)
    {
       std::cout << "Failed to load image: "<<path<<std::endl;
       return NULL;
    }

    image = SDL_DisplayFormat(tmp);

    SDL_FreeSurface(tmp);

    if(image == NULL)
    {
        std::cout << "Failed to convert image to native format"<<path<<std::endl;
        return NULL;
    }

    return image;
}



void Game::drawTile(SDL_Surface* image, int destX, int destY, int srcX, int srcY)
{
    SDL_Rect dest;

    dest.x = destX;
    dest.y = destY;
    dest.w = TILE_SIZE;
    dest.h = TILE_SIZE;

    SDL_Rect src;

    src.x = srcX;
    src.y = srcY;
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;

    SDL_BlitSurface(image, &src, screen, &dest);
}


Game::~Game()
{
    if(background != NULL)
    {
        SDL_FreeSurface(background);
    }

    if(tileSet != NULL)
    {
        SDL_FreeSurface(tileSet);
    }

    SDL_Quit();
}


void Game::draw()
{
    drawImage(background, 0, 0);

    drawMap();

    SDL_Flip(screen);
}


void Game::drawImage(SDL_Surface* image, int x, int y)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, NULL, screen, &dest);

}

