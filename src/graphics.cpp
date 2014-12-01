#include "graphics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <zlib.h>
#include <png.h>

const int TSZ = 128;

SDL_Window* mainwin = NULL;

SDL_Surface* screenSurface = NULL; 

SDL_Surface * surf_spritesheet = NULL;

SDL_Rect rectClips [16];

void startupWindow()
{
	// Init SDL
	
	cout << "Initializing graphics..." << endl;
	
	cout << "- starting SDL..." << endl;
	
	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		cout << "ERROR INITIALIZING SDL" << endl;
		exit(1);
	};

	cout << "- creating window..." << endl;

	mainwin = SDL_CreateWindow(	"La Macchina", 
								SDL_WINDOWPOS_UNDEFINED,
								SDL_WINDOWPOS_UNDEFINED,
								TSZ*8,
								TSZ*8,
								SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	
	screenSurface = SDL_GetWindowSurface( mainwin );
	
	// Init SDL_image
	
	cout << "- starting SDL_image..." << endl;
	
	int imgFlags = IMG_INIT_PNG;
	if( not (IMG_Init(imgFlags) & imgFlags))
	{
		cout << "ERROR INITIALIZING SDL_image" << endl;
		exit(1);
	};
	
	// carica img/spritesheet.png
	
	cout << "- loading spritesheet..." << endl;
	
	SDL_Surface* loads = IMG_Load( "img/spritesheet.png" );
	if (loads == NULL)
	{
		cout << "ERROR LOADING img/spritesheet.bmp: " << IMG_GetError() << endl;
		exit(1);
	}
	
	
	
	//surf_spritesheet = SDL_ConvertSurface(  loads, screenSurface->format, 0);
	//if (surf_spritesheet == NULL)
	//{
		//cout << "ERROR OPTIMIZING img/spritesheet.png" << endl;
		//exit(1);
	//}
	
	surf_spritesheet = loads;
	//SDL_FreeSurface(loads);
	
	SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_BLEND); 
	SDL_SetSurfaceBlendMode(surf_spritesheet, SDL_BLENDMODE_BLEND); 
	
	//Uint32 colorkey = SDL_MapRGB( surf_spritesheet->format, 0, 0xFF, 0xFF );
	//SDL_SetColorKey( surf_spritesheet, SDL_TRUE, colorkey );
	

	
	// prepara rettangoli di sprite clipping
	
	cout << "- init clipping array..." << endl;
	
	for (int i=0; i<16; i++)
	{
		int x = i%4;
		int y = i/4;
		rectClips[i].x = TSZ*x;
		rectClips[i].y = TSZ*y;
		rectClips[i].w = TSZ;
		rectClips[i].h = TSZ;
	};
	
	cout << "complete." << endl;
	
}

void graphTile(int x, int y, unsigned char tileval)
{
	SDL_Rect renderQuad = { x*TSZ, y*TSZ, TSZ, TSZ };
	SDL_BlitSurface(surf_spritesheet, &rectClips[tileval], screenSurface, &renderQuad);
};

const unsigned char fliparray [8] = {VOID, BLACK, WHITE, BQUEEN, WQUEEN,0,0,0};

unsigned char graphFlip(unsigned char v, bool flip)
{
	if (not flip)
		return v;
	else
	{
		return fliparray[v];
	}
}

void graphBoard(State * s, bool flip)
{
	
	int x,y;
	for(int i =0; i<64; i++)
	{
		x = i%8;
		y = i/8;
		int wood;
		if ((x+y)%2)
			wood = 12;
		else
			wood = 9;
		graphTile(x,y,wood);
	}
	
	for(int i=0; i<32; i++)
	{
		if(s->data[i]>0)
		{	
			int y = i/4;
			int x = 2*(i%4) + 1 - (i/4)%2;
		
			graphTile(x,y,graphFlip(s->data[i],flip));
		}
	}
}

void graphPointers(Move m)
{
	if( m[0] >= 32)
		return;
		
	int i = m[0];
	int y = i/4;
	int x = 2*(i%4) + 1 - (i/4)%2;
	graphTile(x,y,5);
	
	if (m.size() > 0)
	{
		i = m.back();
		y = i/4;
		x = 2*(i%4) + 1 - (i/4)%2;
		graphTile(x,y,6);
	}
}

void graphRefresh()
{
	SDL_UpdateWindowSurface( mainwin );
}


void graphCleanup()
{
	SDL_FreeSurface(surf_spritesheet);
	
	SDL_DestroyWindow( mainwin );
	
	SDL_Quit();
}
