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
#include "exe.h"
#include "terrain.h"

/* Order in file */
#define ANI_BG			1
#define ANI_MOUND		19
#define ANI_TOWER		20
#define ANI_RUINS		21
#define ANI_KEEP		22
#define ANI_TEMPLE		23
#define ANI_FALLEN_TEMPLE	76
/* CMBTCITY.LBX */
#define ANI_HIGHLIGHT_WHITE	31
#define ANI_HIGHLIGHT_RED	32
/* COMPIX.LBX */
#define ANI_WAIT		1
#define ANI_DONE		2

#define ITM_NUM 		3

#define ANIM_DURATION		1000

#define TILE_W			30
#define TILE_H			16
#define GRID_WIDTH		22
#define GRID_HEIGHT		22
/* 36 is the height of the bottom bar of the combat screen */
#define GRID_TOP	((ORIGINAL_GAME_HEIGHT/2)-36/2)
#define GRID_LEFT	((ORIGINAL_GAME_WIDTH/2)-((GRID_WIDTH/2)*TILE_W))

#define M_COST_GRASS	2 // Cost of a move onto a grass tile (in 1/2 mouvement point)

extern int mouse_x;
extern int mouse_y;

/* Formation of unit made of several figures */
int formation_x[8][8] = {{0,0,0,0,0,0,0,0}, // x coordinate for 1 figure
			{-7,7,0,0,0,0,0,0}, // x coordinate for 2 figures...
			{7,-7,7,0,0,0,0,0},
			{0,-7,7,0,0,0,0,0},
			{-7,7,0,-7,7,0,0,0},
			{0,4,8,-8,-4,0,0,0},
			{0,0,0,0,0,0,0,0}, // no unit with 7 figures known
			{0,5,10,-2,-10,2,-5,0},
			};
int formation_y[8][8] = {{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{4,0,4,0,0,0,0,0},
			{-4,0,0,4,0,0,0,0},
			{4,4,0,-4,-4,0,0,0},
			{-4,-2,0,0,2,4,0,0},
			{0,0,0,0,0,0,0,0}, // no unit with 7 figures known
			{-5,-3,-2,-2,-1,1,3,5},
			};

int grid_ground[GRID_WIDTH][GRID_HEIGHT];
int grid_unit[GRID_WIDTH][GRID_HEIGHT];

/* Sprite direction */
enum orientation { D_NE,D_E,D_SE,D_S,D_SW,D_W,D_NW,D_N,D_NUM};

static LBXAnimation_t * anim=NULL;
static LBXAnimation_t * grass=NULL;
static LBXAnimation_t * city=NULL;
static LBXAnimation_t * fig=NULL;
static LBXAnimation_t * figure[16];
static LBXAnimation_t * pix=NULL;

static item_t item[ITM_NUM];
static item_t item_ground[GRID_WIDTH*GRID_HEIGHT];
static item_t *item_deco = NULL;
static item_t *item_unit = NULL;

static int end_screen;

static item_t item_highlight_tile;
static item_t item_highlight_unit;

static unit_t* selected_unit=NULL;
static int redraw_unit;
static int rest_unit;
static int wait_anim;

static int cur_gx;
static int cur_gy;

static unit_list_t ** attack;
static unit_list_t ** defense;

static void grid_to_screen(int gx, int gy, int * x, int * y)
{
	*x = GRID_LEFT + (TILE_W/2)*gx + (TILE_W/2)*gy;
	*y = GRID_TOP - (TILE_H/2)*gx + (TILE_H/2)*gy;
}

static void screen_to_grid(int x, int y, int * gx, int * gy)
{
	/* 1000 is to avoid errors due to calculation rounding */
	*gx = (1000*(x-GRID_LEFT))/(2*(TILE_W/2)) - (1000*(y-GRID_TOP))/(2*(TILE_H/2));
	*gx = *gx/1000;
	*gy = (1000*(x-GRID_LEFT))/(2*(TILE_W/2)) + (1000*(y-GRID_TOP))/(2*(TILE_H/2));
	*gy = *gy/1000;
}

static int draw_deco(game_t * game,int map_x, int map_y, int side)
{
	encounter_t * enc;
	int num_item = 0;
	int pic = 0;
	int tile_height = grass[0].frame[0]->h;
	int screen_x;
	int screen_y;

	if( (enc=is_encounter_present(game,map_x,map_y,side)) ) {
		switch(enc->type) {
			case ENC_TOWER:
				pic = ANI_TOWER;
			break;
			case ENC_OWNED_TOWER:
				pic = ANI_TOWER;
			break;
			case ENC_MOUND:
				pic = ANI_MOUND;
			break;
			case ENC_TEMPLE:
				pic = ANI_TEMPLE;
			break;
			case ENC_KEEP:
				pic = ANI_KEEP;
			break;
			case ENC_RUINS:
				pic = ANI_RUINS;
			break;
			case ENC_FALLEN_TEMPLE:
				pic = ANI_FALLEN_TEMPLE;
			break;
			default:
				exit(EXIT_FAILURE);
			break;
		}

		item_deco = (item_t*)realloc(item_deco,sizeof(item_t)*(num_item+1));
		item_init(&item_deco[num_item]);
		grid_to_screen(10, 5, &screen_x, &screen_y);
		/* Adjust the position with regards of the size of the sprite */
		screen_y = screen_y - city[pic].frame[0]->h + tile_height;

		item_set_frame(&item_deco[num_item],screen_x,screen_y,&city[pic]);

		num_item++;
	}

	return num_item;
}
static void cb_tile_highlight(void * arg)
{
	item_t * target = (item_t*)arg;
	static int old_cx=0;
	static int old_cy=0;
	int cx = target->rect.x + TILE_W/2;
	int cy = target->rect.y + TILE_H/2;
	int dist;
	int old_dist;

	/* Tiles are overlapping so we need to test which one is the closest
	to the mouse pointer */

	dist = (cx-mouse_x)*(cx-mouse_x) + (cy-mouse_y)*(cy-mouse_y);
	old_dist = (old_cx-mouse_x)*(old_cx-mouse_x) + (old_cy-mouse_y)*(old_cy-mouse_y);

	if(dist < old_dist) {
		old_cx = cx;
		old_cy = cy;

		item_init(&item_highlight_tile);
		item_set_frame(&item_highlight_tile,target->rect.x,target->rect.y,&city[ANI_HIGHLIGHT_WHITE]);

		screen_to_grid(target->rect.x,target->rect.y,&cur_gx,&cur_gy);
#if 0
		printf("grid : %d,%d\n",cur_gx,cur_gy);
#endif

	}
}

static int path_cost(int sx,int sy, int tx, int ty)
{
	// FIXME: should find the min cost path ant return it
	// for now just count the number of tile and return the global cost	
	int path_x;
	int path_y;

	path_x = abs(sx-tx);
	path_y = abs(sy-ty);

	if( path_y > path_x ) {
		path_x= path_y;
	}

	return path_x*M_COST_GRASS;

}

static unit_t* get_unit_at_pos(int x, int y)
{
	unit_t * unit;
        unit_list_t * cur_unit;
	int i;
	
	cur_unit = *attack;
	/* Do it once for attackers, once for the defenders */
	for( i = 0; i < 2 ; i++) {
		while( cur_unit ) {
			unit = (unit_t*)cur_unit->data;
			/* When dest_g* != g* it means that the animation is
			not finished but the real coordinate of the unit is
			dest_g* */
			if(unit->dest_gx == x && unit->dest_gy == y) {
				return unit;
			}
			cur_unit = cur_unit->next;
		}
		cur_unit = *defense;
	}

	return NULL;
}

static int get_orient(int x,int y)
{
	if(x<=-1) {
		if(y<=-1) return D_NW;
		if(y==0) return D_W;
		if(y>=1) return D_SW;
	}
	if(x==0) {
		if(y<=-1) return D_N;
		if(y==0) return -1;
		if(y>=1) return D_S;
	}
	if(x>=1) {
		if(y<=-1) return D_NE;
		if(y==0) return D_E;
		if(y>=1) return D_SE;
	}

	return -1;
}

static void check_victory()
{
	unit_t * unit;
        unit_list_t * cur_unit;
	int i;
	
	cur_unit = *attack;

	if(cur_unit == NULL) {
		return;
	}

	/* Do it once for attackers, once for the defenders */
	for( i = 0; i < 2 ; i++) {
		while( cur_unit ) {
			unit = (unit_t*)cur_unit->data;
			// Kill unit = remove unit from whatever attack or defense list
			if( unit->fig <= 0 ) {
				*attack = del_llist(*attack,unit);
				*defense = del_llist(*defense,unit);
			}

			cur_unit = cur_unit->next;
		}
		cur_unit = *defense;
	}

	/* Defender victory (or draw) */
	if(*attack==NULL) {
		/*TODO*/
		printf("Defenders win\n");
		end_screen =1;
	}
	/* attacker victory */
	if(*defense==NULL) {
		/*TODO*/
		printf("Attackers win\n");
		end_screen =1;
	}
}

static void unit_attack(unit_t* attack_unit, unit_t* defense_unit)
{
	int strength;
	int to_hit;
	int hits;
	int def;
	int to_block;
	int k;
	int i;
	unit_t * unit_a;
	unit_t * unit_b;
	int hp;
	int f;

	wait_anim = 1;

printf("%s = %d\n",exe_get_unit_name(attack_unit->type),attack_unit->move);
	attack_unit->anim_attack = 1;
	attack_unit->anim_timer = SDL_GetTicks();
//	attack_unit->move -= 2;
printf("%s = %d\n",exe_get_unit_name(attack_unit->type),attack_unit->move);

	defense_unit->anim_attack = 1;
	defense_unit->anim_timer = SDL_GetTicks();

	/* combat calculation */
	printf("- attack\n");
	unit_a=attack_unit;
	unit_b=defense_unit;
	for(k=0;k<2;k++) {
		/* for each figure in the unit */
		for(f=0;f<unit_a->fig;f++) {
			printf("Figure %d ",f);
			/* attack */
			strength = unit_a->def->melee_attack_str;
			printf("strength=%d ",strength);
			to_hit = 30 + (10*unit_a->def->plus_to_hit);
			printf("to hit=%d ",to_hit);
			hits=0;
			for(i=0;i<strength;i++) {
				if( (rand()%100)+1 <= to_hit ) {
					hits++;
				}
			}
			printf("hits=%d\n",hits);
			/* defense */
			while ( hits != 0 && unit_b->fig > 0 ) {
				def = unit_b->def->defense;
				printf("Figure %d defense=%d ",unit_b->fig-1,def);
				to_block = 30;
				printf("to block=%d ",to_block);
				for(i=0;i<def && hits>0;i++) {
					if( (rand()%100)+1 <= to_block ) {
						hits--;
					}
				}
				printf("hits=%d\n",hits);

				/* Wound figure */
				if( unit_b->hp > hits ) {
					hp = unit_b->hp-hits;
					if( hits > 0 ) {
						printf("lives:%d -> %d\n",unit_b->hp,hp);
					}
					unit_b->hp=hp;
					break;
				}
				/* wound figure */
				else {
					hits = hits - unit_b->hp;
					unit_b->fig--;
					printf("fig=%d ",unit_b->fig);
					printf("hits=%d\n",hits);
					if(unit_b->fig > 0) {
						unit_b->hp=unit_b->def->hp;
					}
				}
			}
		}

		if(k==0) {
			printf("\n- counter attack\n");
			unit_a=defense_unit;
			unit_b=attack_unit;
		}
	}

	printf("\n\n\n");
}

/* select the first unit that can move */
static void select_unit(game_t * game)
{
	unit_t *unit;
	unit_list_t *unit_list;
	int i;

	unit_list=*attack;
	for(i=0;i<2;i++) {
		while(unit_list!=NULL) {
			unit = (unit_t*)unit_list->data;
			if(unit->move > 0 && unit->owner == game->wiz) {
				selected_unit = unit;
				return;
			}
			unit_list=unit_list->next;
		}
		unit_list=*defense;
	}

	selected_unit=NULL;
}

/* select the next unit that can move */
static void select_next_unit(game_t * game)
{
	unit_t *unit;
	unit_list_t *unit_list;
	int i;
	int found=0; //Wait until we found the current selected unit in the list

	unit_list=*attack;
	for(i=0;i<2;i++) {
		while(unit_list!=NULL) {
			unit = (unit_t*)unit_list->data;
			if(found == 1 && unit->move > 0 && unit->owner == game->wiz) {
				selected_unit = unit;
				return;
			}
			if(selected_unit == unit) {
				found = 1; // Now we can take the next unit
			}
			unit_list=unit_list->next;
		}
		unit_list=*defense;
	}

	/* If none unit found after the selected one, take the first available*/
	select_unit(game);
}

static void cb_move_unit(void * arg)
{
	int cost;
	unit_t *unit;

	if(selected_unit==NULL) {
		return;
	}

	if(wait_anim) {
		return;
	}

	/* test if the tile is free */
	unit = get_unit_at_pos(cur_gx,cur_gy);
	if( unit == NULL) {
		cost = path_cost(selected_unit->gx,selected_unit->gy,cur_gx,cur_gy);
		/* Test if the target tile is not too far */
		if( selected_unit->move >= cost ) {
			selected_unit->move -= cost;

			selected_unit->orient = get_orient(cur_gx-selected_unit->gx, cur_gy-selected_unit->gy);
			selected_unit->dest_gx = cur_gx;
			selected_unit->dest_gy = cur_gy;
			selected_unit->anim_timer = SDL_GetTicks();
			selected_unit=NULL;
		}
	}
	/* tile is occupied */
	else {
		/* check if it is a foe */
		if(unit->owner != selected_unit->owner) {
		/* test is we are close to the target */
			if( abs(unit->gx - selected_unit->gx) < 2 &&
				abs(unit->gy - selected_unit->gy) < 2 ) {
		/* test if we have still move point for this turn */
				if( selected_unit->move > 0 ) {
					selected_unit->orient = get_orient(cur_gx-selected_unit->gx, cur_gy-selected_unit->gy);
					unit->orient = get_orient(selected_unit->gx-cur_gx, selected_unit->gy-cur_gy);
					unit_attack(selected_unit,unit);
					selected_unit=NULL;
				}
			}
		}
	}
}

static int draw_ground()
{
	int i;
	int j;
	int num_item = 0;
	int screen_x;
	int screen_y;

	for(i=0;i<GRID_WIDTH;i++) {
		for(j=0;j<GRID_HEIGHT;j++) {
			item_init(&item_ground[num_item]);
			grid_to_screen(i, j, &screen_x, &screen_y);
			item_set_frame(&item_ground[num_item],screen_x,screen_y,
				&grass[0]);
			item_set_over(&item_ground[num_item],cb_tile_highlight,&item_ground[num_item]);
			item_set_click_left(&item_ground[num_item],cb_move_unit,NULL);
			num_item++;
		}
	}

	return num_item;

}

static void unit_move_init()
{
	unit_t * unit;
        unit_list_t * cur_unit;
	int i;
	
	cur_unit = *attack;
	/* Do it once for attackers, once for the defenders */
	for( i = 0; i < 2 ; i++) {
		while( cur_unit ) {
			unit = (unit_t*)cur_unit->data;
			unit->move = unit->def->move;
			cur_unit = cur_unit->next;
		}
		cur_unit = *defense;
	}
}

static void init_unit()
{
	unit_t * unit;
	unit_list_t * cur_unit;
	int i;
	int unit_x;
	int unit_y;
	int orient;

	/* Initial position of units on the combat map */
	/* Do it once for attackers, once for the defenders */
	cur_unit = *attack;
	unit_x = 10;
	unit_y = 15;
	orient = D_N;
	for( i = 0; i < 2 ; i++) {
		while( cur_unit ) {
			unit = (unit_t*)cur_unit->data;
			unit->gx=unit_x;
			unit->gy=unit_y;
			unit->anim_attack=0;
			unit->dest_gx=unit->gx;
			unit->dest_gy=unit->gy;
			unit->anim_x = 0;
			unit->anim_y = 0;
			unit->anim_timer=0;
			unit->orient=orient;
			unit_x++;
			cur_unit = cur_unit->next;
		}
		cur_unit = *defense;
		unit_x = 10;
		unit_y = 7;
		orient = D_S;
	}

	unit_move_init();
}

static int draw_unit(game_t * game)
{
	int num_item=0;
	unit_t * unit;
	unit_list_t * cur_unit;
	int num_unit = 0;
	int i;
	int j;
	int screen_x;
	int screen_y;
	int tile_height = grass[0].frame[0]->h;
	int x;
	int y;

	/* Draw highlight on selected unit */
	item_init(&item_highlight_unit);
	if(selected_unit) {
		grid_to_screen(selected_unit->gx,selected_unit->gy,&screen_x,&screen_y);
		item_set_frame(&item_highlight_unit,screen_x,screen_y,&city[ANI_HIGHLIGHT_RED]);
	}

	/* Draw unit form back to front */
	for( y=0; y<GRID_HEIGHT ; y++) {
		/* Draw unit form right to left */
		for( x=GRID_WIDTH-1; x>=0 ; x--) {
			cur_unit = *attack;
			/* Do it once for attackers, once for defenders */
			for( i = 0; i < 2 ; i++) {
				num_unit=0;
				while( cur_unit ) {
					unit = (unit_t*)cur_unit->data;
					if(unit->gx != x || unit->gy != y) {
						cur_unit = cur_unit->next;
						continue;
					}
					for( j=0; j<unit->fig; j++) {
						item_unit = (item_t*)realloc(item_unit,sizeof(item_t)*(num_item+1));
						item_init(&item_unit[num_item]);
						grid_to_screen(unit->gx,unit->gy,&screen_x,&screen_y);

						// apply animation
						screen_x+=unit->anim_x;
						screen_y+=unit->anim_y;

						// figures coordinate in the case
						screen_x+=formation_x[unit->def->num_figures-1][j];
						screen_y+=formation_y[unit->def->num_figures-1][j];

						/* Adjust the position with regards of the size of the sprite */
						screen_y = screen_y - fig[unit->type*D_NUM+unit->orient].frame[0]->h + tile_height;

						/* Animate the sprite if he can fly or if he is moving */
						if( unit->anim_attack == 0 ) {
							if( (unit->def->move_flag & MOVE_FLY) || unit->anim_x || unit->anim_y ) {
								item_set_anim(&item_unit[num_item],screen_x,screen_y,&fig[unit->type*D_NUM+unit->orient]);
								item_set_anim_start(&item_unit[num_item],0);
								item_set_anim_end(&item_unit[num_item],2);
							}
							else {
								item_set_frame(&item_unit[num_item],screen_x,screen_y,&fig[unit->type*D_NUM+unit->orient]);
								item_set_frame_normal(&item_unit[num_item],0);

							}
						}
						/* Animate the sprite while attacking */
						else {
							item_set_anim(&item_unit[num_item],screen_x,screen_y,&fig[unit->type*D_NUM+unit->orient]);
							item_set_anim_start(&item_unit[num_item],2);
							item_set_anim_end(&item_unit[num_item],4);
						}
						unit->item = &item_unit[num_item];
						num_item++;
					}
					num_unit++;
					cur_unit = cur_unit->next;
				}
				cur_unit = *defense;
			}
		}
	}

	return num_item;
}

static void ai_play()
{
	unit_t * unit;
	unit_t * enemy;
        unit_list_t * cur_unit;
        unit_list_t * opp_unit;
	int i;
	int gx;
	int gy;

	if(wait_anim) {
		return;
	}

	cur_unit = *attack;
	opp_unit = *defense;
	/* Do it once for attackers, once for the defenders */
	for( i = 0; i < 2 ; i++) {
		while( cur_unit ) {
			unit = (unit_t*)cur_unit->data;
			/* Skip player's units */
			if(unit->owner) {
				cur_unit = cur_unit->next;
				continue;
			}

			/* Ultra basic AI: move to the first opponent unit */
			if(unit->move > 0 ) {
				enemy = (unit_t*)opp_unit->data;
				gx=unit->gx;
				gy=unit->gy;
				unit->move-=2;
printf("%s = %d\n",exe_get_unit_name(unit->type),unit->move);

				if( abs(enemy->gx- unit->gx) > 0 ) {
					if( enemy->gx < unit->gx ) {
						gx = unit->gx-1;
					}
					else {
						gx = unit->gx+1;
					}
				}

				if( abs(enemy->gy- unit->gy) > 0 ) {
					if( enemy->gy < unit->gy ) {
						gy = unit->gy-1;
					}
					else {
						gy = unit->gy+1;
					}
				}

				unit->orient = get_orient(gx-unit->gx,gy-unit->gy);

				enemy = get_unit_at_pos(gx,gy);
				if( enemy != NULL ) {
					if( enemy->owner != unit->owner ) {	
						unit_attack(unit,enemy);
					}
					return;
				}

				unit->dest_gx=gx;
				unit->dest_gy=gy;
				unit->anim_timer = SDL_GetTicks();

				wait_anim = 1;
				return;
			}
			cur_unit = cur_unit->next;
		}
		cur_unit = *defense;
		opp_unit = *attack;
	}

	return;
}

static void anim_unit(game_t* game)
{
	unit_t * unit;
        unit_list_t * cur_unit;
	int i;
	int x;
	int y;
	int dx;
	int dy;
	int timer;
	
	cur_unit = *attack;
	/* Do it once for attackers, once for defenders */
	for( i = 0; i < 2 ; i++) {
		while( cur_unit ) {
			unit = (unit_t*)cur_unit->data;
			/* Does this unit need animation ? */
			if( unit->gx == unit->dest_gx && unit->gy == unit->dest_gy && unit->anim_attack == 0) {
				cur_unit = cur_unit->next;
				continue;
			}

			redraw_unit = 1;

			/* Move animation */
			if( unit->anim_attack == 0 ) {
				grid_to_screen(unit->gx,unit->gy,&x,&y);
				grid_to_screen(unit->dest_gx,unit->dest_gy,&dx,&dy);
				timer = SDL_GetTicks() - unit->anim_timer;	
				/* End of animation */
				if( timer > ANIM_DURATION) {
					unit->gx=unit->dest_gx;
					unit->gy=unit->dest_gy;
					unit->anim_x = 0;
					unit->anim_y = 0;
					select_unit(game);
					wait_anim = 0;
					cur_unit = cur_unit->next;
					continue;
				}

				/* 10000 to avoid calculation rounding */
				unit->anim_x = ((dx-x)*((timer*10000)/ANIM_DURATION))/10000;
				unit->anim_y = ((dy-y)*((timer*10000)/ANIM_DURATION))/10000;
			}
			/* Attack animation: */
			else {
				timer = SDL_GetTicks() - unit->anim_timer;	
				/* End of animation */
				if( timer > ANIM_DURATION) {
				/* let draw_unit choose the right animation */
					unit->anim_attack = 0;
					select_unit(game);
					wait_anim = 0;
		/* Beware: cur_unit may be deleted by check_victory()
		So we set the next unit before the call to check_victory() */
					cur_unit = cur_unit->next;
					check_victory();
					continue;
				}
			}

			cur_unit = cur_unit->next;
		}
		cur_unit = *defense;
	}

}

static void cb_button_done(void * arg)
{
	rest_unit = 1;
}

static void cb_button_wait(void * arg)
{
	game_t * game = (game_t *)arg;

	select_next_unit(game);
	redraw_unit = 1;
}

void screen_combat(game_t * game, unit_list_t ** list_attack, unit_list_t ** list_defense, int x, int y, int side)
{
	SDL_Event event;
        int i;
        int j;
	int num_grd;
	int num_deco;
	int num_unit;
	char filename[64];
	LBXAnimation_t* anim_ptr;

	end_screen = -1;

	sdl_keyboard_init(NULL,NULL);

	selected_unit = NULL;
	redraw_unit=1;
	rest_unit=1;
	cur_gx=-9999;
	cur_gy=-9999;
	attack=list_attack;
	defense=list_defense;
	wait_anim=0;

	/* Load resource */
	if(anim==NULL) {
		anim = load_graphics("BACKGRND.LBX");
		if(anim == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	/* Load resource */
	if(grass==NULL) {
		grass = load_graphics("CMBGRASS.LBX");
		if(grass == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	if(city==NULL) {
		city= load_graphics("CMBTCITY.LBX");
		if(city == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	if(pix==NULL) {
		pix= load_graphics("COMPIX.LBX");
		if(pix == NULL) {
			exit(EXIT_FAILURE);
		}
	}

	if(fig==NULL) {
		for(j=0;j<9;j++) {
			sprintf(filename,"FIGURES%d.LBX",j+1);
			figure[j]= load_graphics(filename);
			if(figure[j] == NULL) {
				exit(EXIT_FAILURE);
			}
		}
		for(j=9;j<16;j++) {
			sprintf(filename,"FIGURE%d.LBX",j+1);
			figure[j]= load_graphics(filename);
			if(figure[j] == NULL) {
				exit(EXIT_FAILURE);
			}
		}

		/* Copy all figure in the same array */
		j=0;
                for(i=0;i<16;i++) {
			anim_ptr = figure[i];
                        while(anim_ptr->num_frame!=0) {
                                fig = (LBXAnimation_t*)realloc(fig,(j+1)*sizeof(LBXAnimation_t));
                                memcpy(&fig[j],anim_ptr,sizeof(LBXAnimation_t));
                                anim_ptr++;
                                j++;
                        }
                }
	}

	j=0;
	item_init(&item[j]);
	item_set_frame(&item[j],0,164,&anim[ANI_BG]);
	j++;
	/* Wait button */
	item_init(&item[j]);
	item_set_frame(&item[j],170,168,&pix[ANI_WAIT]);
	item_set_frame_click(&item[j],1);
	item_set_click_left(&item[j],cb_button_wait,(void*)game);
	j++;
	/* Done button */
	item_init(&item[j]);
	item_set_frame(&item[j],170,188,&pix[ANI_DONE]);
	item_set_frame_click(&item[j],1);
	item_set_click_left(&item[j],cb_button_done,NULL);
	j++;

	num_grd = draw_ground();
	num_deco = draw_deco(game,x,y,side);

	init_unit();

	/* Main loop */
	while( end_screen == -1)
	{
		if(redraw_unit) {
			num_unit = draw_unit(game);
			redraw_unit=0;
		}
		if(rest_unit) {
			unit_move_init();
			rest_unit=0;
		}

		while (SDL_PollEvent(&event))
		{
			if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) {
				end_screen = 1;
			}
			sdl_screen_manager(&event);
			sdl_mouse_manager(&event,item_ground,num_grd);
			sdl_mouse_manager(&event,item,ITM_NUM);
			sdl_keyboard_manager(&event);
		}

		opengl_blit_item_list(item_ground,num_grd);
		opengl_blit_item_list(item_deco,num_deco);
		opengl_blit_item_list(&item_highlight_tile,1);
		opengl_blit_item_list(&item_highlight_unit,1);
		opengl_blit_item_list(item_unit,num_unit);
		opengl_blit_item_list(item,ITM_NUM);

		opengl_blit_to_screen();

		sdl_loop_manager();

		ai_play();
		anim_unit(game);
	}

	return;
}
