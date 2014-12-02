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

#ifndef LBX_H
#define LBX_H

// http://rewiki.regengedanken.de/wiki/.LBX
//http://www.dragonsword.com/magic/eljay/SaveGam.html
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <inttypes.h>

#define ORIGINAL_GAME_WIDTH 320
#define ORIGINAL_GAME_HEIGHT 200
#define ORIGINAL_GAME_RATIO (4.0/3.0)

#define LBX_TYPE_UNKNOWN 0
#define LBX_TYPE_FONT 2
#define LBX_TYPE_STRING 5
#define LBX_TYPE_TERRAIN 6

#define LBX_FONT_CHAR_NUM 96

#define SHADOW_COLOR 232
#define LIGHT_COLOR 233

#define SOUND_MID	1
#define SOUND_VOC	2

#define SOUND_TMP	"/tmp/omom_sound.tmp"

enum font_name {
	LBX_FONT_TINY,
	LBX_FONT_SMALL,
	LBX_FONT_NORMAL,
	LBX_FONT_BIG,
	LBX_FONT_BIG_GOTH,
	LBX_FONT_HUGE_GOTH,
	LBX_FONT_MAGIC,
	LBX_FONT_BIG_MAGIC,
	LBX_FONT_NUM
};

#define LBX_FONT_FILE_NAME "FONTS.LBX"

#define LBX_ANIM_FLAGS_JUNCTION 0x100000 //Junction flag means that you add each frame to the frame before it making a composted image. and start from the begining on the first frame. That is how only the first frame of the big animations is large and all other frames just include the pixels that changed.
#define LBX_ANIM_FLAGS_INTERNAL_PALETTE 0x10000
#define LBX_ANIM_FLAGS_FUNCTIONAL_COLORS 0x1000 //Tells if the image has functional color. This color is used for effects like transparency or shading.
#define LBX_ANIM_FLAGS_TRANSPARENT    0x100 //Tells the image contains a transparent color (index 232 ?)
#define LBX_ANIM_FLAGS_NO_COMPRESSION 0x1

typedef enum res_type {
	LBX_T_SOUND,
	LBX_T_ARRAY,
	LBX_T_DATA,
	LBX_T_ANIM,
	LBX_T_FONT,
	LBX_T_TERRAIN
} res_type_t;

typedef struct __attribute__((__packed__))
{
	uint16_t numEntries;
	uint16_t magic;
	uint16_t reserved;
	uint16_t fileType;
	uint32_t offsets[];
}
LBXHeader_t;

typedef struct __attribute__((__packed__))
{
	uint16_t magic;
	uint16_t type;
	uint16_t reserved[6];
	uint8_t data;
}
LBXSoundHeader_t;

typedef struct __attribute__((__packed__))
{
	uint16_t numElements;
	uint16_t elementSize;
	char elements[];
}
LBXArrayHeader_t;

typedef struct __attribute__((__packed__))
{
	uint16_t Width;
	uint16_t Height;
	uint16_t unknown1;  // always =0?
	uint16_t BitmapCount;
	uint16_t unknown2;   // Bitmap count - 1
	uint16_t unknown3;
	uint16_t unknown4;
	uint16_t PaletteInfoOffset;
	uint16_t flags;
	uint32_t BitmapOffsets[];
}
LBXGfxHeader_t;

typedef struct __attribute__((__packed__))
{
	uint16_t PaletteOffset;
	uint16_t FirstPaletteColourIndex;
	uint16_t PaletteColourCount;
	uint16_t Unknown;
}
LBXGfxPaletteInfo_t;

typedef struct __attribute__((__packed__))
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}
LBXGfxPaletteEntry_t;

typedef struct {
	int num_frame;
	SDL_Surface ** frame;
	uint16_t flags;
	SDL_Texture ** tex;
	int current_frame;
	int w; // width
	int h; // height
	Uint32 delay; //delay between 2 frame in millisecond
	Uint32 prev_time; //time when the previous anim frame was displayed
} LBXAnimation_t;

typedef struct __attribute__((__packed__))
{
	uint8_t unknown1[362];
	uint16_t unknown2[24];
	uint8_t width[8*LBX_FONT_CHAR_NUM];
	uint16_t offsets[8*LBX_FONT_CHAR_NUM];
	unsigned char data[];
}
LBXFontFile_t;

typedef struct {
	int num_col;
	int w[LBX_FONT_CHAR_NUM];
	int h[LBX_FONT_CHAR_NUM];
	uint8_t * data[LBX_FONT_CHAR_NUM];
} LBXFontTemplate_t;

int lbx_load_hero_names(const char * path);
LBXAnimation_t * lbx_decode_image(SDL_Renderer * render,const char * filename);
void lbx_reset_anim(LBXAnimation_t * anim);
res_type_t lbx_guess_type(const char * lbx_data);
char * lbx_read(const char * file_name);
char ** lbx_read_array(const char * filename, int logical_file_num);
LBXFontTemplate_t * lbx_decode_font(const char * filename,int font_num);
LBXAnimation_t * lbx_generate_font(SDL_Renderer * render,LBXFontTemplate_t * font, LBXGfxPaletteEntry_t * pal,int shadow);
LBXAnimation_t * lbx_decode_terrain(SDL_Renderer * render,const char * filename);
Mix_Chunk * lbx_decode_sound(const char * filename,int file_num);
Mix_Music * lbx_decode_music(const char * filename,int file_num);

#endif
