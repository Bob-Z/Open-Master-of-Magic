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

#include "sdl.h"
#include "opengl.h"
#include "item.h"
#include "game.h"

int window_w = DEFAULT_SCREEN_W;
int window_h = DEFAULT_SCREEN_H;
int windowed_w = DEFAULT_SCREEN_W;
int windowed_h = DEFAULT_SCREEN_H;
int fullscreen_w = 0;
int fullscreen_h = 0;
int fullscreen = 0;

int mouse_x = 0;
int mouse_y = 0;

const SDL_VideoInfo* video_info;

static char keyboard_buf[2048];
static unsigned int keyboard_index = 0;
static void (*keyboard_cb)(void * arg) = NULL;

//You must SDL_LockSurface(surface); then SDL_UnlockSurface(surface); before calling this function
void sdl_set_pixel(SDL_Surface *surface, int x, int y, Uint32 R, Uint32 G, Uint32 B, Uint32 A)
{
	Uint32 color = (A << 24) + (R << 16) + (G << 8) + (B);
	Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * sizeof(color);
	*(Uint32 *)target_pixel = color;
}

void sdl_init(const char * data_path,LBXFontTemplate_t ** font_template)
{
	int i;
	char path[2048];
	char lower_case[2048];
	const char filename[] = LBX_FONT_FILE_NAME;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
		printf("Echec d'initialisation de SDL.\n");
		exit(EXIT_FAILURE);
	}

	if(Mix_OpenAudio(11025, MIX_DEFAULT_FORMAT, 2, 512)==-1) {
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}

	video_info = SDL_GetVideoInfo();
	fullscreen_w = video_info->current_w;
	fullscreen_h = video_info->current_h;

	if(fullscreen) {
		window_w=fullscreen_w;
		window_h=fullscreen_h;
	}

	SDL_SetVideoMode(window_w, window_h, 32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE | fullscreen);

	opengl_init(window_w,window_h);

	atexit(sdl_cleanup);

	if( font_template!= NULL ) {
		i=0;
		while( filename[i] != 0 ) {
			lower_case[i] = tolower(filename[i]);
			i++;
		}

		font_template[LBX_FONT_NUM-1] = NULL;

		if(data_path != NULL ) {
			sprintf(path,"%s/%s",data_path,filename);
			for(i=0; i<LBX_FONT_NUM; i++) {
				font_template[i] = lbx_decode_font(path,i);
				if(font_template[i]==NULL) {
					break;
				}
			}
			if(font_template[LBX_FONT_NUM-1] != NULL) {
				return;
			}
			/* Try lower case */
			sprintf(path,"%s/%s",data_path,lower_case);
			for(i=0; i<LBX_FONT_NUM; i++) {
				font_template[i] = lbx_decode_font(path,i);
				if(font_template[i]==NULL) {
					break;
				}
			}
			if(font_template[LBX_FONT_NUM-1] != NULL) {
				return;
			}
		}

		/* Try local data */
		sprintf(path,"./%s",filename);
		for(i=0; i<LBX_FONT_NUM; i++) {
			font_template[i] = lbx_decode_font(path,i);
			if(font_template[i]==NULL) {
				break;
			}
		}
		if(font_template[LBX_FONT_NUM-1] != NULL) {
			return;
		}

		/* and with lowercase */
		sprintf(path,"./%s",lower_case);
		for(i=0; i<LBX_FONT_NUM; i++) {
			font_template[i] = lbx_decode_font(path,i);
			if(font_template[i]==NULL) {
				break;
			}
		}
		if(font_template[LBX_FONT_NUM-1] != NULL) {
			return;
		}

		printf("Could not load fonts\n");
		exit(EXIT_FAILURE);
	}
}

void sdl_cleanup()
{
	opengl_cleanup();
	SDL_Quit();
}

void sdl_mouse_manager(SDL_Event * event, item_t * item, int item_num)
{
	SDL_Rect rect;
	int i;

	switch (event->type) {
	case SDL_MOUSEMOTION:
#if 0
		printf("Mouse moved by %d,%d to (%d,%d)\n",
			   event->motion.xrel, event->motion.yrel,
			   event->motion.x, event->motion.y);
#endif
		rect.x = event->motion.x;
		rect.y = event->motion.y;
		opengl_screen_coord(&rect);
		mouse_x = rect.x;
		mouse_y = rect.y;
//			printf("orig coord = %d,%d \n",rect.x,rect.y);
		for(i=0; i<item_num; i++) {
			item[i].current_frame = item[i].frame_normal;
			if( (item[i].rect.x < rect.x) &&
					((item[i].rect.x+item[i].rect.w) > rect.x) &&
					(item[i].rect.y < rect.y) &&
					((item[i].rect.y+item[i].rect.h) > rect.y) ) {
				item[i].current_frame = item[i].frame_over;
				if( item[i].over ) {
					item[i].over(item[i].over_arg);
				}
			}
			if(item[i].clicked) {
				item[i].current_frame = item[i].frame_click;
			}
		}

		break;
	case SDL_MOUSEBUTTONDOWN:
#if 0
		printf("Mouse button %d pressed at (%d,%d)\n",
			   event->button.button, event->button.x, event->button.y);
#endif
		rect.x = event->button.x;
		rect.y = event->button.y;
		opengl_screen_coord(&rect);
//			printf("orig coord = %d,%d \n",rect.x,rect.y);
		for(i=0; i<item_num; i++) {
			if( (item[i].rect.x < rect.x) &&
					((item[i].rect.x+item[i].rect.w) > rect.x) &&
					(item[i].rect.y < rect.y) &&
					((item[i].rect.y+item[i].rect.h) > rect.y) ) {
				if( item[i].click_left && event->button.button == SDL_BUTTON_LEFT) {
					item[i].current_frame=item[i].frame_click;
					item[i].clicked=1;
				}
				if( item[i].click_right && event->button.button == SDL_BUTTON_RIGHT) {
					item[i].current_frame=item[i].frame_click;
					item[i].clicked=1;
				}
			}
		}

		break;
	case SDL_MOUSEBUTTONUP:
		rect.x = event->button.x;
		rect.y = event->button.y;
		opengl_screen_coord(&rect);
		for(i=0; i<item_num; i++) {
			item[i].clicked=0;
			item[i].current_frame = item[i].frame_normal;
			if( (item[i].rect.x < rect.x) &&
					((item[i].rect.x+item[i].rect.w) > rect.x) &&
					(item[i].rect.y < rect.y) &&
					((item[i].rect.y+item[i].rect.h) > rect.y) ) {
				if( item[i].click_left && event->button.button == SDL_BUTTON_LEFT) {
					item[i].click_left(item[i].click_left_arg);
				}
				if( item[i].click_right && event->button.button == SDL_BUTTON_RIGHT) {
					item[i].click_right(item[i].click_right_arg);
				}
			}
		}

		break;
	}
}

/* Take care of system's windowing event */
void sdl_screen_manager(SDL_Event * event)
{
	Uint8 *keystate;

	switch (event->type) {
	case SDL_VIDEORESIZE:
		window_w = event->resize.w;
		window_h = event->resize.h;
		windowed_w = window_w;
		windowed_h = window_h;
		SDL_SetVideoMode(window_w, window_h, 32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE | fullscreen);

		break;
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLK_RETURN:
			keystate = SDL_GetKeyState(NULL);

			if( keystate[SDLK_RALT] || keystate[SDLK_LALT] ) {
				if(!fullscreen) {
					fullscreen = SDL_FULLSCREEN;
					window_w=fullscreen_w;
					window_h=fullscreen_h;
				} else {
					fullscreen = 0;
					window_w=windowed_w;
					window_h=windowed_h;
				}
				SDL_SetVideoMode(window_w, window_h, 32, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE | fullscreen);
				break;
			}
			break;
		default:
			break;
		}
		break;
	case SDL_QUIT:
		exit(EXIT_SUCCESS);
		break;
	default:
		break;
	}
}

int sdl_get_win_w()
{
	return window_w;
}

int sdl_get_win_h()
{
	return window_h;
}

void sdl_loop_manager()
{
	static Uint32 old_timer = 0;
	Uint32 timer;

	if( old_timer == 0 ) {
		old_timer = SDL_GetTicks();
	}

	timer = SDL_GetTicks();

	if( timer < old_timer + FRAME_DELAY ) {
		SDL_Delay(old_timer + FRAME_DELAY - timer);
	}
}

void sdl_print(LBXAnimation_t * anim,int x, int y, const char * string)
{
	size_t i;
	SDL_Rect rect;
	int index;

	rect.x=x;
	rect.y=y;

	for(i=0; i<strlen(string); i++) {
		if(string[i] < 32 || string[i] > 32+LBX_FONT_CHAR_NUM) {
			continue;
		}
		index = string[i]-32;
		rect.w = anim[index].w;
		rect.h = anim[index].h;
		opengl_blit_anim(&anim[index],&rect,0,-1);
		rect.x += anim[index].w;
	}
}

void sdl_print_center(LBXAnimation_t * anim,int x, int y, const char * string)
{
	size_t i;
	int index;
	int w=0;
	int h=0;

	for(i=0; i<strlen(string); i++) {
		if(string[i] < 32 || string[i] > 32+LBX_FONT_CHAR_NUM) {
			continue;
		}
		index = string[i]-32;
		w+=anim[index].w;
		if( anim[index].h > h ) {
			h = anim[index].h;
		}
	}

	sdl_print(anim,x-(w/2),y-(h/2),string);
}

void sdl_print_center_x(LBXAnimation_t * anim,int x, int y, const char * string)
{
	size_t i;
	int index;
	int w=0;

	for(i=0; i<strlen(string); i++) {
		if(string[i] < 32 || string[i] > 32+LBX_FONT_CHAR_NUM) {
			continue;
		}
		index = string[i]-32;
		w+=anim[index].w;
	}

	sdl_print(anim,x-(w/2),y,string);
}

void sdl_print_item(item_t * item,LBXAnimation_t * font,const char * string)
{
	int x;
	int y;

	x = item->rect.x + (item->rect.w/2);
	y = item->rect.y + (item->rect.h/2);

	sdl_print_center(font,x,y,string);
}

void sdl_keyboard_init(char * string, void (*cb)(void*arg))
{
	keyboard_index=0;
	if( string ) {
		strcpy(keyboard_buf,string);
		keyboard_index=strlen(keyboard_buf);
	}
	keyboard_buf[keyboard_index]=0;
	keyboard_cb=cb;
}

char * sdl_keyboard_get_buf()
{
	if(keyboard_cb) {
		return keyboard_buf;
	} else {
		return NULL;
	}
}

void sdl_keyboard_manager(SDL_Event * event)
{
	Uint8 *keystate;

	switch (event->type) {
	case SDL_KEYDOWN:
		if( event->key.keysym.sym == SDLK_RETURN ) {
			if( keyboard_cb ) {
				keyboard_cb(NULL);
				keyboard_cb=NULL;
			}
		}

		if( event->key.keysym.sym == SDLK_DELETE ||
				event->key.keysym.sym == SDLK_BACKSPACE) {
			if(keyboard_index > 0 ) {
				keyboard_index--;
			}
			keyboard_buf[keyboard_index]=0;
		}

		if( event->key.keysym.sym >= SDLK_SPACE &&
				event->key.keysym.sym < SDLK_DELETE ) {

			/* Uppercase */
			keystate = SDL_GetKeyState(NULL);
			if( (keystate[SDLK_RSHIFT] ||
					keystate[SDLK_LSHIFT] ) &&
					(event->key.keysym.sym >=SDLK_a &&
					 event->key.keysym.sym <=SDLK_z) ) {
				event->key.keysym.sym = (SDLKey)(event->key.keysym.sym-32);
			}
			keyboard_buf[keyboard_index]=event->key.keysym.sym;
			if( keyboard_index < sizeof(keyboard_buf)) {
				keyboard_index++;
			}
			keyboard_buf[keyboard_index]=0;
		}
		break;
	default:
		break;
	}
}
