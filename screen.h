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

#include "game.h"

enum screen { SCREEN_QUIT,SCREEN_NEW,SCREEN_LOAD,SCREEN_CONTINUE,SCREEN_NUM};

void screen_intro(SDL_Renderer * render);
int screen_start(SDL_Renderer * render);
int screen_newgame(SDL_Renderer * render,game_t * new_game);
void screen_play(SDL_Renderer * render,game_t * game);
void screen_city(SDL_Renderer * render,city_t * city);
void screen_combat(SDL_Renderer * render,game_t * game,unit_list_t ** attack, unit_list_t ** defense, int x, int y, int side);
