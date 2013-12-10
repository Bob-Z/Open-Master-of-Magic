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
#include "lbx.h"
#include "item.h"
#include "game.h"
#include "sdl.h"
#include "terrain.h"
#include "screen.h"

#define BACK_COLOR	14
#define CITY_WALL	20
#define CITY_NOWALL	21

extern LBXFontTemplate_t *font_template[];

static SDL_Renderer * local_render;

static LBXAnimation_t * anim=NULL;
static LBXAnimation_t * tile=NULL;
static LBXAnimation_t * unit=NULL;
static LBXAnimation_t * unit1=NULL;
static LBXAnimation_t * unit2=NULL;
static LBXAnimation_t * back=NULL;

#define ITM_UI_NUM 10

static item_t item_ui[ITM_UI_NUM];
static item_t * item_tile = NULL;
static item_t * item_unit = NULL;
static item_t * item_back = NULL;
static item_t * item_city = NULL;
static item_t * item_encounter = NULL;

static int orig_x = 115;
static int orig_y = 100;

static int cur_tile_x = 0;
static int cur_tile_y = 0;

static int cur_side = 0;

static LBXAnimation_t * font = NULL;

static group_t * selected_group;

static int end_screen;

static int blink_state = 0;
static Uint32 blink_time = 0;

static void cb_move_map(void * arg)
{
	intptr_t coord = (intptr_t)arg;
	int x,y;

	x = coord >> 16;
	y = coord & 0xffff;

	cur_tile_x = x;
	cur_tile_y = y;
}

/* Return screen coordinate for the given tile coordinate */
static int get_tile_coord_x(int tile, game_t * game)
{
	int wrap_left_relative_tile;
	int wrap_right_relative_tile;
	int relative_tile;

	wrap_left_relative_tile = (tile + game->map->width - cur_tile_x) % game->map->width;
	wrap_right_relative_tile = (tile - cur_tile_x - game->map->width) % game->map->width;

	if( abs(wrap_left_relative_tile) < abs(wrap_right_relative_tile)) {
		relative_tile = wrap_left_relative_tile;
	} else {
		relative_tile = wrap_right_relative_tile;
	}

	return ( orig_x + (relative_tile*TILE_SIZE_W));

}
static int get_tile_coord_y(int tile, game_t * game)
{
	int relative_tile;

	relative_tile = tile - cur_tile_y;

	return ( orig_y + (relative_tile*TILE_SIZE_H) );
}
static int draw_tile(game_t * game)
{
	int tile_x,tile_y;
	int index;
	item_t * item_cur;
	int num_tile_w = ORIGINAL_GAME_WIDTH/TILE_SIZE_W ;
	int num_tile_h = ORIGINAL_GAME_HEIGHT/TILE_SIZE_H +2 ;
	int x,y;
	int count_w;
	int count_h;
	int tile_index;

	if( item_tile == NULL ) {
		item_tile = (item_t*)malloc(sizeof(item_t)*game->map->width*game->map->height);
	}

	index=0;
	for( count_w = -num_tile_w/2; count_w < num_tile_w/2; count_w ++) {
		for( count_h=(-num_tile_h/2); count_h<num_tile_h/2; count_h++) {
			tile_y=cur_tile_y+count_h;
			if(tile_y < 0 || tile_y >= game->map->height) {
				continue;
			}
			tile_x=(cur_tile_x+game->map->width+count_w)%game->map->width;
			item_cur = &item_tile[index];
			item_init(item_cur);
			x=orig_x+(count_w*TILE_SIZE_W);
			y=orig_y+(count_h*TILE_SIZE_H);
			tile_index=game->map->side[cur_side].tile[tile_x][tile_y];
			item_set_anim(item_cur,x,y,&tile[tile_index]);
			item_set_click_right(item_cur,cb_move_map,(void*)(((intptr_t)tile_x<<16)+(intptr_t)tile_y));
			index++;
		}
	}

	return index;
}

static int draw_unit(game_t * game)
{
	int i;
	int x,y;
	int num_item;
	group_t * grp_list;
	unit_list_t * ulist;
	unit_t * cur_unit;

	if( item_unit != NULL ) {
		free(item_unit);
	}
	if( item_back != NULL ) {
		free(item_back);
	}

	if(selected_group == NULL ) {
		selected_group = (group_t *)game->wiz[0].group;
		blink_state = 0;
	}

	item_unit = NULL;
	item_back = NULL;

	num_item = 0;
	for(i=0; i<game->opponent_num+1; i++) {
		grp_list = game->wiz[i].group;
		while(grp_list!=NULL ) {
			ulist = (unit_list_t *)grp_list->data;;
			cur_unit = (unit_t*)ulist->data;
			if(cur_unit->side != cur_side) {
				grp_list = grp_list->next;
				continue;
			}

			item_unit = (item_t*)realloc(item_unit,sizeof(item_t)*(num_item+1));
			item_back = (item_t*)realloc(item_back,sizeof(item_t)*(num_item+1));

			x = get_tile_coord_x(cur_unit->x, game);
			y = get_tile_coord_y(cur_unit->y, game);

			item_init(&item_unit[num_item]);
			item_init(&item_back[num_item]);

			/* Blinking selected unit */
			if( selected_group && grp_list == selected_group ) {
				if( SDL_GetTicks() > (blink_time+UNIT_BLINK_TIME) ) {
					blink_state=(blink_state+1)%2;
					blink_time=SDL_GetTicks();
				}

				item_set_frame(&item_back[num_item],x,y,
							   blink_state?&back[game->wiz[i].color + BACK_COLOR]:NULL);

				item_set_frame(&item_unit[num_item],x,y+1,
							   blink_state?&unit[cur_unit->type]:NULL);
			} else {
				item_set_frame(&item_back[num_item],x,y,
							   &back[game->wiz[i].color+BACK_COLOR]);

				item_set_frame(&item_unit[num_item],x,y+1,
							   &unit[cur_unit->type]);

			}
			num_item++;
			grp_list = grp_list->next;
		}
	}

	return num_item;
}

static void cb_city_screen(void * arg)
{
	screen_city(local_render,(city_t *) arg);
}

static int draw_city(game_t * game)
{
	int i;
	int x,y;
	int num_item;
	int size_pic;
	llist_t * cur_list;
	city_t * cur_city;

	if( item_city != NULL ) {
		free(item_city);
	}

	item_city = NULL;

	num_item = 0;
	for(i=0; i<game->opponent_num+1; i++) {
		cur_list = game->wiz[i].city;
		while(cur_list!=NULL ) {
			cur_city = (city_t*)cur_list->data;

			if(cur_city->side != cur_side) {
				cur_list = cur_list->next;
				continue;
			}

			item_city = (item_t*)realloc(item_city,sizeof(item_t)*(num_item+1));

			x = get_tile_coord_x(cur_city->x, game);
			y = get_tile_coord_y(cur_city->y, game);

			/* City picture are larger than tiles picture */
			x -= (back[CITY_NOWALL].w - TILE_SIZE_W)/2;
			y -= (back[CITY_NOWALL].h - TILE_SIZE_H)/2;

			item_init(&item_city[num_item]);

			size_pic = cur_city->population/10000;
			if(size_pic > 5 ) {
				size_pic = 5;
			}
			item_set_frame(&item_city[num_item],x,y+1,
						   &back[CITY_NOWALL]);
			item_set_frame_normal(&item_city[num_item],size_pic);

			item_set_click_right(&item_city[num_item],cb_city_screen,cur_city);

			num_item++;
			cur_list = cur_list->next;
		}
	}

	return num_item;
}

static int draw_encounter(game_t * game)
{
	int x,y;
	int num_item;
	llist_t * cur_list;
	encounter_t * cur_encounter;

	if( item_encounter != NULL ) {
		free(item_encounter);
	}

	item_encounter = NULL;

	num_item = 0;

	cur_list = game->encounter;
	while(cur_list!=NULL ) {
		cur_encounter = (encounter_t*)cur_list->data;

		if(cur_encounter->side != cur_side) {
			cur_list = cur_list->next;
			continue;
		}

		item_encounter = (item_t*)realloc(item_encounter,sizeof(item_t)*(num_item+1));

		x = get_tile_coord_x(cur_encounter->x, game);
		y = get_tile_coord_y(cur_encounter->y, game);

		item_init(&item_encounter[num_item]);

		item_set_frame(&item_encounter[num_item],x,y+1,
					   &back[cur_encounter->type]);

		num_item++;
		cur_list = cur_list->next;
	}

	return num_item;
}

/* Remove the first group with no more unit */
static void remove_dead_groups(game_t * game)
{
	int i;

	for(i=0; i<game->opponent_num+1; i++) {
		game->wiz[i].group = del_llist(game->wiz[i].group,NULL);
	}
}

static void move_unit(game_t * game,int key)
{
	int new_x;
	int new_y;
	int side;
	int target_tile;
	uint8_t move_flag;
	encounter_t * enc;
	unit_t * unit;
	unit_list_t * ulist;

	if(selected_group==NULL) {
		return;
	}

	ulist = (unit_list_t *)selected_group->data;
	unit = (unit_t*)ulist->data;
	new_x = unit->x;
	new_y = unit->y;
	side = unit->side;

	switch( key ) {
	case SDLK_ESCAPE:
		return;
	case SDLK_KP_1:
		new_x--;
		new_y++;
		break;
	case SDLK_KP_2:
		new_y++;
		break;
	case SDLK_KP_3:
		new_x++;
		new_y++;
		break;
	case SDLK_KP_4:
		new_x--;
		break;
	case SDLK_KP_6:
		new_x++;
		break;
	case SDLK_KP_7:
		new_x--;
		new_y--;
		break;
	case SDLK_KP_8:
		new_y--;
		break;
	case SDLK_KP_9:
		new_x++;
		new_y--;
		break;
	default:
		return;
		break;
	}

	if(new_x<0) {
		new_x = DEF_MAP_W-1;
	}
	if(new_x>=DEF_MAP_W) {
		new_x = 0;
	}
	if(new_y<0) {
		new_y = 0;
	}
	if(new_y>=DEF_MAP_H) {
		new_y = DEF_MAP_H-1;
	}

	/* Test destination tile cost and event */
	target_tile = game->map->side[side].tile[new_x][new_y];
	/* FIXME only take first unit flag into account */
	move_flag = unit->def->move_flag;

	/* Type of tile : cost */
	if(!is_ground(target_tile)) {
		if( (move_flag & MOVE_SWIM) || (move_flag & MOVE_SAIL)) {
			ulist = (unit_list_t *)selected_group->data;
			while(ulist!= NULL) {
				unit = (unit_t*)ulist->data;
				unit->x = new_x;
				unit->y = new_y;
				ulist=ulist->next;
			}
		}
	} else {
		ulist = (unit_list_t *)selected_group->data;
		while(ulist!= NULL) {
			unit = (unit_t*)ulist->data;
			unit->x = new_x;
			unit->y = new_y;
			ulist=ulist->next;
		}
	}

	/* Encounter ? */
	if( (enc=is_encounter_present(game,new_x,new_y,unit->side)) ) {
		if(enc->unit) {
			screen_combat(local_render,game,(unit_list_t **)&selected_group->data,&enc->unit,new_x,new_y,unit->side);
		}
	}

	remove_dead_groups(game);

	/* Avoid blinking while moving */
	blink_state=1;
	blink_time=SDL_GetTicks();

}

static void load_font()
{
	LBXGfxPaletteEntry_t pal[3] = {{136,132,128},{248,252,248},{0,0,0}};

	if(font == NULL ) {
		font = lbx_generate_font(local_render,font_template[LBX_FONT_TINY],pal,1);
	}
}

static void cb_plane(void * arg)
{
	cur_side = (cur_side+1)%2;
}

void screen_play(SDL_Renderer * render,game_t * game)
{
	char gold_buf[128];
	char mana_buf[128];
	SDL_Event event;
	int tile_num;
	int unit_num;
	int city_num;
	int encounter_num;
	int i;
	int j;
	int x;
	LBXAnimation_t * anim_ptr;
	city_t * city;

	local_render = render;

	end_screen = -1;

	selected_group = NULL;

	city = (city_t*)game->wiz[0].city->data;
	cur_tile_x = city->x;
	cur_tile_y = city->y;

	load_font();

	/* Load resource */
	if(anim==NULL) {
		anim = load_graphics(render,"MAIN.LBX");
		if(anim == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	/* Load tiles */
	if(tile==NULL) {
		tile = load_graphics(render,"TERRAIN.LBX");
		if(tile == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	/* Load unit */
	if(unit1==NULL) {
		unit1 = load_graphics(render,"UNITS1.LBX");
		if(unit1 == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	if(unit2==NULL) {
		unit2 = load_graphics(render,"UNITS2.LBX");
		if(unit2 == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	/* Concat unit1 and unit2 */
	if(unit==NULL) {
		j=0;
		anim_ptr = unit1;
		for(i=0; i<2; i++) {
			while(anim_ptr->num_frame!=0) {
				unit = (LBXAnimation_t*)realloc(unit,(j+1)*sizeof(LBXAnimation_t));
				memcpy(&unit[j],anim_ptr,sizeof(LBXAnimation_t));
				anim_ptr++;
				j++;
			}
			anim_ptr= unit2;
		}
	}

	/* Load back */
	if(back==NULL) {
		back = load_graphics(render,"MAPBACK.LBX");
		if(back == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	j=0;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],0,0,&anim[j]);
	j++;
	x=7;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],x,4,&anim[j]);
	item_set_frame_click(&item_ui[j],1);
	j++;
	x+=item_ui[j-1].anim->w + 1;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],x,4,&anim[j]);
	item_set_frame_click(&item_ui[j],1);
	j++;
	x+=item_ui[j-1].anim->w + 1;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],x,4,&anim[j]);
	item_set_frame_click(&item_ui[j],1);
	j++;
	x+=item_ui[j-1].anim->w + 1;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],x,4,&anim[j]);
	item_set_frame_click(&item_ui[j],1);
	j++;
	x+=item_ui[j-1].anim->w + 1;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],x,4,&anim[j]);
	item_set_frame_click(&item_ui[j],1);
	j++;
	x+=item_ui[j-1].anim->w + 1;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],x,4,&anim[j]);
	item_set_frame_click(&item_ui[j],1);
	j++;
	x+=item_ui[j-1].anim->w + 1;
	item_init(&item_ui[j]);
	item_set_frame(&item_ui[j],x,4,&anim[j]);
	item_set_frame_click(&item_ui[j],1);
	item_set_click_left(&item_ui[j],cb_plane,NULL);
	j++;

	/* Main loop */
	while( end_screen == -1) {
		tile_num = draw_tile(game);
		city_num = draw_city(game);
		encounter_num = draw_encounter(game);
		unit_num = draw_unit(game);

		/* Print gold */
		item_init(&item_ui[8]);
		item_set_font(&item_ui[8],font);
		sprintf(gold_buf,"%d GP",game->wiz[0].gold);
		item_set_string(&item_ui[8],gold_buf);
		item_set_frame(&item_ui[8],264,71,NULL);

		/* Print mana */
		item_init(&item_ui[9]);
		item_set_font(&item_ui[9],font);
		sprintf(mana_buf,"%d MP",game->wiz[0].mana);
		item_set_string(&item_ui[9],mana_buf);
		item_set_frame(&item_ui[9],300,71,NULL);

		while (SDL_PollEvent(&event)) {
			if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) {
				return;
			}

			/* Move selected_unit */
			if(event.type==SDL_KEYDOWN ) {
				move_unit(game,event.key.keysym.sym);
			}

			sdl_screen_manager(&event);
			sdl_mouse_manager(&event,item_ui,ITM_UI_NUM);
			sdl_mouse_manager(&event,item_city,city_num);
			sdl_mouse_manager(&event,item_encounter,encounter_num);
			sdl_mouse_manager(&event,item_tile,tile_num);
			sdl_mouse_manager(&event,item_unit,unit_num);
			sdl_keyboard_manager(&event);
		}

		SDL_RenderClear(render);

		sdl_blit_item_list(item_tile,tile_num);
		sdl_blit_item_list(item_city,city_num);
		sdl_blit_item_list(item_encounter,encounter_num);
		sdl_blit_item_list(item_back,unit_num);
		sdl_blit_item_list(item_unit,unit_num);
		sdl_blit_item_list(item_ui,ITM_UI_NUM);

		sdl_blit_to_screen();

		sdl_loop_manager();
	}

	return;
}
