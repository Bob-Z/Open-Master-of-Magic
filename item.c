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

#include "item.h"

void item_init(item_t * item)
{
	item->rect.x=-1;
	item->rect.y=-1;
	item->rect.w=0;
	item->rect.h=0;
	item->anim=NULL;
	item->anim_start=0;
	item->anim_end=-1;
	item->current_frame=0;
	item->frame_normal=0;
	item->frame_over=0;
	item->frame_click=0;
	item->clicked=0;
	item->click_left=NULL;
	item->click_left_arg=NULL;
	item->click_right=NULL;
	item->click_right_arg=NULL;
	item->over=NULL;
	item->over_arg=NULL;
	item->font=NULL;
	item->string=NULL;
}

void item_set_frame(item_t * item, int x, int y,LBXAnimation_t * frame)
{
	item->rect.x = x;
	item->rect.y = y;
	if( frame ) {
		item->rect.w = frame->w;
		item->rect.h = frame->h;
		item->anim = frame;
	}
}
void item_set_anim(item_t * item, int x, int y,LBXAnimation_t * frame)
{
	item_set_frame(item,x,y,frame);
	item->frame_normal = -1;
}
void item_set_frame_normal(item_t * item, int num_frame)
{
	item->frame_normal = num_frame;
}

void item_set_anim_start(item_t * item, int start_frame)
{
	item->anim_start = start_frame;
}

void item_set_anim_end(item_t * item, int end_frame)
{
	item->anim_end = end_frame;
}

void item_set_frame_over(item_t * item, int num_frame)
{
	item->frame_over = num_frame;
}

void item_set_frame_click(item_t * item, int num_frame)
{
	item->frame_click = num_frame;
}

void item_set_click_left(item_t * item,void (*click_left)(void * arg),void * click_left_arg)
{
	item->click_left=click_left;
	item->click_left_arg=click_left_arg;
}

void item_set_click_right(item_t * item,void (*click_right)(void * arg),void * click_right_arg)
{
	item->click_right=click_right;
	item->click_right_arg=click_right_arg;
}

void item_set_over(item_t * item,void (*over)(void * arg),void * over_arg)
{
	item->over=over;
	item->over_arg=over_arg;
}

void item_set_font(item_t * item,LBXAnimation_t * font)
{
	item->font = font;
}
void item_set_string(item_t * item,const char * string)
{
	item->string = string;
}
void item_set_geometry(item_t * item,int x, int y, int w, int h)
{
	item->rect.x=x;
	item->rect.y=y;
	item->rect.w=w;
	item->rect.h=h;
}
