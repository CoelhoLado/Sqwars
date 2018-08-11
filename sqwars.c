#include <stdio.h>
#include <math.h>
#include <time.h>

#include "SDL2/SDL.h"

#define MAX_BUFFER		20480

typedef enum {EMPTY, WALL, FIRE, WATER} PIECE;

typedef struct player{
    float posX, posY, velX, velY, ang;
    int HP, max_HP;
    int speed, strength, range;
    int actions[10];
    int class; // (0 = warrior) (1 = mage) (2 = rogue prai)
    //SKILLS skillBuild;
} PLAYER;

typedef struct skills{
    //huehuehue
} SKILLS;

/*typedef struct entity{
    float posX;
    float posY;
    float velX;
    float velY;
    int HP, max_HP;
    int speed;
    int strenght;
    int agressive;
    int range;
} ENTITY;*/

/*typedef struct type{
    ENTITY zombie;
    ENTITY giant;
    ENTITY spider;
    int level;
} TYPE;*/

typedef struct gameState {
    PIECE map[30][30];
    //ENV environment;
    //int intensity;
    PLAYER player;
    //TYPE enemies[30];
} STATE;

//typedef enum {MENUS, REGULAR, NETHER, DESERT, SEA} ENV;

char *estado2str(STATE e) {
	static char buffer[MAX_BUFFER];
	unsigned char *p = (unsigned char *) &e;
	int i;

	buffer[0] = 0;

	for(i = 0; i < (int) sizeof(STATE); i++)
		sprintf(buffer, "%s%02x", buffer, p[i]);

	return buffer;
}

STATE str2estado(char *argumentos) {
	STATE e;
	unsigned char *p = (unsigned char *) &e;
	int i;

	for(i = 0; i < (int) sizeof(STATE); i++, argumentos += 2) {
		unsigned d;
		sscanf(argumentos, "%2x", &d);
		p[i] = (unsigned char) d;
	}
	return e;
}

void saveFileState(STATE state) {
	char fState[MAX_BUFFER];
	sprintf(fState, "%s/estado.txt","./estado");
	FILE *fp = fopen(fState,"w");
	fprintf(fp,"%s", estado2str(state));
	fclose(fp);
}

/**
Função que verifica se um ficheiro existe
@param *fname O diretório do ficheiro com o estado
@returns 1 se o ficheiro existir
*/
int exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

//Preenche 3ª linha do mapa com parede
/*STATE fillMap(STATE state){
    int l, c;
    for(l = 0; l < 26; l++)
        for(c = 0; c < 26; c++){
            state.map[l][c] = EMPTY;
            state.map[0][c] = WALL;
            state.map[25][c] = WALL;
            state.map[l][0] = WALL;
            state.map[l][25] = WALL;
        }
    for(c = 5; c < 21; c++){
        state.map[23][c] = FIRE;
    }
    for(c = 5; c < 21; c++){
        state.map[12][c] = WATER;
        state.map[13][c] = WATER;
    }
    for(c = 5; c < 21; c++){
        state.map[2][c] = WALL;
        state.map[0][c] = WALL;
        state.map[25][c] = WALL;
    }
    return state;
}*/

STATE readMap(STATE state, char *mapName) {
	char mapLine[MAX_BUFFER];
	int l, c, lmax, cmax;
	char v;
	FILE *fp = fopen(mapName,"r");
	fscanf(fp,"%d %d",&lmax,&cmax);
	for(l = 0; l < lmax; l++){
		fscanf(fp,"%s",mapLine);
		for(c = 0; c < cmax; c++){
			v = mapLine[c];
			switch(v){
				case '.':
					state.map [l] [c] = EMPTY;
					break;
				case 'F':
					state.map [l] [c] = FIRE;
					break;
				case 'W':
					state.map [l] [c] = WATER;
					break;
				default:
					state.map [l] [c] = WALL;
					break;
			}
		}
	}
	fclose(fp);
    return state;
}

/**
Função que lê o estado do jogo apartir de um ficheiro
@param *fState O diretório do ficheiro com o estado
@returns O estado do jogo extraido do ficheiro
*/
STATE readFileState(char *fState){
	char est[MAX_BUFFER];
	STATE e;
    int i = 0;
	if(exists(fState)){
		FILE *fp = fopen(fState,"r");
		i = fscanf(fp,"%s", est);
		fclose(fp);
        e = str2estado(est);
		return e;
	}
	else{
        e.player.posX = 3;
        e.player.posY = 6;
        e.player.velX = 0;
        e.player.velY = 0;
        e.player.ang = 0;
        e.player.max_HP = 150;
        e.player.HP = 130;
        e = readMap(e, "./mapas/map1.txt");
		return e;
	}
}

void drawMap(SDL_Renderer *renderer, STATE *state){
    int l ,c;
    for(l = 0; l < 22; l++)
        for(c = 0; c < 26; c++){
            switch(state -> map[l][c]){
                case EMPTY:{
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_Rect rect = { 25*c+30, 25*l+30, 25, 25};
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                }
                case WALL:{
                    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 200);
                    SDL_Rect rect = { 25*c + 30, 25*l + 30, 25, 25};
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                }
                case FIRE:{
                    SDL_SetRenderDrawColor(renderer, 255, 110, 0, 225);
                    SDL_Rect rect = { 25*c + 30, 25*l + 30, 25, 25};
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                }
                case WATER:{
                    SDL_SetRenderDrawColor(renderer, 0, 60, 195, 195);
                    SDL_Rect rect = { 25*c + 30, 25*l + 30, 25, 25};
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                }
            }
        }
}

void drawPlayer(SDL_Renderer *renderer, STATE *state){
    SDL_Texture* pTexture;
    int SDL_SetTextureColorMod(pTexture, 255, 40, 255);

    SDL_SetRenderDrawColor(renderer, 255, 40, 255, 150);
    SDL_Rect rect = { state -> player.posX * 25 + 25, state -> player.posY * 25 + 25, 10, 10};
    SDL_RenderCopyEx(renderer, pTexture, &rect);
}

void drawHP(SDL_Renderer *renderer, STATE *state){
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 150);
    SDL_Rect HP = { 700, 30, state->player.HP, 20};
    SDL_RenderFillRect(renderer, &HP);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
    SDL_Rect HP_lost = { 700 + state->player.HP, 30, state->player.max_HP - state->player.HP, 20};
    SDL_RenderFillRect(renderer, &HP_lost);
}

/*STATE movement(STATE state){


}*/

FLOAT getAng(PLAYER player){
    float posx, posy, x , y, ang;
    posx = player.posX;
    posy = player.posY;
    SDL_GetMouseState(&x, &y);
    ang = atan((y - posy)/(x - posx))
    if ((x - posx) < 0) ang = ang + pi;
    return ang;
}

STATE updatePlayer(STATE state){
    PIECE nextPieceY, nextPieceX, piece;
    piece = state.map[(int) (state.player.posY)][(int) (state.player.posX)];
    nextPieceY = state.map[(int) (state.player.posY + state.player.velY)][(int) (state.player.posX)];
    nextPieceX = state.map[(int) (state.player.posY)][(int) (state.player.posX + state.player.velX)];
    if(nextPieceY == WALL) state.player.velY = 0;
    if(nextPieceX == WALL) state.player.velX = 0;
    if(piece == FIRE && state.player.HP > 0) state.player.HP -= 4;
    if(state.player.HP <= 0) {
        state.player.posX = 3;
        state.player.posY = 6;
        state.player.HP = state.player.max_HP;
    }
    if(piece == WATER){
        state.player.velX = state.player.velX/2;
        state.player.velY = state.player.velY/2;
    }
    state.player.posX += state.player.velX;
    state.player.posY += state.player.velY;
    state.player.ang = getAng(state.player);
    return state;
}

void processMovementEvents(PLAYER *player, SDL_Event event){
    //if(event.key.repeat == 1) return;
    switch(event.type){
        case SDL_KEYDOWN:{
            switch(event.key.keysym.sym){
                case SDLK_DOWN:{
                    player -> actions[0] = 1;
                    return;
                break;
                }
                case SDLK_UP:{
                    player -> actions[1] = 1;
                    return;
                break;
                }
                case SDLK_RIGHT:{
                    player -> actions[2] = 1;
                    return;
                break;
                }
                case SDLK_LEFT:{
                    player -> actions[3] = 1;
                    return;
                break;
                }
            }
        }
        case SDL_KEYUP:{
            switch(event.key.keysym.sym){
                case SDLK_DOWN:{
                    player -> actions[0] = 0;
                    return;
                break;
                }
                case SDLK_UP:{
                    player -> actions[1] = 0;
                    return;
                break;
                }
                case SDLK_RIGHT:{
                    player -> actions[2] = 0;
                    return;
                break;
                }
                case SDLK_LEFT:{
                    player -> actions[3] = 0;
                    return;
                break;
                }
            }
        }
    }
    return;
}

STATE processA (STATE state) {
    if (state.player.actions[0]) state.player.velY = 0.2;
    if (state.player.actions[1]) state.player.velY = -0.2;
    if (state.player.actions[2]) state.player.velX = 0.2;
    if (state.player.actions[3]) state.player.velX = -0.2;
    if (state.player.actions[0] == state.player.actions[1]) state.player.velY = 0;
    if (state.player.actions[2] == state.player.actions[3]) state.player.velX = 0;
    return state;
}

int processCloseEvents(SDL_Window *window, SDL_Event event){
  int done = 0;
    switch(event.type)
    {
      case SDL_WINDOWEVENT_CLOSE:
      {
        if(window)
        {
          SDL_DestroyWindow(window);
          window = NULL;
          done = 1;
        }
      }
      break;
      case SDL_KEYUP:
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            done = 1;
          break;
        }
      }
      break;
      case SDL_QUIT:
        //quit out of the game
        done = 1;
      break;
    }
  return done;
}

int processEvents(SDL_Window *window, STATE *state){
    int done = 0;
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        done = processCloseEvents(window, event);
        processMovementEvents(&(state -> player), event);
    }
    return done;
}


void doRender(SDL_Renderer *renderer, STATE *state){
    //set the drawing color to Black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    //Clear the screen (to blue)
    SDL_RenderClear(renderer);
    //desenha MAPA
    drawMap(renderer, state);
    //desenha HP
    drawHP(renderer, state);
    //desenha PLAYER
    drawPlayer(renderer, state);
    //We are done drawing, "present" or show to the screen what we've drawn
    SDL_RenderPresent(renderer);
}


int main(/*int argc, char *argv[]*/){
    srand(time(NULL));
    STATE gstate;
    gstate = readFileState("./estado/estado.txt");
    SDL_Window *window;                    // Declare a window
    SDL_Renderer *renderer;                // Declare a renderer
    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
    //Create an application window with the following settings:
    window = SDL_CreateWindow("Game Window",                     // window title
                            SDL_WINDOWPOS_UNDEFINED,           // initial x position
                            SDL_WINDOWPOS_UNDEFINED,           // initial y position
                            1360,                               // width, in pixels
                            760,                               // height, in pixels
                            0                                  // flags
                            );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // The window is open: enter program loop (see SDL_PollEvent)
    int done = 0;
    //Event loop
    while(!done){
        //Check for events
        done = processEvents(window, &gstate);
        //Render display
        doRender(renderer, &gstate);
        gstate = processA(gstate);
        gstate = updatePlayer(gstate);
        SDL_Delay(33);
    }
    saveFileState(gstate);
    // Close and destroy the window
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    // Clean up
    SDL_Quit();
    return 0;
}
