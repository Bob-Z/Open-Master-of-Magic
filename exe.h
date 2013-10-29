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

#ifndef EXE_H
#define EXE_H

#include <inttypes.h>

#define MOVE_MERGE	0x80
#define MOVE_MOUNTAIN	0x40
#define MOVE_FOREST	0x20
#define MOVE_TELEPORT	0x10
#define MOVE_FLY	0x08
#define MOVE_SWIM	0x04
#define MOVE_SAIL	0x02
#define MOVE_CAVALRY	0x01

typedef struct { __attribute__((__packed__))
        uint16_t name_ptr;
        uint8_t melee_attack_str;
        uint8_t ranged_attack_str;
        uint8_t ranged_attack_type;
        uint8_t ranged_attack_num_shot;
        uint8_t plus_to_hit; // to hit=30 + plus_to_hit*10
        uint8_t defense;
        uint8_t resistance;
        uint8_t move; // in 1/2 MP      
        uint16_t cost;
        uint8_t summon_upkeep;
        uint8_t race;
        uint16_t extra;
        uint8_t hero_type;
        uint8_t zero1;
        uint8_t hp;
        uint8_t scout_range;
        uint8_t transport_capacity;
        uint8_t num_figures;
        uint8_t build_capability;
        uint8_t special_attack;
        uint8_t move_flag;
        uint8_t zero2;
        uint8_t immunity_flag;
        uint8_t attribute_flag1;
        uint8_t attribute_flag2;
        uint8_t zero3;
        uint8_t attribute_flag3;
        uint8_t attribute_flag4;
        uint8_t special_attack_flag_1;
        uint8_t special_attack_flag_2;
        uint8_t zero4;
        uint8_t zero5;
} def_unit_t;

typedef struct __attribute__((__packed__))
{
	uint16_t name_ptr;
	uint16_t num_prohibited_buildings;
	uint16_t prohibited_buildings[7];
	uint16_t outpost_growth_proba; //percent chance to grow in each turn
	uint16_t population_growth_modifier; //in units of 10 people per turn
	uint16_t house_picture; // (0 wood frame house, 1 tree house, 2 mud hut
} race_t;

char * exe_init(const char * file_name);
const char * exe_get_race_name(int index);
const char * exe_get_unit_name(int index);
def_unit_t * exe_get_unit_def(int index);
#endif
