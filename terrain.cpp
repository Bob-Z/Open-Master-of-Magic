/*
   Free MoM is a free implementation of classical game Mater of Magic.
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
#include <libnoise/noise.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>
#include "game.h"

#define ELEV_MAP_W	DEF_MAP_W+1
#define ELEV_MAP_H	DEF_MAP_H+1

#define COAST_START	2
#define COAST_NUM	160

#define B_NW	0x00000001
#define B_N	0x00000010
#define B_NE	0x00000100
#define B_W	0x00001000
#define B_E	0x00010000
#define B_SW	0x00100000
#define B_S	0x01000000
#define B_SE	0x10000000

static int test_border(int ** tile,int s_x, int s_y)
{
	int w; // west
	int n; // north
	int e; // east
	int s; // south
	int myr = 0;
	int res = 0;
	int tile_type[4] = {T_GRASS,T_HILL,T_FOREST,T_MOUNTAIN};
	unsigned int i;

	/* Only water for now */
	if( (tile[s_x][s_y]%T_MYRRAN) != T_WATER ) {
		return 0;
	}

	if( tile[s_x][s_y] >= T_MYRRAN) {
		myr = T_MYRRAN;
	}

	/* left border */
	w = s_x -1;
	if( w < 0 ) w=DEF_MAP_W-1;

	e = (s_x+1)%DEF_MAP_W;

	n = s_y -1;
	s = s_y+1;

	for( i=0;i<4;i++) {
		if(n>=0 ) {
			if( tile[w][n] == tile_type[i]+myr ) {
				res |= B_NW;
			}
			if( tile[s_x][n] == tile_type[i]+myr ) {
				res |= B_N;
			}
			if( tile[e][n] == tile_type[i]+myr ) {
				res |= B_NE;
			}
		}

		if( tile[w][s_y] == tile_type[i]+myr ) {
			res |= B_W;
		}
		if( tile[e][s_y] == tile_type[i]+myr ) {
			res |= B_E;
		}
		if(s<DEF_MAP_H) {
			if( tile[w][s] == tile_type[i]+myr ) {
				res |= B_SW;
			}
			if( tile[s_x][s] == tile_type[i]+myr ) {
				res |= B_S;
			}
			if( tile[e][s] == tile_type[i]+myr ) {
				res |= B_SE;
			}	
		}
	}

	if( res == B_SE) {
		res = B_SE;
	}

	return res;
}

static int get_border_tile(int code,int tile_type)
{
	int index = COAST_START;
	int i;
	int code_array[COAST_NUM] = {B_SE,B_SE+B_S,B_SE+B_S+B_SW,B_S+B_SW,B_SW,B_SW+B_SE,B_SW+B_NE,B_NW+B_SW,
/*10*/B_E+B_SE,B_S,B_W+B_SW,B_NW+B_NE,B_NW+B_SE,B_NE+B_SE,B_NE+B_E+B_SE,B_E,B_NW+B_N+B_NE+B_W+B_E+B_SW+B_S+B_SE,B_W,
/*20*/B_NW+B_W+B_SW,B_E+B_NE,B_N,B_W+B_NW,B_NW+B_NE+B_SE,B_SW+B_NE+B_SE,B_SW+B_NW+B_SE,B_NE,B_NE+B_N,B_NE+B_N+B_NW,
/*30*/B_NW+B_N,B_NW,B_NW+B_NE+B_SW,B_NW+B_NE+B_SW+B_SE,B_NW+B_N+B_W,B_NW+B_N+B_W+B_NE,B_NW+B_N+B_W+B_SW,B_NW+B_N+B_W+B_SW+B_NE,B_E+B_SE+B_S,B_E+B_SE+B_S+B_NE,
/*40*/B_E+B_SE+B_S+B_SW, B_E+B_SE+B_S+B_SW+B_NE, B_N+B_NE+B_E, B_N+B_NE+B_E+B_SE, B_N+B_NE+B_E+B_NW, B_N+B_NE+B_E+B_NW+B_SE, B_W+B_SW+B_S, B_W+B_SW+B_S+B_SE, B_NW+B_W+B_SW+B_S, B_W+B_SW+B_S+B_NW+B_SE,
/*50*/B_NW+B_N+B_NE+B_SW+B_S+B_SE, B_NW+B_N+B_NE+B_SW+B_S, B_NW+B_N+B_NE+B_S+B_SE, B_NW+B_N+B_NE+B_S, B_N+B_SW+B_S+B_SE, B_NW+B_N+B_SW+B_S, B_N+B_S+B_SE, B_N+B_S+B_SW, B_N+B_NE+B_SW+B_S+B_SE, B_N+B_NE+B_SW+B_S, 
/*60*/B_N+B_NE+B_S+B_SE, B_N+B_NE+B_S, B_NW+B_N+B_SW+B_S+B_SE, B_NW+B_N+B_S, B_NW+B_N+B_S+B_SE, B_N+B_S, B_NW+B_W+B_E+B_SW, B_NW+B_W+B_E+B_SW+B_SE, B_NE+B_W+B_E+B_SW+B_SE, B_NW+B_NE+B_W+B_E+B_SW,
/*70*/B_NE+B_W+B_E+B_SW, B_NW+B_W+B_E+B_SE,B_NW+B_NE+B_W+B_E,B_NW+B_NE+B_W+B_E+B_SE,B_W+B_E+B_SW,B_W+B_E+B_SW+B_SE,B_W+B_E,B_NW+B_NE+B_W+B_E+B_SW+B_SE,B_W+B_E+B_SE,B_NW+B_W+B_E,
/*80*/B_NE+B_W+B_E, B_NE+B_W+B_E+B_SE, B_W+B_E+B_SW+B_S+B_SE, B_NW+B_N+B_W+B_SW+B_S, B_NW+B_N+B_NE+B_W+B_E, B_N+B_NE+B_E+B_S+B_SE, B_NW+B_W+B_E+B_SW+B_S+B_SE, B_NW+B_N+B_NE+B_W+B_SW+B_S, B_NW+B_N+B_NE+B_W+B_E+B_SE, B_N+B_NE+B_E+B_SW+B_S+B_SE,
/*90*/B_W+B_NE+B_E+B_SW+B_S+B_SE,B_NW+B_N+B_W+B_SW+B_S+B_SE,B_NW+B_N+B_NE+B_W+B_E+B_SW,B_NW+B_N+B_NE+B_E+B_S+B_SE,B_NW+B_NE+B_W+B_E+B_SW+B_S+B_SE,B_NW+B_N+B_NE+B_W+B_SW+B_S+B_SE,B_NW+B_N+B_NE+B_W+B_E+B_SE+B_SW,B_NW+B_N+B_NE+B_E+B_SW+B_S+B_SE,B_NW+B_NE+B_E+B_SE,B_NW+B_NE+B_E,
/*100*/B_NW+B_E+B_SE,B_NW+B_E,B_NW+B_NE+B_E+B_SW+B_SE,B_NW+B_NE+B_E+B_SW,B_NW+B_E+B_SE+B_SW,B_NW+B_E+B_SW,B_SW+B_NE+B_E+B_SE,B_SW+B_NE+B_E,B_SW+B_E+B_SE,B_SW+B_E,
/*110*/B_NW+B_SW+B_S+B_SE,B_NW+B_NE+B_SW+B_S+B_SE,B_NE+B_SW+B_S+B_SE,B_NW+B_S+B_SE,B_NW+B_NE+B_S+B_SE,B_NE+B_S+B_SE,B_NW+B_SW+B_S,B_NW+B_NE+B_SW+B_S,B_NE+B_SW+B_S,B_NW+B_S,
/*120*/B_NW+B_NE+B_S,B_NE+B_S,B_NE+B_W,B_NW+B_NE+B_W,B_NE+B_W+B_SW,B_NW+B_NE+B_W+B_SW, B_NE+B_W+B_SE, B_NW+B_NE+B_W+B_SE, B_NE+B_W+B_SW+B_SE, B_NW+B_NE+B_W+B_SW+B_SE, 
/*130*/B_W+B_SE, B_NW+B_W+B_SE, B_W+B_SW+B_SE, B_NW+B_W+B_SW+B_SE, B_N+B_SW, B_N+B_SW+B_SE, B_N+B_SE, B_NW+B_N+B_SW, B_NW+B_N+B_SW+B_SE, B_NW+B_N+B_SE, 
/*140*/ B_N+B_NE+B_SW, B_N+B_NE+B_SW+B_SE, B_N+B_NE+B_SE, B_NW+B_N+B_NE+B_SW, B_NW+B_N+B_NE+B_SW+B_SE, B_NW+B_N+B_NE+B_SE, B_NW+B_N+B_W+B_SE, B_NW+B_N+B_NE+B_W+B_SE, B_NW+B_N+B_W+B_SW+B_SE, B_NW+B_N+B_NE+B_W+B_SW+B_SE, 
/*150*/B_NW+B_E+B_S+B_SE, B_NW+B_NE+B_E+B_S+B_SE, B_NW+B_E+B_SW+B_S+B_SE, B_NW+B_NE+B_E+B_SW+B_S+B_SE, B_N+B_NE+B_E+B_SW, B_N+B_NE+B_E+B_SW+B_SE, B_NW+B_N+B_NE+B_E+B_SW, B_NW+B_N+B_NE+B_E+B_SW+B_SE, B_NE+B_W+B_SW+B_S, B_NE+B_W+B_SW+B_S+B_SE, 
/*160*/ B_NW+B_NE+B_W+B_SW+B_S, B_NW+B_NE+B_W+B_SW+B_S+B_SE };

#if 0
	int j;
	for(i=0;i<159;i++) {
		for(j=i+1;j<160;j++) {
			if(code_array[i]==code_array[j]) {
				printf("%d = %d\n",i+2,j+2);
			}
		}
	}
	exit(0);
#endif

	if( tile_type >= T_MYRRAN ) {
		index = index+T_MYRRAN;
	}

	for(i=0;i<COAST_NUM;i++) {
		if( code == code_array[i] ) {
			return i+index;
		}
	}

	return tile_type;
}

static void draw_coast(game_t * game)
{
	int map[NUM_SIDE][DEF_MAP_W][DEF_MAP_H];
	int x;
	int y;
	int side;
	int ** tile;
	int code;

	for( side=0 ; side<NUM_SIDE ; side++) {
		tile = game->map->side[side].tile;
		for( x=0 ; x<DEF_MAP_W ; x++ ) {
			for( y=0 ; y<DEF_MAP_H ; y++ ) {
				code=test_border(tile,x,y);
				map[side][x][y] = get_border_tile(code,tile[x][y]);
				if(map[side][x][y]==T_WATER+side*T_MYRRAN) {
					printf("missing %x\n",code);
				}
			}
		}
	}
	

	/* Copy new data into game structure */
	for( side=0 ; side<NUM_SIDE ; side++) {
		for( x=0 ; x<DEF_MAP_W ; x++ ) {
			for( y=0 ; y<DEF_MAP_H ; y++ ) {
				game->map->side[side].tile[x][y] = map[side][x][y];
			}
		}
	}
}

static void add_moving_water(game_t * game)
{
	int side;
	int x;
	int y;

	for( side=0 ; side<NUM_SIDE ; side++) {
		for( x=0 ; x<DEF_MAP_W ; x++ ) {
			for( y=0 ; y<DEF_MAP_H ; y++ ) {
				if( game->map->side[side].tile[x][y] == T_WATER) {
					if(!(rand()%10)) {
						game->map->side[side].tile[x][y]=T_MOV_WATER;
					}
				}
			}
		}
	}
}

static void create_map(game_t * game)
{
#define NUM_LEVEL	5
	int i,j,k;
	// From -1 to 1
	double level[NUM_LEVEL] = {0.0,0.3,0.5,0.7,1.0};
	int tile[NUM_LEVEL] = {T_WATER,T_GRASS,T_FOREST,T_HILL,T_MOUNTAIN};
	int li;
	double map[ELEV_MAP_W][ELEV_MAP_H];
	double min=0;
	double max=0;
	double middle=0;
	double ratio=0;


	for(k=0;k<NUM_SIDE;k++) {
		/* Generate map */
		min = 0.0;
		max = 0.0;

		noise::module::Perlin noise;
		noise.SetFrequency (2.0);
		noise.SetSeed((int)time(NULL)+k);

		noise::model::Cylinder height_map(noise);

		for(i=0;i<DEF_MAP_W;i++) {
			for(j=0;j<DEF_MAP_H;j++) {
				map[i][j]=height_map.GetValue(
						360.0*((double)DEF_MAP_W-(double)i)/(double)DEF_MAP_W,
						/* Since the cylinder is 1.0 radius, we are using 2*Pi
						   radian of data around the cylinder.
						   To avoid distorsion, we have to use 2*Pi data alongside
						   the cylinder */
						((double)DEF_MAP_H-(double)j)/(double)DEF_MAP_H * (2.0*M_PI));

				if(map[i][j] < min) {
					min = map[i][j];
				}
				if(map[i][j] > max) {
					max = map[i][j];
				}
			}
		}

		middle = (max+min)/2.0;
		ratio = max - middle;

		for(i=0;i<DEF_MAP_W;i++) {
			for(j=0;j<DEF_MAP_H;j++) {
				li=0;
				while(1) {
					if( map[i][j] <= (level[li]*ratio)+middle) {
						game->map->side[k].tile[i][j] = k*T_MYRRAN + tile[li];	
						break;
					}
					li++;
					/* This may happen because of calculation rounding */
					if(li==NUM_LEVEL) {
						game->map->side[k].tile[i][j] = k*T_MYRRAN + tile[NUM_LEVEL-1];
						break;
					}
				}
			}
		}
	}
}

/* return a pointer to the encounter at given coordinate or NULL if none */
encounter_t * is_encounter_present(game_t * game,int x,int y, int side)
{
	llist_t * list = game->encounter;
	encounter_t * enc;

	while(list != NULL) {
		enc = (encounter_t *)list->data;
		if( enc->x == x && enc->y == y && enc->side == side) {
			return enc;
		}

		list = list->next;
	}

	return NULL;
}

/* return a pointer to the city at given coordinate or NULL if none */
city_t * is_city_present(game_t * game,int x,int y, int side)
{
	llist_t * list = game->encounter;
	city_t * city;
	int i;


	for(i=-1;i<game->opponent_num;i++) {
		list = game->wiz[i].city;
		while(list != NULL) {
			city = (city_t *)list->data;
			if( city->x == x && city->y == y && city->side == side){
				return city;
			}

			list = list->next;
		}
	}

	return NULL;
}

int is_ground(int tile_type)
{
#if 0
	if((tile_type%T_MYRRAN) == T_GRASS ||
		(tile_type%T_MYRRAN) == T_HILL ||
		(tile_type%T_MYRRAN) == T_FOREST ||
		(tile_type%T_MYRRAN) == T_MOUNTAIN ) {
		return 1;
	}
#endif

	if( (tile_type%T_MYRRAN) >= COAST_START+COAST_NUM ) {
		return 1;
	}

	return 0;
}

static void create_encounters(game_t * game)
{
#define UNITS	3
	int i,j,k;
	int num_enc;
	int x,y;
	encounter_t * encounter;
	unit_t * unit[UNITS];

	num_enc = (int)( (double)game->map->width * (double)game->map->height * ENCOUNTER_RATIO / 100.0 );

	printf("Creating %d encounters\n",num_enc);

	for(k=0;k<NUM_SIDE;k++) {
		for(i=0;i<num_enc;i++) {
			/* search a potential site without previous encounter or city */
			do {
				x=rand()%game->map->width;
				y=rand()%game->map->height;
			} while ( !is_ground(game->map->side[k].tile[x][y]) || is_encounter_present(game,x,y,k) || is_city_present(game,x,y,k));

			encounter = (encounter_t*)malloc(sizeof(encounter_t));
			encounter->type = ENC_TOWER + ( rand()%(ENC_NUM-ENC_TOWER) );
			encounter->x = x;
			encounter->y = y;
			encounter->side = k;
			encounter->unit = NULL;

			for(j=0;j<UNITS;j++) {
				unit[j] = new unit_t;
				unit[j]->type = CHAOS_UNIT + rand()%43;
				unit[j]->side=k;
				unit[j]->def=exe_get_unit_def(unit[j]->type);
				unit[j]->hp = unit[j]->def->hp;
				unit[j]->fig = unit[j]->def->num_figures;
				unit[j]->owner=NULL;
				printf("Creating encounter with unit: %s (%d)\n",exe_get_unit_name(unit[j]->type),unit[j]->type);

				encounter->unit = add_llist(encounter->unit,unit[j]);
			}
			printf("\n");
			game->encounter = add_llist(game->encounter,encounter);
		}
	}
}

/* It seems that there is no graphics allowing "diagonal coast".
So we delete all potential "diagonal coast" here */
static void remove_isolated_water(game_t * game)
{
	int i,j,k;
	int clean = 0;

	while( !clean) {
		clean = 1;
		for(k=0;k<NUM_SIDE;k++) {
			for(i=DEF_MAP_W;i<2*DEF_MAP_W;i++) {
				for(j=1;j<DEF_MAP_H-1;j++) {
					if(game->map->side[k].tile[(i)%DEF_MAP_W][j]==T_WATER+k*T_MYRRAN) {

						/* upper left */
						if( game->map->side[k].tile[(i-1)%DEF_MAP_W][j-1] == T_WATER+k*T_MYRRAN &&
							is_ground(game->map->side[k].tile[(i-1)%DEF_MAP_W][j]) &&
							is_ground(game->map->side[k].tile[(i)%DEF_MAP_W][j-1]) ){
								game->map->side[k].tile[(i-1)%DEF_MAP_W][j-1] = T_GRASS+k*T_MYRRAN;
								clean = 0;
							}
						/* upper right */
						if( game->map->side[k].tile[(i+1)%DEF_MAP_W][j-1] == T_WATER+k*T_MYRRAN &&
							is_ground(game->map->side[k].tile[(i+1)%DEF_MAP_W][j]) &&
							is_ground(game->map->side[k].tile[(i)%DEF_MAP_W][j-1]) ){
								game->map->side[k].tile[(i+1)%DEF_MAP_W][j-1] = T_GRASS+k*T_MYRRAN;
								clean = 0;
						}
						/* lower left */
						if( game->map->side[k].tile[(i-1)%DEF_MAP_W][j+1] == T_WATER+k*T_MYRRAN &&
							is_ground(game->map->side[k].tile[(i-1)%DEF_MAP_W][j]) &&
							is_ground(game->map->side[k].tile[(i)%DEF_MAP_W][j+1]) ){
								game->map->side[k].tile[(i-1)%DEF_MAP_W][j+1] = T_GRASS+k*T_MYRRAN;
								clean = 0;
						}
						/* lower right */
						if( game->map->side[k].tile[(i+1)%DEF_MAP_W][j+1] == T_WATER+k*T_MYRRAN &&
							is_ground(game->map->side[k].tile[(i+1)%DEF_MAP_W][j]) &&
							is_ground(game->map->side[k].tile[(i)%DEF_MAP_W][j+1]) ){
								game->map->side[k].tile[(i+1)%DEF_MAP_W][j+1] = T_GRASS+k*T_MYRRAN;
								clean = 0;
						}
					}
				}
			}
		}
	}
}

static void create_terrain(game_t * game)
{
	create_map(game);
	remove_isolated_water(game);
	draw_coast(game);
	add_moving_water(game);

	create_encounters(game);
}

void terrain_generate(int width, int height, game_t * game)
{
	int i;
	int j;

	srand( (unsigned)time(NULL) );

	/* Init map */
	game->map = (map_t*)malloc(sizeof(map_t));
	game->map->width=width;
	game->map->height=height;

	for(i=0;i<NUM_SIDE;i++) {
		game->map->side[i].tile = (int **)malloc(sizeof(int*) * width);
		for(j=0;j<width;j++) {
			game->map->side[i].tile[j] = (int *)malloc(sizeof(int) * height);
		}
		game->encounter=NULL;
	}

	create_terrain(game);
}
