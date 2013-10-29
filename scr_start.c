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
#include "item.h"
#include "screen.h"

/* Order in file */
#define ANI_UP 0
#define ANI_CONT 1
#define ANI_LOAD 2
#define ANI_NEW 3
#define ANI_QUIT 4
#define ANI_DOWN 5
#define ANI_NUM 6

/* Order to display */
#define ITM_UP 0
#define ITM_DOWN 1
#define ITM_CONT 2
#define ITM_LOAD 3
#define ITM_NEW 4
#define ITM_QUIT 5
#define ITM_NUM 6

static LBXAnimation_t * anim;
static item_t item[ITM_NUM];
static int ret;

void cb_new(void * noarg)
{
	ret = SCREEN_NEW;
}

void cb_quit(void * noarg)
{
	ret = SCREEN_QUIT;
}

int screen_start()
{
	SDL_Event event;
	int j;

	ret = SCREEN_NUM;

	if(anim==NULL){
		anim = load_graphics("MAINSCRN.LBX");
		if(anim == NULL) {
			exit(EXIT_FAILURE);
		}
	}
	else {
		/* Start over the anim */
		lbx_reset_anim(item[ITM_UP].anim);
	}

	j=0;
	item_init(&item[j]);
        item_set_anim(&item[j],0,0,&anim[ANI_UP]);
        j++;
	item_init(&item[j]);
        item_set_frame(&item[j],0,item[ITM_UP].rect.h,&anim[ANI_DOWN]);
        j++;
	item_init(&item[j]);
        item_set_frame(&item[j],ORIGINAL_GAME_WIDTH/2-anim[ANI_CONT].w/2,
                                ORIGINAL_GAME_HEIGHT-16*4,&anim[ANI_CONT]);
	item_set_frame_over(&item[j],1);
	item_set_frame_click(&item[j],1);
        j++;
	item_init(&item[j]);
        item_set_frame(&item[j],ORIGINAL_GAME_WIDTH/2-anim[ANI_LOAD].w/2,
                                ORIGINAL_GAME_HEIGHT-16*3,&anim[ANI_LOAD]);
	item_set_frame_over(&item[j],1);
	item_set_frame_click(&item[j],1);
	j++;
	item_init(&item[j]);
        item_set_frame(&item[j],ORIGINAL_GAME_WIDTH/2-anim[ANI_NEW].w/2,
                                ORIGINAL_GAME_HEIGHT-16*2,&anim[ANI_NEW]);
	item_set_frame_over(&item[j],1);
	item_set_frame_click(&item[j],1);
	item_set_click_left(&item[j],cb_new,NULL);
	j++;
	item_init(&item[j]);
        item_set_frame(&item[j],ORIGINAL_GAME_WIDTH/2-anim[ANI_QUIT].w/2,
                                ORIGINAL_GAME_HEIGHT-16*1,&anim[ANI_QUIT]);
	item_set_frame_over(&item[j],1);
	item_set_frame_click(&item[j],1);
	item_set_click_left(&item[j],cb_quit,NULL);

	opengl_clear_fbo();

	while( ret == SCREEN_NUM )
	{
		while (SDL_PollEvent(&event))
		{
			if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) {
				return SCREEN_QUIT;
			}
			sdl_screen_manager(&event);
			sdl_mouse_manager(&event,item,ITM_NUM);
		}

		opengl_blit_item_list(item,ITM_NUM);

		opengl_blit_to_screen();

		sdl_loop_manager();
	}

	return ret;
}
