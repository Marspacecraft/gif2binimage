/* gifdec example -- simple GIF player using SDL2
 * compiling:
 *   cc `pkg-config --cflags --libs sdl2` -o example gifdec.c example.c
 * executing:
 *   ./example animation.gif
 * */

#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <math.h> 


#include "gifdec.h"
#include "binimagearray.h"

#define BIN_INCLUDE_FILE \
"#include \"binimagearray.h\"\n\n"\
"\n"\
"bia_t g_bitimage =\n"\
"{\n"\
".id = BIA_HEAD_ID,\n"\
".arrange_mode = %d,\n"\
".msb = %d,\n"\
".width = %d,\n"\
".height = %d,\n"\
".number = %d,\n"\
".delay = %d,\n"\
".size = %d,\n"\
"};\n"


#define RECT_STEP_SIZE 4

static int mode = 0;
static int msbfg = 1;
static int rotate = 0;
static int flip = 0;
static int a_width = 0;
static int a_height = 0;
static int s_width = 0;
static int s_height = 0;
static Uint8 gray = 0x7f;
static Uint8 showlittle = 0;
static float recttime = 1;
uint8_t *bitbuffer = NULL;
bia_t bia;
SDL_Rect rect;


Uint8 * getcolor(int s_x,int s_y,Uint8* frame)
{
	int tmp;
	int width = s_width;
	switch(rotate)
	{
		case 90:
			tmp = s_x;
			s_x = s_y;
			s_y = s_width - tmp - 1;
			width = s_height;
			break;
		case 180:
			s_x = s_width - s_x - 1;
			s_y = s_height - s_y - 1;
			break;
		case 270:
			tmp = s_y;
			s_y = s_x;
			s_x = s_height - tmp - 1;
			width = s_height;
			break;
	}
	
	if(flip)
	{
		s_x = width - s_x - 1;
	}

	return &frame[3*(s_y*width+s_x)];
}

static Uint8 rgb_to_gray(Uint8 r, Uint8 g, Uint8 b) 
{
    return (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
}


Uint32  drawpixel(int s_x,int s_y,const SDL_PixelFormat * format,Uint8 r, Uint8 g, Uint8 b)
{
	Uint8 sgray;
	if(((s_x>=rect.x) && (s_x < (rect.x + rect.w)))
		&&
		((s_y>=rect.y) && (s_y < (rect.y + rect.h))))
	{
			sgray = rgb_to_gray(r,g,b);
			if(sgray < gray)
			{
				int s_i = round((s_x - rect.x)/recttime);
				int s_j = round((s_y - rect.y)/recttime);
				bia_setpixel(s_i,s_j,bitbuffer,&bia);
				return SDL_MapRGB(format, 0, 255, 0);
			}
			else
			{
				return SDL_MapRGB(format, 255, 255, 255);
			}
	}
	else
	{
		return SDL_MapRGB(format, r, g,b);
	}
}

void drawframe2Surface(gd_GIF *gif,SDL_Surface *surface,Uint8* frame)
{
	Uint8 *color;
	int i,j;
	Uint32 pixel;
	void *addr;
	
	SDL_LockSurface(surface);
	gd_render_frame(gif, frame);
	//color = frame;
	for (i = 0; i < s_height; i++) 
	{
	    for (j = 0; j < s_width; j++) 
		{ 
			color = getcolor(j,i,frame);
	        pixel = drawpixel(j,i,surface->format, color[0], color[1], color[2]);
	        addr = surface->pixels + (i * surface->pitch + j * sizeof(pixel));
	        memcpy(addr, &pixel, sizeof(pixel));
	       
	    }
	}

	if(showlittle)
	{
		for (i = 0; i < a_height; i++) 
		{
		    for (j = 0; j < a_width; j++) 
			{ 
				if(bia_getpixel(j,i,bitbuffer,&bia))
				{
		        	pixel = SDL_MapRGB(surface->format, 0, 0, 255);
		        	addr = surface->pixels + (i * surface->pitch + j * sizeof(pixel));
		        	memcpy(addr, &pixel, sizeof(pixel));
				}
		    }
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

void usage(char* pname)
{
	fprintf(stderr, "\n--------------------------------------\n");
	fprintf(stderr, "\nusage:\n  %s gifurl arrangemode MSB rotate flip width height \n", pname);

	fprintf(stderr, "\n\tgifurl: gif file url\n");

	fprintf(stderr, "\n\tarrangemode:\n");
		
	fprintf(stderr, "\t\t1:\n");
	fprintf(stderr, "\t\t\tByte1:|bit7...bit0|\tByte2:|bit7...bit0|\tByten:|bit7...bit0|\n");
	fprintf(stderr, "\t\t\tByten+1:|bit7...bit0|\tByten+2:|bit7...bit0|\tByten+n:|bit7...bit0|\n");
	
	fprintf(stderr, "\t\t2:\n");
	fprintf(stderr, "\t\t\tByte1\tByten+1\n");
	fprintf(stderr, "\t\t\t ___ \t ___ \n");
	fprintf(stderr, "\t\t\tbit7 \tbit7 \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\tbit0 \tbit0 \n");
	fprintf(stderr, "\t\t\t ___ \t ___ \n");
	
	fprintf(stderr, "\t\t\tByte2\tByten+2\n");
	fprintf(stderr, "\t\t\t ___ \t ___ \n");
	fprintf(stderr, "\t\t\tbit7 \tbit7 \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\tbit0 \tbit0 \n");
	fprintf(stderr, "\t\t\t ___ \t ___ \n");
	
	fprintf(stderr, "\t\t\tByten\tByten+n\n");
	fprintf(stderr, "\t\t\t ___ \t ___ \n");
	fprintf(stderr, "\t\t\tbit7 \tbit7 \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\t  .  	.  \n");
	fprintf(stderr, "\t\t\tbit0 \tbit0 \n");
	fprintf(stderr, "\t\t\t ___ \t ___ \n");
	
	fprintf(stderr, "\t\t3:\n");
	fprintf(stderr, "\t\t\tByte1:|bit0...bit7|\tByten+1:|bit0...bit7|\n");
	fprintf(stderr, "\t\t\tByte2:|bit0...bit7|\tByten+2:|bit0...bit7|\n");
	fprintf(stderr, "\t\t\tByten:|bit0...bit7|\tByten+n:|bit0...bit7|\n");

	fprintf(stderr, "\t\t4:\n");
	fprintf(stderr, "\t\t\tByte1\tByte2\tByten\n");
	fprintf(stderr, "\t\t\t ___ \t ___ \t ___ \n");
	fprintf(stderr, "\t\t\tbit7 \tbit7 \tbit7 \n");
	fprintf(stderr, "\t\t\t  .  \t  .  \t  .  \n");
	fprintf(stderr, "\t\t\t  .  \t  .  \t  .  \n");
	fprintf(stderr, "\t\t\t  .  \t  .  \t  .  \n");
	fprintf(stderr, "\t\t\tbit0 \tbit0 \tbit0 \n");
	fprintf(stderr, "\t\t\t ___ \t ___ \t ___ \n");
	
	fprintf(stderr, "\t\t\tByten\tByten+1\tByten+n\n");
	fprintf(stderr, "\t\t\t ___ \t ___ \t ___ \n");
	fprintf(stderr, "\t\t\tbit7 \tbit7 \tbit7 \n");
	fprintf(stderr, "\t\t\t  .  \t  .  \t  .  \n");
	fprintf(stderr, "\t\t\t  .  \t  .  \t  .  \n");
	fprintf(stderr, "\t\t\t  .  \t  .  \t  .  \n");
	fprintf(stderr, "\t\t\tbit0 \tbit0 \tbit0 \n");
	fprintf(stderr, "\t\t\t ___ \t ___ \t ___ \n");

	fprintf(stderr, "\n\tMSB: MSB first if 1\n");
	fprintf(stderr, "\n\trotate: rotate 0, 90, 180 or 270 degrees clockwise\n");
	fprintf(stderr, "\n\tflip: flip left and right if 1\n");
	fprintf(stderr, "\n--------------------------------------\n");

	
	fprintf(stderr, "\n\nkey function:\n\n");
	fprintf(stderr, "\tF1:zoom out\n");
	fprintf(stderr, "\tF2:zoom in\n");
	fprintf(stderr, "\tF3:gray down\n");
	fprintf(stderr, "\tF4:gray up\n");
	fprintf(stderr, "\tF5:creat blank frame\n");
	fprintf(stderr, "\tF6:creat full frame\n");
	fprintf(stderr, "\tF12:creat the real size image on the top left corner of the screen\n");
	fprintf(stderr, "\tUP,DOWN,LEFT,RIGHT:move the camera\n");
	fprintf(stderr, "\td:delete current frame\n");
	fprintf(stderr, "\tSPACE:save current frame\n");
	fprintf(stderr, "\tq:quit\n\n");
	fprintf(stderr, "\n--------------------------------------\n");

	//fprintf(stderr, BIN_INCLUDE_FILE,"mode",0,1,2,3,4,5); 

}

int
main(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Event event;
    gd_GIF *gif;
    char title[32] = {0};
    Uint8 *color, *frame;
    int i, j;
    int ret, paused, quit;
	uint8_t startfg = 0;
	FILE *stream; 
    if (argc != 8) 
	{
        usage(argv[0]);
        return 1;
    }

	mode = atoi(argv[2]);
	if(mode > 4 || mode < 1)
	{
		fprintf(stderr, "arrangemode must to be [1,4]\n");
        return 1;
	}
	mode--;

	msbfg = atoi(argv[3]);
	msbfg = msbfg?1:0;

	rotate = atoi(argv[4]);
	if(0!=rotate&&90!=rotate&&180!=rotate&&270!=rotate)
	{
		fprintf(stderr, "rotate must to be 0, 90, 180 or 270\n");
        return 1;
	}

	flip = atoi(argv[5]);
	flip = flip?1:0;

	a_width = atoi(argv[6]);
	a_height = atoi(argv[7]);
	if(a_width < 0 || a_height < 0)
	{
		fprintf(stderr, "width or height error!\n");
        return 1;
	}
	
    gif = gd_open_gif(argv[1]);
    if (!gif) 
	{
        fprintf(stderr, "Could not open %s\n", argv[1]);
        return 1;
    }

	if(rotate == 0 || rotate == 180)
	{
		s_width = gif->width;
		s_height = gif->height;
		
	}
	else
	{
		s_width = gif->height;
		s_height = gif->width;
	}

	if(a_width > s_width || a_height > s_height)
	{
		fprintf(stderr, "width[%d] or height[%d] error.gif file[%dx%d] rotate[%d]\n",a_width,a_height,gif->width,gif->height,rotate);
    	return 1;
	}
	
	
    frame = malloc(gif->width * gif->height * 3);
    if (!frame) 
	{
        fprintf(stderr, "Could not allocate frame\n");
        return 1;
    }
	bitbuffer = malloc(a_width*a_height/8);
	if(!bitbuffer)
	{
		fprintf(stderr, "Could not allocate buffer\n");
        return 1;
	}
	memset(bitbuffer,0,a_width*a_height/8);
	
	stream = fopen("./binimage.bin","w+");
	if(!stream)
	{
		fprintf(stderr, "Could not open binimage.bin\n");
        return 1;
	}
	fwrite(&bia,sizeof(bia_t),1,stream);
	
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) 
	{
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    if (SDL_CreateWindowAndRenderer(s_width, s_height, SDL_WINDOW_RESIZABLE, &window, &renderer)) 
	{
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return 1;
    }
    snprintf(title, sizeof(title) - 1, "GIF %dx%d",
             s_width, s_height);
    SDL_SetWindowTitle(window, title);
    color = &gif->gct.colors[gif->bgindex * 3];
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    surface = SDL_CreateRGBSurface(0, s_width, s_height, 32, 0, 0, 0, 0);
    if (!surface) 
	{
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        return 1;
    }

	fprintf(stdout, "gifurl[%s][%dx%d] arrangemode[%d] MSB[%d] rotate[%d] flip[%d] width[%d] height[%d]\n",
		argv[1],gif->width, gif->height,
		mode,
		msbfg,
		rotate,
		flip,
		a_width,
		a_height);

	rect.h = a_height;
	rect.w = a_width;
	rect.x = 0;
	rect.y = 0;

	bia.id = BIA_HEAD_ID;
	bia.arrange_mode = mode;
	bia.msb = msbfg;
	bia.width = a_width;
	bia.height = a_height;
	bia.number = 0;
	bia.delay = gif->gce.delay;
	bia.size = 0;
    paused = 0;
    quit = 0;
    while (1) 
	{
		while(!SDL_PollEvent(&event))
		{
			SDL_Delay(10);
		}

        if (event.type == SDL_QUIT)
            quit = 1;
		
        if (event.type == SDL_KEYDOWN) 
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_q:
					quit = 1;
					break;
				case SDLK_UP:
					if(rect.y > RECT_STEP_SIZE)
						rect.y -= RECT_STEP_SIZE;
					else 
						rect.y = 0;
					break;
				case SDLK_DOWN:
					rect.y += RECT_STEP_SIZE;
					if((rect.y+rect.h) >= s_height)
						rect.y = s_height - rect.h;
					break;
				case SDLK_RIGHT:
					rect.x += RECT_STEP_SIZE;
					if((rect.x+rect.w) >= s_width)
						rect.x = s_width - rect.w;
					break;
				case SDLK_LEFT:
					if(rect.x > RECT_STEP_SIZE)
						rect.x -= RECT_STEP_SIZE;
					else 
						rect.x = 0;
					break;
				case SDLK_SPACE:
					
					if(startfg)
					{
						fwrite(bitbuffer,a_width*a_height/8,1,stream);
						bia.number++;
					}
					
				case SDLK_d:
					if (gd_get_frame(gif) == -1)
					{
	    				quit = 1;
					}
					startfg = 1;
					break;
				case SDLK_F2:
					recttime += 0.1;
					if((a_width*recttime)>s_width || (a_height*recttime)>s_height )
					{
						recttime -= 0.1;
						break;
					}

					rect.w = a_width*recttime;
					rect.h = a_height*recttime;
					if((rect.x+rect.w)>s_width)
						rect.x = s_width - rect.w;
					if((rect.y+rect.h)>s_height)
						rect.y = s_height - rect.h;

					TITLE_SHOW:			

					snprintf(title, sizeof(title) - 1, "GIF %dx%d times %.1lf",s_width, s_height,recttime);
					SDL_SetWindowTitle(window, title);
					
					break;
				case SDLK_F1:
					if(recttime > 1)
					{
						recttime -= 0.1;
					}

					rect.w = a_width*recttime;
					rect.h = a_height*recttime;

					goto TITLE_SHOW;
					
				case SDLK_F3:

					if(gray >= 8)
						gray -= 8;
					
					break;
				case SDLK_F4:
					if(gray <= 247)
						gray += 8;
					break;
				case SDLK_F5:
					memset(bitbuffer,0,a_width*a_height/8);
					fwrite(bitbuffer,a_width*a_height/8,1,stream);
					bia.number++;
					continue;
				case SDLK_F6:
					memset(bitbuffer,0xff,a_width*a_height/8);
					fwrite(bitbuffer,a_width*a_height/8,1,stream);
					bia.number++;
					continue;
				case SDLK_F12:
					showlittle = showlittle?0:1;
					break;
				
			}
        }
       
        if (quit) 
			break;
		memset(bitbuffer,0,a_width*a_height/8);
		drawframe2Surface(gif,surface,frame);

		texture = RenderDrawStart(renderer,surface);

		SDL_RenderDrawRect(renderer, &rect);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		RenderStopandShow(renderer,texture);
    }

	fwrite(bitbuffer,a_width*a_height/8,1,stream);
	bia.number++;
	bia.size = bia.number*a_width*a_height/8;

	rewind(stream);
	fwrite(&bia,sizeof(bia_t),1,stream);
	fclose(stream);
	uint8_t cfile[2048];
	sprintf( (char *)cfile, BIN_INCLUDE_FILE,bia.arrange_mode,bia.msb,bia.width,bia.height,bia.number,bia.delay,bia.size);

	stream = fopen("./binimage.c","w+");
	if(!stream)
	{
		fprintf(stderr, "Could not open binimage.c\n");
	}
	else
	{
		fwrite(cfile,strlen((char *)cfile),1,stream);
		fclose(stream);
	}

	
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(frame);
    gd_close_gif(gif);
    return 0;
}




