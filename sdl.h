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

#include <SDL.h>
#include "item.h"

#define SDL_OPAQUE 0xff
#define SDL_TRANSPARENT 0x00

#define RMASK 0x00ff0000
#define GMASK 0x0000ff00
#define BMASK 0x000000ff
#define AMASK 0xff000000

#define DEFAULT_SCREEN_W 640
#define DEFAULT_SCREEN_H 480
#define DEFAULT_ASPECT_RATIO (4.0/3.0)

//#define PAL_TO_RGB(x) x.r<<2,x.g<<2,x.b<<2,SDL_OPAQUE
#define PAL_TO_RGB(x) x.r,x.g,x.b,SDL_OPAQUE

void sdl_init(const char * data_path,LBXFontTemplate_t ** font_template);
void sdl_cleanup(void);
void sdl_set_pixel(SDL_Surface *surface, int x, int y, Uint32 R, Uint32 G, Uint32 B, Uint32 A);
void sdl_mouse_manager(SDL_Event * event, item_t * item, int item_num);
void sdl_screen_manager(SDL_Event * event);
int sdl_get_win_w();
int sdl_get_win_h();
void sdl_loop_manager();
void sdl_print(LBXAnimation_t * anim,int x, int y, const char * string);
void sdl_print_center(LBXAnimation_t * anim,int x, int y, const char * string);
void sdl_print_center_x(LBXAnimation_t * anim,int x, int y, const char * string);
void sdl_print_item(item_t * item,LBXAnimation_t * font,const char * string);
void sdl_keyboard_init(char * string, void (*cb)(void*arg));
char * sdl_keyboard_get_buf();
void sdl_keyboard_manager(SDL_Event * event);

