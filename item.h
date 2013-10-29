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

#ifndef ITEM_H
#define ITEM_H

#include <SDL.h>
#include "lbx.h"

typedef struct {
        SDL_Rect rect;
        LBXAnimation_t * anim;
	int current_frame;
	int anim_start;
	int anim_end;
        int frame_normal; // if -1 it's an animation
        int frame_over;
        int frame_click;
	int clicked;
        void (*click_left)(void * arg); //callback on left click on this item
        void * click_left_arg;
        void (*click_right)(void * arg); //callback on right click on this item
        void * click_right_arg;
        void (*over)(void * arg); //callback on mouse over this item
        void * over_arg;
	LBXAnimation_t * font;
	const char * string;		// string centered on item
} item_t;

void item_init(item_t * item);
void item_set_frame(item_t * item, int x, int y,LBXAnimation_t * frame);
void item_set_anim(item_t * item, int x, int y,LBXAnimation_t * frame);
void item_set_frame_normal(item_t * item, int num_frame);
void item_set_frame_over(item_t * item, int num_frame);
void item_set_frame_click(item_t * item, int num_frame);
void item_set_click_left(item_t * item,void (*click_left)(void * arg),void * click_left_arg);
void item_set_click_right(item_t * item,void (*click_right)(void * arg),void * click_right_arg);
void item_set_over(item_t * item,void (*over)(void * arg),void * over_arg);
void item_set_font(item_t * item,LBXAnimation_t * font);
void item_set_string(item_t * item,const char * string);
void item_set_geometry(item_t * item,int x, int y, int w, int h);
void item_set_anim_start(item_t * item, int start_frame);
void item_set_anim_end(item_t * item, int end_frame);

#endif
