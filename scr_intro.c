/*
   Open MoM is a free implementation of classical game Mater of Magic.
   Copyright (C) 2013 carabobz@gmail.com

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

static LBXAnimation_t * anim = NULL;

void screen_intro()
{
	int end_anim;
	int current_anim = 0;
	SDL_Event event;
	SDL_Rect rect;

	if(anim==NULL) {
		anim = load_graphics("INTRO.LBX");
		if(anim == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	rect.x = 0;
	rect.y = 0;
	rect.w = anim[0].w;
	rect.h = anim[0].h;

	opengl_clear_fbo();

	while( anim[current_anim].num_frame != 0 ) {
		while (SDL_PollEvent(&event)) {
			if(event.type==SDL_KEYDOWN && (event.key.keysym.sym==SDLK_ESCAPE || event.key.keysym.sym==SDLK_SPACE)) {
				return;
			}
			sdl_screen_manager(&event);
		}

		end_anim = opengl_blit_anim(&anim[current_anim],&rect,0,-1);

		opengl_blit_to_screen();

		sdl_loop_manager();

		if(end_anim) {
			current_anim++;
			printf("current_anim = %d\n",current_anim);
			opengl_clear_fbo();
		}
	}

	return;
}
