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

#include <SDL_opengl.h>
#include "lbx.h"
#include "item.h"

void opengl_init(int w, int h);
void opengl_cleanup();
void opengl_clear_screen();
void opengl_clear_fbo();
GLuint opengl_create_texture(SDL_Surface * surf);
void opengl_blit_frame(LBXAnimation_t * anim, SDL_Rect * rect, int frame_num);
int opengl_blit_anim(LBXAnimation_t * anim, SDL_Rect * rect,int start,int end);
int opengl_blit_item(item_t * item);
void opengl_screen_coord(SDL_Rect * rect);
void opengl_blit_to_screen();
void opengl_blit_item_list(item_t * list, int num);
