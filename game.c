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

extern const char * data_path;

char * retort_name[RT_NUM]= {
	"Alchemy",
	"Warlord",
	"Channeler",
	"Archmage",
	"Artificier",
	"Conjurer",
	"Sage Master",
	"Myrran",
	"Divine Power",
	"Infernal Power",
	"Famous",
	"Runemaster",
	"Charismatic",
	"Chaos Mastery",
	"Nature Mastery",
	"Sorcery Mastery",
	"Mana Focusing",
	"Node Mastery"
};

wizard_t default_wizard[WIZARD_NUM]= {
	{"Merlin",{5,0,5,0,0},SET_RT(RT_SAGE_MASTER),0},
	{"Raven",{0,6,5,0,0},0,0},
	{"Sharee",{0,0,0,5,5},SET_RT(RT_CONJURER),0},
	{"Lo Pan",{0,5,0,0,5},SET_RT(RT_CHANNELER),0},
	{"Jafar",{0,10,0,0,0},SET_RT(RT_ALCHEMY),0},
	{"Oberic",{0,0,5,0,5},SET_RT(RT_MANA_FOCUSING),0},
	{"Rjak",{0,0,0,9,0},SET_RT(RT_INFERNAL_POWER),0},
	{"Sss'ra",{4,0,0,0,4},SET_RT(RT_MYRRAN),0},
	{"Tauron",{0,0,0,0,10},SET_RT(RT_CHAOS_MASTERY),0},
	{"Freya",{0,0,10,0,0},SET_RT(RT_NATURE_MASTERY),0},
	{"Horus",{5,5,0,0,0},SET_RT(RT_ARCHMAGE),0},
	{"Ariel",{10,0,0,0,0},SET_RT(RT_CHARISMATIC),0},
	{"Tlaloc",{0,0,4,5,0},SET_RT(RT_WARLORD),0},
	{"Kali",{0,5,0,5,0},SET_RT(RT_ARTIFICER),0}
};

intptr_t arcanian_race[ARCANIAN_RACE_NUM] = {R_BARBARIAN,R_GNOLL,R_HALFLING,R_HIGH_ELF,R_HIGH_MAN,R_KLACKON,R_LIZARDMAN,R_NOMAD,R_ORC};
intptr_t myrran_race[MYRRAN_RACE_NUM] = {R_BEASTMAN,R_DARK_ELF,R_DRACONIAN,R_DWARVEN,R_TROLL};

int race_unit[R_NUM] = {BARBARIAN_UNIT,BEASTMAN_UNIT,DARK_ELF_UNIT,DRACONIAN_UNIT,DWARVEN_UNIT,GNOLL_UNIT,HALFLING_UNIT,HIGH_ELF_UNIT,HIGH_MAN_UNIT,KLACKON_UNIT,LIZARDMAN_UNIT,NOMAD_UNIT,ORC_UNIT,TROLL_UNIT,0,0,0,0,0,0,0};

LBXAnimation_t * load_graphics(SDL_Renderer * render,const char * filename)
{
	char path[2048];
	char lower_case[2048];
	int i=0;
	LBXAnimation_t * anim = NULL;

	while( filename[i] != 0 ) {
		lower_case[i] = tolower(filename[i]);
		i++;
	}
	lower_case[i]=0;

	if(data_path != NULL) {
		sprintf(path,"%s/%s",data_path,filename);
		anim = lbx_decode_image(render,path);
		if(anim != NULL) {
			return anim;
		}
		anim = lbx_decode_terrain(render,path);
		if(anim != NULL) {
			return anim;
		}

		/* try lower case */
		sprintf(path,"%s/%s",data_path,lower_case);
		anim = lbx_decode_image(render,path);
		if(anim != NULL) {
			return anim;
		}
		anim = lbx_decode_terrain(render,path);
		if(anim != NULL) {
			return anim;
		}
	}


	/* Try local data */
	sprintf(path,"./%s",filename);
	anim = lbx_decode_image(render,path);
	if(anim != NULL) {
		return anim;
	}
	anim = lbx_decode_terrain(render,path);
	if(anim != NULL) {
		return anim;
	}
	/* and with lowercase */
	sprintf(path,"./%s",lower_case);
	anim = lbx_decode_image(render,path);
	if(anim != NULL) {
		return anim;
	}
	anim = lbx_decode_terrain(render,path);
	if(anim != NULL) {
		return anim;
	}

	printf("Could not load %s\n",filename);

	return NULL;
}

/* add a unit to a group */
group_t * add_group(group_t * group, unit_t * unit)
{
	group_t * grp;
	unit_list_t * unit_list;
	if(group == NULL) {
		unit_list = add_llist(NULL,unit);
		grp = add_llist(group,unit_list);

		return grp;
	}

	group->data =  add_llist((llist_t*)group->data,unit);

	return group;
}

/* remove an unit from a group and return the first group pointer (may be NULL )*/
group_t * del_group(group_t * first, group_t * group, unit_list_t * unit)
{
	llist_t* first_unit_list;

	first_unit_list = (llist_t*)group->data;
	first_unit_list = del_llist(first_unit_list,unit);

	/* delete empty group */
	if( first_unit_list == NULL ) {
		return del_llist(first,group);
	}

	return first;
}
