/*
   Free MoM is a free implementation of classical game Mater of Magic.
   Copyright (C) 2013 bobz38@free.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <SDL.h>
#include <stdlib.h>
#include "../lbx.h"
#include "../sdl.h"
#include "../opengl.h"
#include "../game.h"

extern int window_w;
extern int window_h;

int display_font(const char * file_name)
{
        SDL_Event event;
	int fEnd = 0;
	LBXAnimation_t * anim;
	LBXFontTemplate_t * template;
	int current_anim = 0;
	SDL_Rect rect;
	int clear_screen = 0;
	LBXGfxPaletteEntry_t * pal;
	int i;

	sdl_init(NULL,NULL);

//	template = lbx_decode_font(file_name,LBX_FONT_BIG_GOTH);
	template = lbx_decode_font(file_name,LBX_FONT_SMALL);

	printf("Number of color: %d\n",template->num_col);
	pal = malloc((template->num_col+1)*sizeof(LBXGfxPaletteEntry_t));
	for(i=0;i<template->num_col;i++) {
		pal[i].r=pal[i].g=pal[i].b=(i+1)*(0xff/template->num_col);
	}
	pal[template->num_col].r=0xff;
	pal[template->num_col].g=0x00;
	pal[template->num_col].b=0x00;
	anim = lbx_generate_font(template,pal,1);

	while ( ! fEnd )
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_c:
							clear_screen = !clear_screen;
							break;
						case SDLK_ESCAPE:
							fEnd = 1;
							break;
						case SDLK_SPACE:
							current_anim++;
							if(anim[current_anim].frame == NULL) {
								exit(EXIT_SUCCESS);

							}
							printf("File %d (%c)\n",current_anim,(current_anim%96)+32);
							opengl_clear_fbo();
							break;


						default:
							break;
					}
					break;
				default:
					;
			}
			sdl_screen_manager(&event);
		}

		rect.x=0;	
		rect.y=0;	
		rect.w=anim[current_anim].w*10;	
		rect.h=anim[current_anim].h*10;	

		if(clear_screen) {
			opengl_clear_fbo();
		}
		opengl_blit_anim(&anim[current_anim],&rect,0,-1);
		opengl_blit_to_screen();
		SDL_Delay(ANIM_DELAY);
	}

	return 0;

}

int display_array(const char * file_name)
{
	int i = 0;
	int j = 0;
	char ** array = NULL;

	printf("Data array file\n");
	while( (array=lbx_read_array(file_name,i)) != NULL ) {
		j=0;
		while( array[j] != NULL) {
			printf("%d: %s\n",j,array[j]);
			j++;
		}
		i++;
	}

	return 0;
}

int display_image(const char * file_name)
{
        SDL_Event event;
	int fEnd = 0;
	LBXAnimation_t * anim;
	int current_anim = 0;
	SDL_Rect rect;
	int clear_screen = 0;

	sdl_init(NULL,NULL);

	anim = lbx_decode_image(file_name);
	if(  anim == NULL) {
		return -1;
	}

	while ( ! fEnd )
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_c:
							clear_screen = !clear_screen;
							break;
						case SDLK_ESCAPE:
							fEnd = 1;
							break;
						case SDLK_SPACE:
							current_anim++;
							if(anim[current_anim].frame == NULL) {
								exit(EXIT_SUCCESS);
							}
							printf("File %d\n",current_anim);
							opengl_clear_fbo();
							break;
						default:
							break;
					}
					break;
				default:
					;
			}
			sdl_screen_manager(&event);
		}

		rect.x=0;	
		rect.y=0;	
		rect.w=anim[current_anim].w;	
		rect.h=anim[current_anim].h;	

		if(clear_screen) {
			opengl_clear_fbo();
		}
		opengl_blit_anim(&anim[current_anim],&rect,0,-1);
		opengl_blit_to_screen();
//		SDL_Delay(ANIM_DELAY);
	}

	return 0;
}

int display_terrain(const char * file_name)
{
        SDL_Event event;
	int fEnd = 0;
	LBXAnimation_t * anim;
	int current_anim = 0;
	SDL_Rect rect;
	int clear_screen = 0;
	int j;

	sdl_init(NULL,NULL);

	anim = lbx_decode_terrain(file_name);
	if(  anim == NULL) {
		return -1;
	}

	while ( ! fEnd )
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_c:
							clear_screen = !clear_screen;
							break;
						case SDLK_ESCAPE:
							fEnd = 1;
							break;
						case SDLK_SPACE:
							current_anim++;
							if(anim[current_anim].frame == NULL) {
								exit(EXIT_SUCCESS);

							}
							printf("File %d (%02x)\n",current_anim,current_anim);
							opengl_clear_fbo();
							break;

						case SDLK_RIGHT:
							j=0;
							while(anim[current_anim].frame != NULL && j<11 ) {
								current_anim++;
								j++;
							}
							current_anim--;
							printf("File %d (%02x)\n",current_anim,current_anim);
							opengl_clear_fbo();
							break;

						case SDLK_LEFT:
							current_anim-=10;
							if(current_anim<0){
								current_anim=0;
							}
							printf("File %d (%02x)\n",current_anim,current_anim);
							opengl_clear_fbo();
							break;

						case SDLK_UP:
							j=0;
							while(anim[current_anim].frame != NULL && j<2 ) {
								current_anim++;
								j++;
							}
							current_anim--;
							printf("File %d (%02x)\n",current_anim,current_anim);
							opengl_clear_fbo();
							break;

						case SDLK_DOWN:
							current_anim-=1;
							if(current_anim<0){
								current_anim=0;
							}
							printf("File %d (%02x)\n",current_anim,current_anim);
							opengl_clear_fbo();
							break;

						default:
							break;
					}
					break;
				default:
					;
			}
			sdl_screen_manager(&event);
		}

		rect.x=0;	
		rect.y=0;	
		rect.w=anim[current_anim].w;	
		rect.h=anim[current_anim].h;	

		if(clear_screen) {
			opengl_clear_fbo();
		}
		opengl_blit_anim(&anim[current_anim],&rect,0,-1);
		opengl_blit_to_screen();
	}

	return 0;
}

int play_sound(const char * file_name)
{
	Mix_Chunk * chunk = NULL;
	Mix_Music * music = NULL;
	int fEnd = 0;
        SDL_Event event;
	int file_num = 0;
	int channel_num = 0;

	sdl_init(NULL,NULL);

	chunk = lbx_decode_sound(file_name,file_num);
	if(chunk != NULL ) {
		Mix_PlayChannel(channel_num, chunk, -1);
	}
	else {
		music = lbx_decode_music(file_name,file_num);
		if( music == NULL ) {
			return -1;
		}
		Mix_PlayMusic(music,-1);
	}

	while ( ! fEnd )
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							fEnd = 1;
							break;
						case SDLK_SPACE:
							if(chunk) {
								Mix_HaltChannel(channel_num);
							}
							if(music) {
								Mix_HaltMusic();
							}
							
							file_num++;

							chunk = lbx_decode_sound(file_name,file_num);
							if(chunk != NULL ) {
								Mix_PlayChannel(channel_num, chunk, -1);
							}
							else {
								music = lbx_decode_music(file_name,file_num);
								if( music == NULL ) {
									printf("Cannot find music or sound resource\n");
									fEnd = 1;
									break;
								}
								Mix_PlayMusic(music,-1);
							}

							printf("Playing resource %d\n",file_num);
							break;


						default:
							break;
					}
					break;
				default:
					;
			}
		}

		SDL_Delay(100);
	}

	return 0;
}

int main(int argc, char **argv)
{
	int type;
	char * data;

	if(argc != 2 || argv[1] == NULL) {
		printf("Give a file name\n");
		return 0;
	}

	data = lbx_read(argv[1]);
	type = lbx_guess_type(data);

	switch (type) {
		case LBX_T_FONT:
			display_font(argv[1]);
			exit(EXIT_SUCCESS);
		break;

		case LBX_T_SOUND:
			play_sound(argv[1]);
			exit(EXIT_SUCCESS);
		break;

		case LBX_T_ARRAY:
			display_array(argv[1]);
			exit(EXIT_SUCCESS);
		break;

		case LBX_T_TERRAIN:
			display_terrain(argv[1]);
			exit(EXIT_SUCCESS);
		break;

		default:
			if( display_image(argv[1]) == -1) {
				printf("Raw data file ?\n");
				exit(EXIT_SUCCESS);
			}
		break;
	}

	return 0;
}
