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
#include "game.h"
#include "item.h"
#include "screen.h"
#include "exe.h"
#include "terrain.h"
#include "llist.h"

/* Order in file */
#define ANI_BG 0
#define ANI_WIZ_LAYOUT 8
#define ANI_WIZ_B1 9
#define ANI_WIZ_B8 16
#define ANI_WIZ_B15 23
#define ANI_BOOK_WH1 24
#define ANI_NAME 40
#define ANI_FLAG 46
#define ANI_UNDERLINE 47
#define ANI_RACE_BG 55

#define ITM_OPT_B1 4
#define ITM_OPT_B2 5
#define ITM_OPT_B3 6
#define ITM_OPT_B4 7

#define ITM_OPT_NUM 8
#define ITM_WIZ_NUM 18
#define ITM_NAME_NUM 3
#define ITM_RACE_NUM 34
#define ITM_COL_NUM 8

#define ALIGN_VERT  28

char *diff_text[DIFF_NUM] = {"Easy","Normal","Hard","Impossible"};
char *opp_text[OPPONENT_MAX+1] = {"Zero","One","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten"};
char *size_text[SIZE_NUM] = {"Small","Medium","Large"};
char *magic_text[MAGIC_NUM] = {"Weak","Normal","Powerful"};

extern LBXFontTemplate_t *font_template[];
extern wizard_t default_wizard[];
extern char * retort_name[];
extern intptr_t arcanian_race[];
extern intptr_t myrran_race[];
extern int race_unit[];

static LBXAnimation_t * anim=NULL;
static LBXAnimation_t * wiz=NULL;
static LBXAnimation_t * display_wiz=NULL;
static char * exe_data=NULL;

static char * display_wiz_name=NULL;
static wizard_t * selected_wiz=NULL;

static item_t item_opt[ITM_OPT_NUM];
static item_t item_wiz[ITM_WIZ_NUM];
static item_t item_name[ITM_NAME_NUM];
static item_t item_race[ITM_RACE_NUM];
static item_t item_col[ITM_COL_NUM];

static item_t * item_cur = item_opt;
static int num_item = ITM_OPT_NUM;

static LBXAnimation_t * font = NULL;
static LBXAnimation_t * name_font = NULL;
static LBXAnimation_t * title_font = NULL;
static LBXAnimation_t * retort_font = NULL;
static LBXAnimation_t * edit_font = NULL;
static LBXAnimation_t * race_world_font = NULL;
static LBXAnimation_t * race_font = NULL;
static LBXAnimation_t * race_grayed_font = NULL;

static int end_screen;

static game_t * game;

static void cb_cancel(void * noarg)
{
	end_screen = 0;
}

static void cb_difficulty(void * noarg)
{
	game->difficulty_level = (game->difficulty_level+1)%DIFF_NUM;
	item_set_string(&item_opt[ITM_OPT_B1],diff_text[game->difficulty_level]);
}

static void cb_opponent(void * noarg)
{
	game->opponent_num = (game->opponent_num+1)%(OPPONENT_MAX+1);
	item_set_string(&item_opt[ITM_OPT_B2],opp_text[game->opponent_num]);
}

static void cb_size(void * noarg)
{
	game->land_size = (game->land_size+1)%SIZE_NUM;
	item_set_string(&item_opt[ITM_OPT_B3],size_text[game->land_size]);
}

static void cb_magic(void * noarg)
{
	game->magic = (game->magic+1)%MAGIC_NUM;
	item_set_string(&item_opt[ITM_OPT_B4],magic_text[game->magic]);
}

void cb_change_wiz_display(void * arg)
{
	intptr_t wiz_num = (intptr_t)arg;
	display_wiz = &wiz[wiz_num];
	selected_wiz = &default_wizard[wiz_num];
	display_wiz_name = selected_wiz->name;
}

void cb_display_wizard_selection_screen(void * noarg)
{
	item_cur = item_wiz;
	num_item = ITM_WIZ_NUM;
}

void cb_display_race(void * noarg)
{
	selected_wiz->name = strdup(sdl_keyboard_get_buf());
	item_cur = item_race;
	num_item = ITM_RACE_NUM;
}

void cb_display_wizard_name(void * noarg)
{
	item_cur = item_name;
	num_item = ITM_NAME_NUM;
	sdl_keyboard_init(display_wiz_name,cb_display_race);
}

void cb_display_color(void * race)
{
	selected_wiz->race = (intptr_t)race;
	item_cur = item_col;
	num_item = ITM_COL_NUM;
}

void cb_choose_color(void * num_color)
{
#define UNITS	4
	int color[5] = {COL_GREEN,COL_BLUE,COL_RED,COL_PURPLE,COL_YELLOW};
	intptr_t col = (intptr_t)num_color;
	unit_t * unit[UNITS];
	city_t * city;
	int i;

	city=new city_t;

	selected_wiz->color=color[col];
	game->wiz=selected_wiz;
	game->wiz[0].gold=100;
	game->wiz[0].mana=0;
	game->wiz[0].group=NULL;
	game->wiz[0].city=NULL;

	terrain_generate(DEF_MAP_W,DEF_MAP_H,game);

	/* FIXME create the first city */
	city->population=4000;
	city->game=game;
	city->side=0;
	do {
		city->x=rand()%game->map->width;
		city->y=rand()%game->map->height;
	} while (!is_ground(game->map->side[city->side].tile[city->x][city->y]));

	game->wiz[0].city = add_llist(game->wiz[0].city,(void *)city);

	game->wiz[0].group=NULL;
	/* FIXME create an arbitrary unit here */
	for(i=0; i<UNITS; i++) {
		unit[i]=new unit_t;
		unit[i]->type=race_unit[game->wiz[0].race]+i;
		unit[i]->side=0;
		unit[i]->def=exe_get_unit_def(unit[i]->type);
		unit[i]->hp = unit[i]->def->hp;
		unit[i]->fig = unit[i]->def->num_figures;

		unit[i]->x=city->x;
		unit[i]->y=city->y;
		unit[i]->owner=&game->wiz[0];

		game->wiz[0].group = add_group(game->wiz[0].group,unit[i]);
	}

	end_screen=1;
}

static void load_font(SDL_Renderer * render)
{
	LBXGfxPaletteEntry_t pal[5] = {{0x30,0x28,0x18},{0x30,0x28,0x18},{0x30,0x28,0x18},{0x30,0x28,0x18},{0xa0,0x84,0x68}};
	LBXGfxPaletteEntry_t name_pal[6] = {{0x30,0x28,0x18},{0x30,0x28,0x18},{0x30,0x28,0x18},{0x30,0x28,0x18},{0x30,0x28,0x18},{0xa0,0x84,0x68}};
	LBXGfxPaletteEntry_t title_pal[14] = {{0,0,0},{0,255,0},{248,200,96},{248,200,32},{232,164,32},{216,148,24},{0,0,255},{200,136,24},{160,108,24},{120,84,32},{96,68,32},{255,0,0},{72,56,32},{0,0,0}};
	LBXGfxPaletteEntry_t retort_pal[3] = {{136,96,32},{232,164,32},{48,40,24}};
	LBXGfxPaletteEntry_t edit_pal[4] = {{0xa0,0x84,0x68},{0xa0,0x84,0x68},{0xa0,0x84,0x68},{0xa0,0x84,0x68}};
	LBXGfxPaletteEntry_t race_world_pal[5] = {{200,136,24},{200,136,24},{200,136,24},{200,136,24},{0,0,0}};
	LBXGfxPaletteEntry_t race_pal[3] = {{112,84,64},{184,152,112},{8,12,8}};
	LBXGfxPaletteEntry_t race_grayed_pal[3] = {{112,84,64},{96,72,56},{8,12,8}};

	if(font == NULL ) {
		font = lbx_generate_font(render,font_template[LBX_FONT_BIG],pal,1);
	}
	if(name_font == NULL ) {
		name_font = lbx_generate_font(render,font_template[LBX_FONT_BIG_GOTH],name_pal,1);
	}
	if(title_font == NULL ) {
		title_font = lbx_generate_font(render,font_template[LBX_FONT_HUGE_GOTH],title_pal,1);
	}
	if(retort_font == NULL ) {
		retort_font = lbx_generate_font(render,font_template[LBX_FONT_TINY],retort_pal,1);
	}
	if(edit_font == NULL ) {
		edit_font = lbx_generate_font(render,font_template[LBX_FONT_BIG],edit_pal,0);
	}
	if(race_world_font == NULL ) {
		race_world_font = lbx_generate_font(render,font_template[LBX_FONT_BIG],race_world_pal,1);
	}
	if(race_font == NULL ) {
		race_font = lbx_generate_font(render,font_template[LBX_FONT_SMALL],race_pal,1);
	}
	if(race_grayed_font == NULL ) {
		race_grayed_font = lbx_generate_font(render,font_template[LBX_FONT_SMALL],race_grayed_pal,1);
	}
}

/* return 0 if conf is aborted, return 1 if conf if OK */
int screen_newgame(SDL_Renderer * render,game_t * new_game)
{
	SDL_Event event;
	intptr_t i;
	int j;
	int index;
	SDL_Rect rect;
	char * key_buf;

	game=new_game;

	game->difficulty_level = DIFF_EASY;
	game->opponent_num = 0;
	game->land_size = SIZE_MEDIUM;
	game->magic = MAGIC_NORMAL;

	end_screen = -1;

	item_cur = item_opt;
	num_item = ITM_OPT_NUM;

	display_wiz = NULL;

	sdl_keyboard_init(NULL,NULL);

	/* Load resource */
	if(anim==NULL) {
		anim = load_graphics(render,"NEWGAME.LBX");
		if(anim == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	if(wiz==NULL) {
		wiz = load_graphics(render,"WIZARDS.LBX");
		if(wiz == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	if(exe_data==NULL) {
		exe_data=exe_init("WIZARDS.EXE");
		if( exe_data == NULL ) {
			exit(EXIT_FAILURE);
		}
	}

	load_font(render);

	/* Option screen */
	/* We use first 8 files in the LBX */
	j=0;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],0,0,&anim[j]);
	j++;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],165,0,&anim[j]);
	j++;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],252,179,&anim[j]);
	item_set_frame_click(&item_opt[j],1);
	item_set_click_left(&item_opt[j],cb_display_wizard_selection_screen,NULL);
	j++;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],171,179,&anim[j]);
	item_set_frame_click(&item_opt[j],1);
	item_set_click_left(&item_opt[j],cb_cancel,NULL);
	j++;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],251,39,&anim[j]);
	item_set_click_left(&item_opt[j],cb_difficulty,NULL);
	item_set_font(&item_opt[j],font);
	item_set_string(&item_opt[j],diff_text[game->difficulty_level]);
	j++;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],251,66,&anim[j]);
	item_set_click_left(&item_opt[j],cb_opponent,NULL);
	item_set_font(&item_opt[j],font);
	item_set_string(&item_opt[j],opp_text[game->opponent_num]);
	j++;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],251,93,&anim[j]);
	item_set_click_left(&item_opt[j],cb_size,NULL);
	item_set_font(&item_opt[j],font);
	item_set_string(&item_opt[j],size_text[game->land_size]);
	j++;
	item_init(&item_opt[j]);
	item_set_frame(&item_opt[j],251,120,&anim[j]);
	item_set_click_left(&item_opt[j],cb_magic,NULL);
	item_set_font(&item_opt[j],font);
	item_set_string(&item_opt[j],magic_text[game->magic]);

	/* Wizard selection screen */
	j=0;
	item_init(&item_wiz[j]);
	item_set_frame(&item_wiz[j],0,0,&anim[ANI_BG]);
	j++;
	item_init(&item_wiz[j]);
	item_set_frame(&item_wiz[j],165,18,&anim[ANI_WIZ_LAYOUT]);
	j++;

	for(i=0; i<WIZARD_NUM/2; i++) {
		item_init(&item_wiz[j]);
		item_set_frame(&item_wiz[j],169,ALIGN_VERT+(i*22),
					   &anim[ANI_WIZ_B1+i]);
		item_set_click_left(&item_wiz[j],cb_display_wizard_name,NULL);
		item_set_over(&item_wiz[j],cb_change_wiz_display,(void*)i);
		item_set_font(&item_wiz[j],font);
		item_set_string(&item_wiz[j],default_wizard[i].name);
		j++;
	}

	for(i=0; i<WIZARD_NUM/2; i++) {
		item_init(&item_wiz[j]);
		item_set_frame(&item_wiz[j],245,ALIGN_VERT+(i*22),
					   &anim[ANI_WIZ_B8+i]);
		item_set_click_left(&item_wiz[j],cb_display_wizard_name,NULL);
		item_set_over(&item_wiz[j],cb_change_wiz_display,(void*)(i+7));
		item_set_font(&item_wiz[j],font);
		item_set_string(&item_wiz[j],default_wizard[i+7].name);
		j++;
	}

	item_init(&item_wiz[j]);
	item_set_frame(&item_wiz[j],245,ALIGN_VERT+(7*22),&anim[ANI_WIZ_B15]);
	item_set_font(&item_wiz[j],font);
	item_set_string(&item_wiz[j],"Custom");
	j++;
	item_init(&item_wiz[j]);
	item_set_frame(&item_wiz[j],242,10,NULL);
	item_set_font(&item_wiz[j],title_font);
	item_set_string(&item_wiz[j],"Select Wizard");


	/* Wizard name selection */
	j=0;
	item_init(&item_name[j]);
	item_set_frame(&item_name[j],0,0,&anim[ANI_BG]);
	j++;
	item_init(&item_name[j]);
	item_set_frame(&item_name[j],242,10,NULL);
	item_set_font(&item_name[j],title_font);
	item_set_string(&item_name[j],"Wizard's Name");
	j++;
	item_init(&item_name[j]);
	item_set_frame(&item_name[j],180,18,&anim[ANI_NAME]);

	/* Race selection */
	j=0;
	item_init(&item_race[j]);
	item_set_frame(&item_race[j],0,0,&anim[ANI_BG]);
	j++;
	item_init(&item_race[j]);
	item_set_frame(&item_race[j],242,8,NULL);
	item_set_font(&item_race[j],title_font);
	item_set_string(&item_race[j],"Select Race");
	j++;
	item_init(&item_race[j]);
	item_set_frame(&item_race[j],180,17,&anim[ANI_UNDERLINE]);
	j++;
	item_init(&item_race[j]);
	item_set_frame(&item_race[j],206,34,&anim[ANI_RACE_BG]);
	j++;
	item_init(&item_race[j]);
	item_set_frame(&item_race[j],238,29,NULL);
	item_set_font(&item_race[j],race_world_font);
	item_set_string(&item_race[j],"Arcanian Races:");
	j++;
	item_init(&item_race[j]);
	item_set_frame(&item_race[j],238,139,NULL);
	item_set_font(&item_race[j],race_world_font);
	item_set_string(&item_race[j],"Myrran Races:");
	j++;
	for(i=0; i<ARCANIAN_RACE_NUM; i++) {
		item_init(&item_race[j]);
		item_set_frame(&item_race[j],238,40+i*10,NULL);
		item_set_font(&item_race[j],race_font);
		item_set_string(&item_race[j],exe_get_race_name(arcanian_race[i]));
		j++;
		item_init(&item_race[j]);
		item_set_geometry(&item_race[j],200,35+i*10,80,10);
		j++;
	}
	for(i=0; i<MYRRAN_RACE_NUM; i++) {
		item_init(&item_race[j]);
		item_set_frame(&item_race[j],238,150+i*10,NULL);
		item_set_font(&item_race[j],race_grayed_font);
		item_set_string(&item_race[j],exe_get_race_name(myrran_race[i]));
		j++;
		item_init(&item_race[j]);
		item_set_geometry(&item_race[j],200,145+i*10,80,10);
		j++;
	}

	/* Color selection */
	j=0;
	item_init(&item_col[j]);
	item_set_frame(&item_col[j],0,0,&anim[ANI_BG]);
	j++;
	item_init(&item_col[j]);
	item_set_frame(&item_col[j],158,0,&anim[ANI_FLAG]);
	j++;
	item_init(&item_col[j]);
	item_set_frame(&item_col[j],242,8,NULL);
	item_set_font(&item_col[j],title_font);
	item_set_string(&item_col[j],"Select Banner");
	j++;
	for(i=0; i<COL_NUM; i++) {
		item_init(&item_col[j]);
		item_set_geometry(&item_col[j],165,22+i*35,150,35);
		item_set_click_left(&item_col[j],cb_choose_color,(void *)i);
		j++;
	}

	/* Main loop */
	while( end_screen == -1) {
		while (SDL_PollEvent(&event)) {
			if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) {
				return 0;
			}
			sdl_screen_manager(&event);
			sdl_mouse_manager(&event,item_cur,num_item);
			sdl_keyboard_manager(&event);
		}

		sdl_blit_item_list(item_cur,num_item);

		/* Display wizard and its retort */
		if( display_wiz != NULL ) {
			rect.x = 24;
			rect.y = 10;
			rect.w = 109;
			rect.h = 104;
			sdl_blit_anim(display_wiz,&rect,0,-1);
			sdl_print_center(name_font,78,125,display_wiz_name);
			for(i=0; i<RT_NUM; i++) {
				if( (selected_wiz->retort) & (1<<i) ) {
					sdl_print(retort_font,10,180,retort_name[i]);
				}
			}
			rect.x = 36;
			rect.y = 135;
			for(i=0; i<BOOK_NUM; i++) {
				srand(i);
				for(j=0; j<selected_wiz->book[i]; j++) {
					index = ANI_BOOK_WH1+(i*3)+(rand()%3);
					rect.w=anim[index].w;
					rect.h=anim[index].h;
					sdl_blit_anim(&anim[index],&rect,0,-1);
					rect.x+=rect.w;
				}
			};
		}

		/* Display available races for the selected wizard */
		if(selected_wiz) {
			for(i=0,j=6; i<ARCANIAN_RACE_NUM; i++,j++) {
				if((selected_wiz->retort&SET_RT(RT_MYRRAN))==0) {
					item_set_font(&item_race[j],race_font);
					j++;
					item_set_click_left(&item_race[j],cb_display_color,(void *)arcanian_race[i]);
				} else {
					item_set_font(&item_race[j],race_grayed_font);
					j++;
					item_set_click_left(&item_race[j],NULL,NULL);
				}
			}
			for(i=0,j=6+(ARCANIAN_RACE_NUM*2); i<MYRRAN_RACE_NUM; i++,j++) {
				if((selected_wiz->retort&SET_RT(RT_MYRRAN))!=0) {
					item_set_font(&item_race[j],race_font);
					j++;
					item_set_click_left(&item_race[j],cb_display_color,(void *)myrran_race[i]);
				} else {
					item_set_font(&item_race[j],race_grayed_font);
					j++;
					item_set_click_left(&item_race[j],NULL,NULL);
				}
			}
		}

		key_buf = sdl_keyboard_get_buf();
		if(key_buf) {
			sdl_print(edit_font,193,37,key_buf);
		}

		sdl_blit_to_screen();

		sdl_loop_manager();
	}

	return end_screen;
}
