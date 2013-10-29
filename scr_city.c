/*
   Open MoM is a free implementation of classical game Mater of Magic.
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

#include <stdlib.h>
#include <stdio.h>
#include "lbx.h"
#include <SDL.h>
#include "sdl.h"
#include "opengl.h"
#include "game.h"
#include "item.h"
#include "screen.h"
#include "exe.h"
#include "terrain.h"

/* Order in file */
#define ANI_BG 3

#define ITM_NUM 1

static LBXAnimation_t * anim=NULL;

static item_t item[ITM_NUM];

static int end_screen;

void screen_city(city_t * city)
{
	SDL_Event event;
        int j;
	//game_t * game = city->game;

	end_screen = -1;

	sdl_keyboard_init(NULL,NULL);

	/* Load resource */
	if(anim==NULL) {
		anim = load_graphics("BACKGRND.LBX");
		if(anim == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	j=0;
	item_init(&item[j]);
	item_set_frame(&item[j],0,0,&anim[ANI_BG]);
	j++;

	/* Main loop */
	while( end_screen == -1)
	{
		while (SDL_PollEvent(&event))
		{
			if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) {
				return;
			}
			sdl_screen_manager(&event);
			sdl_keyboard_manager(&event);
		}

		opengl_blit_item_list(item,ITM_NUM);

		opengl_blit_to_screen();

		sdl_loop_manager();
	}

	return;
}
