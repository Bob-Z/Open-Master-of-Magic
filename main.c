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
#include <libconfig.h>
#include <SDL.h>
#include <time.h>
#include "sdl.h"
#include "screen.h"
#include "game.h"

config_t config;
game_t game;
LBXFontTemplate_t * font_template[LBX_FONT_NUM];
const char * data_path;

int main(int argc, char ** argv)
{
	char config_path[2048];
	int screen;
	int ret;

	srand( (unsigned)time(NULL) );

	config_init(&config);

	sprintf(config_path,"%s/.config/omom/omom.cfg",getenv("HOME"));
	config_read_file(&config,config_path);
	config_lookup_string(&config,"data_path", &data_path);

	sdl_init(data_path,font_template);

	screen_intro();
	while( (screen=screen_start()) != SCREEN_QUIT) {
		switch(screen) {
			case SCREEN_NEW:
				ret = screen_newgame(&game);
				if(ret == 1) {
					screen_play(&game);
				}
			break;
			case SCREEN_LOAD:
			break;
			case SCREEN_CONTINUE:
			break;
			default:
			break;
		}
	}

	return 0;
}
