
/* gifdec example -- simple GIF player using SDL2
 * compiling:
 *   cc `pkg-config --cflags --libs sdl2` -o example gifdec.c example.c
 * executing:
 *   ./example animation.gif
 * */

#include <stdio.h>
#include <string.h>
#include <SDL.h>

#include "binimagearray.h"

static int mode = 0;
static int msbfg = 1;
static int a_width = 0;
static int a_height = 0;
uint8_t *bitbuffer = NULL;
bia_t bia;




Uint8  getcolor(int s_x,int s_y,Uint8* frame)
{
	return bia_getpixel(s_x,s_y,frame,&bia);
}

Uint32  drawpixel(int s_x,int s_y,const SDL_PixelFormat * format,Uint8* frame)
{
	if(getcolor(s_x,s_y,frame))
	{
		return SDL_MapRGB(format, 0, 0, 255);
	}
	else
	{
		return SDL_MapRGB(format, 255, 255, 255);
	}
}

void drawframe2Surface(SDL_Surface *surface,Uint8* frame)
{
	int i,j;
	Uint32 pixel;
	void *addr;
	
	SDL_LockSurface(surface);

	for (i = 0; i < a_height; i++) 
	{
	    for (j = 0; j < a_width; j++) 
		{ 
	        pixel = drawpixel(j,i,surface->format,frame);
	        addr = surface->pixels + (i * surface->pitch + j * sizeof(pixel));
	        memcpy(addr, &pixel, sizeof(pixel));
	       
	    }
	}

	SDL_UnlockSurface(surface);
}

SDL_Texture* RenderDrawStart(    SDL_Renderer *renderer,SDL_Surface *surface)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	
	SDL_RenderCopy(renderer, texture, NULL, NULL);

	return texture;

}

void RenderStopandShow(    SDL_Renderer *renderer,SDL_Texture* texture)
{
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);

}

int
main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Event event;
    char title[32] = {0};
	Uint32 t0, t1, delay, delta;
	
    int i, j;
    int ret, paused, quit;
	uint8_t stop = 0;
	FILE *stream; 
    if (argc != 2) 
	{
        fprintf(stderr, "no bin file\n");
        return 1;
    }

	stream = fopen(argv[1],"rb+");
	if(!stream)
	{
		fprintf(stderr, "Could not open %s\n",argv[1]);
        return 1;
	}

	 if(1 != fread( &bia, sizeof(bia_t), 1, stream ))
	 {
		fprintf(stderr, "Read %s error!\n",argv[1]);
        return 1;
	 }

	 if(bia.id != BIA_HEAD_ID)
	 {
		fprintf(stderr, "Check ID[%x] error!\n",bia.id);
        return 1;
	 }

	 if(bia.arrange_mode >= BIA_ARRANGE_MAX)
	 {
		fprintf(stderr, "Check mode[%d] error!\n",bia.arrange_mode+1);
        return 1;
	 }

	if(bia.size != (bia.number*bia.width*bia.height/8))
	{
		fprintf(stderr, "Check size[%x] error! number[%d] width[%d] height[%d]\n",bia.size,bia.number,bia.width,bia.height);
		return 1;
	}

	a_width = bia.width;
	a_height = bia.height;
	mode = bia.arrange_mode;
	msbfg = bia.msb;
	bia.delay = bia.delay?bia.delay:10;
	bitbuffer = malloc(a_width*a_height/8);
	if(!bitbuffer)
	{
		fprintf(stderr, "Could not allocate buffer\n");
        return 1;
	}
	memset(bitbuffer,0,a_width*a_height/8);
	
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) 
	{
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    if (SDL_CreateWindowAndRenderer(a_width, a_height, SDL_WINDOW_RESIZABLE, &window, &renderer)) 
	{
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return 1;
    }
    snprintf(title, sizeof(title) - 1, "GIF %dx%d",a_width, a_height);
    SDL_SetWindowTitle(window, title);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    surface = SDL_CreateRGBSurface(0, a_width, a_height, 32, 0, 0, 0, 0);
    if (!surface) 
	{
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        return 1;
    }

	fprintf(stdout, "gifurl[%s] arrangemode[%d] MSB[%d] width[%d] height[%d]\n",
		argv[1],
		mode,
		msbfg,
		a_width,
		a_height);

    paused = 0;
    quit = 0;
    while (1) 
	{
		while(SDL_PollEvent(&event) || stop)
		{
			if (event.type == SDL_QUIT)
            	quit = 1;
		
        	if (event.type == SDL_KEYDOWN) 
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_q:
						quit = 1;
						break;
					
					case SDLK_SPACE:
						stop = stop?0:1;
						break;
				}
	        }
       
	        if (quit) 
				break;
			SDL_Delay(10);
		}
		if(quit)
			break;
		if(stop)
		{
			SDL_Delay(10);
			continue;
		}
        t0 = SDL_GetTicks();
		memset(bitbuffer,0,a_width*a_height/8);
		if(1 != fread( bitbuffer, a_width*a_height/8, 1, stream ))
		{
			fprintf(stdout, "Finish!\n");
			break;
		}
		
		drawframe2Surface(surface,bitbuffer);

		texture = RenderDrawStart(renderer,surface);

		RenderStopandShow(renderer,texture);

		t1 = SDL_GetTicks();
        delta = t1 - t0;
        delay = bia.delay * 10;
        delay = delay > delta ? delay - delta : 1;
        SDL_Delay(delay);
		
    }

	fclose(stream);

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(bitbuffer);
    return 0;
}



