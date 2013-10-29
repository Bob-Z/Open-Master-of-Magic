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

#ifndef GAME_H
#define GAME_H

#include "exe.h"
#include "llist.h"
#include "lbx.h"
#include "item.h"

#define FRAME_DELAY 10 //delay between TWO frame in millisecond

#define ANIM_DELAY 100 // default animation delay in millisecond

#define OPPONENT_MAX 10
#define UNIT_PER_TILE_MAX 9

#define DEF_MAP_W	25
#define DEF_MAP_H	10
#define NUM_SIDE	2
/* Percentage of the map tile to have encounters */
#define ENCOUNTER_RATIO	2.0

#define WIZARD_NUM	14
#define RACE_NUM	14

#define UNIT_BLINK_TIME	700

enum difficulty_level {
	DIFF_EASY,
	DIFF_NORMAL,
	DIFF_HARD,
	DIFF_IMPOSSIBLE,
	DIFF_NUM
};

enum land_size {
	SIZE_TINY,
	SIZE_MEDIUM,
	SIZE_LARGE,
	SIZE_NUM
};

enum magic {
	MAGIC_WEAK,
	MAGIC_NORMAL,
	MAGIC_POWERFUL,
	MAGIC_NUM
};

#define SET_RT(x)		(1<<x)
#define ADD_RT(x,y)		x|(1<<y)
#define DEL_RT(x,y)		x&(!(1<<y))
enum retort {RT_ALCHEMY,RT_WARLORD,RT_CHANNELER,RT_ARCHMAGE,RT_ARTIFICER,RT_CONJURER,RT_SAGE_MASTER,RT_MYRRAN,RT_DIVINE_POWER,RT_INFERNAL_POWER,RT_FAMOUS,RT_RUNEMASTER,RT_CHARISMATIC,RT_CHAOS_MASTERY,RT_NATURE_MASTERY,RT_SORCERY_MASTERY,RT_MANA_FOCUSING,RT_NODE_MASTERY,RT_NUM};

enum book {BOOK_LIFE,BOOK_SORCERY,BOOK_NATURE,BOOK_DEATH,BOOK_CHAOS,BOOK_NUM};

enum color {COL_BLUE,COL_GREEN,COL_PURPLE,COL_RED,COL_YELLOW,COL_NUM};

enum race { R_BARBARIAN,R_BEASTMAN,R_DARK_ELF,R_DRACONIAN,R_DWARVEN,R_GNOLL,R_HALFLING,R_HIGH_ELF,R_HIGH_MAN,R_KLACKON,R_LIZARDMAN,R_NOMAD,R_ORC,R_TROLL,R_GENERIC,R_ARCANE,R_NATURE,R_SORCERY,R_CHAOS,R_LIFE,R_DEATH,R_NUM};

enum encounter { ENC_TOWER=46,ENC_OWNED_TOWER,ENC_MOUND,ENC_TEMPLE,ENC_KEEP,ENC_RUINS,ENC_FALLEN_TEMPLE,ENC_NUM};

#define UNIT_TYPE	39
#define BARBARIAN_UNIT	UNIT_TYPE
#define BEASTMAN_UNIT	BARBARIAN_UNIT+7
#define DARK_ELF_UNIT	BEASTMAN_UNIT+11
#define DRACONIAN_UNIT	DARK_ELF_UNIT+9
#define DWARVEN_UNIT	DRACONIAN_UNIT+10
#define GNOLL_UNIT	DWARVEN_UNIT+7
#define HALFLING_UNIT	GNOLL_UNIT+6
#define HIGH_ELF_UNIT	HALFLING_UNIT+6
#define HIGH_MAN_UNIT	HIGH_ELF_UNIT+9
#define KLACKON_UNIT	HIGH_MAN_UNIT+10
#define LIZARDMAN_UNIT	KLACKON_UNIT+6
#define NOMAD_UNIT	LIZARDMAN_UNIT+7
#define ORC_UNIT	NOMAD_UNIT+10
#define TROLL_UNIT	ORC_UNIT+10
#define MAG_SPIRIT_UNIT	TROLL_UNIT+7
#define CHAOS_UNIT	MAG_SPIRIT_UNIT+1   /* 155 */
#define DEATH_UNIT	CHAOS_UNIT+10
#define LIFE_UNIT	DEATH_UNIT+10
#define NATURE_UNIT	LIFE_UNIT+4
#define SORCERY_UNIT	NATURE_UNIT+11
#define END_UNIT	SORCERY_UNIT+8


typedef struct unit_struct {
	int type;
	/* overall map position */
	int side;
	int x;
	int y;
	/* combat map position (grid position) */
	int gx;
	int gy;
	int anim_attack; // 1 if an attack anim is pending
	int dest_gx;
	int dest_gy;
	int anim_x;
	int anim_y;
	Uint32 anim_timer;
	int orient; 
	int move; /* remaining movement point */

	int hp; // current hp (max is in def)
	int fig; // current figures number (max is in def)
	def_unit_t * def;
	struct wizard_struct * owner;
	item_t * item; // Item associated to this unit
} unit_t;

typedef struct llist_struct group_t;
typedef struct llist_struct unit_list_t;

typedef struct {
	int population;
	int side;
	int x;
	int y;
	struct game_struct * game;
} city_t;

typedef struct {
	int type;
	int x;
	int y;
	int side;
	unit_list_t * unit;
} encounter_t;

typedef struct wizard_struct {
	char * name;
	int book[BOOK_NUM];
	int retort;
	int race;
	int color;
	int gold;
	int mana;
	struct llist_struct * group;
	struct llist_struct * city;
} wizard_t;

typedef struct {
	int ** tile;
} map_side_t;

typedef struct {
	int width;
	int height;
	map_side_t side[NUM_SIDE];
} map_t;

typedef struct game_struct {
	int difficulty_level;
	int opponent_num;
	int land_size;
	int magic;
	wizard_t * wiz;
	map_t * map;
	struct llist_struct * encounter;
} game_t;

#define ARCANIAN_RACE_NUM 9
#define MYRRAN_RACE_NUM 5

/* Tiles */
#define T_MYRRAN        0x2fa //762
#define T_WATER         0x0
#define T_GRASS         0xa2 //162
#define T_FOREST        0xa3
#define T_MOUNTAIN      0xa4
#define T_HILL          0xab
#define T_MOV_WATER     0x259 //601

LBXAnimation_t * load_graphics(const char * filename);
group_t * add_group(group_t * group, unit_t * unit);
group_t * del_group(group_t * first, group_t * group, unit_list_t * unit);
#endif
