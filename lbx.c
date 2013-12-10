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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "sdl.h"
#include "lbx.h"
#include "game.h"

LBXGfxPaletteEntry_t default_palette[256] = {{0x0,0x0,0x0},{0x8,0x4,0x4},{0x24,0x1c,0x18},{0x38,0x30,0x2c},{0x48,0x40,0x3c},{0x58,0x50,0x4c},{0x68,0x60,0x5c},{0x7c,0x74,0x70},{0x8c,0x84,0x80},{0x9c,0x94,0x90},{0xac,0xa4,0xa0},{0xc0,0xb8,0xb4},{0xd0,0xc8,0xc4},{0xe0,0xd8,0xd4},{0xf0,0xe8,0xe4},{0xfc,0xfc,0xfc},{0x38,0x20,0x1c},{0x40,0x2c,0x24},{0x48,0x34,0x2c},{0x50,0x3c,0x30},{0x58,0x40,0x34},{0x5c,0x44,0x38},{0x60,0x48,0x3c},{0x64,0x4c,0x3c},{0x68,0x50,0x40},{0x70,0x54,0x44},{0x78,0x5c,0x4c},{0x80,0x64,0x50},{0x8c,0x70,0x58},{0x94,0x74,0x5c},{0x9c,0x7c,0x64},{0xa4,0x84,0x68},{0xec,0xc0,0xd4},{0xd4,0x98,0xb4},{0xbc,0x74,0x94},{0xa4,0x54,0x7c},{0x8c,0x38,0x60},{0x74,0x24,0x4c},{0x5c,0x10,0x34},{0x44,0x4,0x20},{0xec,0xc0,0xc0},{0xd4,0x94,0x94},{0xbc,0x74,0x74},{0xa4,0x54,0x54},{0x8c,0x38,0x38},{0x74,0x24,0x24},{0x5c,0x10,0x10},{0x44,0x4,0x4},{0xec,0xd4,0xc0},{0xd4,0xb4,0x98},{0xbc,0x98,0x74},{0xa4,0x7c,0x54},{0x8c,0x60,0x38},{0x74,0x4c,0x24},{0x5c,0x34,0x10},{0x44,0x24,0x4},{0xec,0xec,0xc0},{0xd4,0xd4,0x94},{0xbc,0xbc,0x74},{0xa4,0xa4,0x54},{0x8c,0x8c,0x38},{0x74,0x74,0x24},{0x5c,0x5c,0x10},{0x44,0x44,0x4},{0xd4,0xec,0xbc},{0xb8,0xd4,0x98},{0x98,0xbc,0x74},{0x7c,0xa4,0x54},{0x60,0x8c,0x38},{0x4c,0x74,0x24},{0x38,0x5c,0x10},{0x24,0x44,0x4},{0xc0,0xec,0xc0},{0x98,0xd4,0x98},{0x74,0xbc,0x74},{0x54,0xa4,0x54},{0x38,0x8c,0x38},{0x24,0x74,0x24},{0x10,0x5c,0x10},{0x4,0x44,0x4},{0xc0,0xec,0xd8},{0x98,0xd4,0xb8},{0x74,0xbc,0x98},{0x54,0xa4,0x7c},{0x38,0x8c,0x60},{0x24,0x74,0x4c},{0x10,0x5c,0x38},{0x4,0x44,0x24},{0xf4,0xc0,0xa0},{0xe0,0xa0,0x84},{0xcc,0x84,0x6c},{0xc8,0x8c,0x68},{0xa8,0x78,0x54},{0x98,0x68,0x4c},{0x8c,0x60,0x44},{0x7c,0x50,0x3c},{0xc0,0xd8,0xec},{0x94,0xb4,0xd4},{0x70,0x98,0xbc},{0x54,0x7c,0xa4},{0x38,0x64,0x8c},{0x24,0x4c,0x74},{0x10,0x38,0x5c},{0x4,0x24,0x44},{0xc0,0xc0,0xec},{0x98,0x98,0xd4},{0x74,0x74,0xbc},{0x54,0x54,0xa4},{0x3c,0x38,0x8c},{0x24,0x24,0x74},{0x10,0x10,0x5c},{0x4,0x4,0x44},{0xd8,0xc0,0xec},{0xb8,0x98,0xd4},{0x98,0x74,0xbc},{0x7c,0x54,0xa4},{0x60,0x38,0x8c},{0x4c,0x24,0x74},{0x38,0x10,0x5c},{0x24,0x4,0x44},{0xec,0xc0,0xec},{0xd4,0x98,0xd4},{0xbc,0x74,0xbc},{0xa4,0x54,0xa4},{0x8c,0x38,0x8c},{0x74,0x24,0x74},{0x5c,0x10,0x5c},{0x44,0x4,0x44},{0xd8,0xd0,0xd0},{0xc0,0xb0,0xb0},{0xa4,0x90,0x90},{0x8c,0x74,0x74},{0x78,0x5c,0x5c},{0x68,0x4c,0x4c},{0x5c,0x3c,0x3c},{0x48,0x2c,0x2c},{0xd0,0xd8,0xd0},{0xb0,0xc0,0xb0},{0x90,0xa4,0x90},{0x74,0x8c,0x74},{0x5c,0x78,0x5c},{0x4c,0x68,0x4c},{0x3c,0x5c,0x3c},{0x2c,0x48,0x2c},{0xc8,0xc8,0xd4},{0xb0,0xb0,0xc0},{0x90,0x90,0xa4},{0x74,0x74,0x8c},{0x5c,0x5c,0x78},{0x4c,0x4c,0x68},{0x3c,0x3c,0x5c},{0x2c,0x2c,0x48},{0xd8,0xdc,0xec},{0xc8,0xcc,0xdc},{0xb8,0xc0,0xd4},{0xa8,0xb8,0xcc},{0x9c,0xb0,0xcc},{0x94,0xac,0xcc},{0x88,0xa4,0xcc},{0x88,0x94,0xdc},{0xfc,0xf0,0x90},{0xfc,0xe4,0x60},{0xfc,0xc8,0x24},{0xfc,0xac,0xc},{0xfc,0x78,0x10},{0xd0,0x1c,0x0},{0x98,0x0,0x0},{0x58,0x0,0x0},{0x90,0xf0,0xfc},{0x60,0xe4,0xfc},{0x24,0xc8,0xfc},{0xc,0xac,0xfc},{0x10,0x78,0xfc},{0x0,0x1c,0xd0},{0x0,0x0,0x98},{0x0,0x0,0x58},{0xfc,0xc8,0x64},{0xfc,0xb4,0x2c},{0xec,0xa4,0x24},{0xdc,0x94,0x1c},{0xcc,0x88,0x18},{0xbc,0x7c,0x14},{0xa4,0x6c,0x1c},{0x8c,0x60,0x24},{0x78,0x54,0x24},{0x60,0x44,0x24},{0x48,0x38,0x24},{0x34,0x28,0x1c},{0x90,0x68,0x34},{0x90,0x64,0x2c},{0x94,0x6c,0x34},{0x94,0x70,0x40},{0x8c,0x5c,0x24},{0x90,0x64,0x2c},{0x90,0x68,0x30},{0x98,0x78,0x4c},{0x60,0x3c,0x2c},{0x54,0xa4,0xa4},{0xc0,0x0,0x0},{0xfc,0x88,0xe0},{0xfc,0x58,0x84},{0xf4,0x0,0xc},{0xd4,0x0,0x0},{0xac,0x0,0x0},{0xe8,0xa8,0xfc},{0xe0,0x7c,0xfc},{0xd0,0x3c,0xfc},{0xc4,0x0,0xfc},{0x90,0x0,0xbc},{0xfc,0xf4,0x7c},{0xfc,0xe4,0x0},{0xe4,0xd0,0x0},{0xa4,0x98,0x0},{0x64,0x58,0x0},{0xac,0xfc,0xa8},{0x74,0xe4,0x70},{0x0,0xbc,0x0},{0x0,0xa4,0x0},{0x0,0x7c,0x0},{0xac,0xa8,0xfc},{0x80,0x7c,0xfc},{0x0,0x0,0xfc},{0x0,0x0,0xbc},{0x0,0x0,0x7c},{0x30,0x30,0x50},{0x28,0x28,0x48},{0x24,0x24,0x40},{0x20,0x1c,0x38},{0x1c,0x18,0x34},{0x18,0x14,0x2c},{0x14,0x10,0x24},{0x10,0xc,0x20},{0xa0,0xa0,0xb4},{0x88,0x88,0xa4},{0x74,0x74,0x90},{0x60,0x60,0x80},{0x50,0x4c,0x70},{0x40,0x3c,0x60},{0x30,0x2c,0x50},{0x24,0x20,0x40},{0x18,0x14,0x30},{0x10,0xc,0x20},{0x14,0xc,0x8},{0x18,0x10,0xc},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0},{0x0,0x0,0x0}};

/*
void lbx_write(const char * file_name,const char * lbx_data)
{
	LBXHeader_t * header;
	int i;
	int fd;
	char outfile_name[2048];
	ssize_t size;

	header = (LBXHeader_t *)lbx_data;

	for(i=0;i<header->numEntries;i++) {
		sprintf(outfile_name,"%s.%d",file_name,i);
		fd = open(outfile_name,O_CREAT | O_RDWR | O_TRUNC);
		if(fd==-1) {
			perror(NULL);
			printf("Error opening %s (%d)\n",outfile_name,errno);
			return;
		}

		size = write(fd,lbx_data+(header->offsets[i]),header->offsets[i+1]-header->offsets[i]);

		close(fd);
		if( size != header->offsets[i+1]-header->offsets[i] ) {
			printf("Error writing to %s\n",outfile_name);
			return;
		}
	}
}
*/

void lbx_display_header(LBXHeader_t * header)
{
	printf("numEntries = %d\n",header->numEntries);
	printf("magic = %04X\n",header->magic);
	printf("reserved = %d\n",header->reserved);
	printf("fileType = %d\n",header->fileType);
}

void lbx_check_header(LBXHeader_t * header)
{
	if( header->magic != 0xfead && header->magic != 0xdeaf ) {
		printf("Unknown header magic value : %d\n",header->magic);
	}

	if( header->reserved != 0 ) {
		printf("Unknown header reserved value : %d\n",header->reserved);
	}

	if( header->fileType != LBX_TYPE_UNKNOWN && header->fileType != LBX_TYPE_STRING && header->fileType != LBX_TYPE_FONT) {
		printf("Unknown header file type value : %d\n",header->fileType);
	}
}

void lbx_check_gfx_header(LBXGfxHeader_t * h)
{
	uint16_t f = h->flags;

	if( h->unknown1 != 0 ) {
		printf("GfxHeader unknown1 = %d\n",h->unknown1);
	}
	if( h->unknown2 != h->BitmapCount-1 ) {
		printf("GfxHeader unknown2 is different of BitmapCount-1 : %d\n",h->unknown2);
	}
	if( h->unknown3 != 0 ) {
		printf("GfxHeader unknown3 = %d\n",h->unknown3);
	}
	if( h->unknown4 != 0 ) {
		printf("GfxHeader unknown4 = %d\n",h->unknown4);
	}

	if(f & LBX_ANIM_FLAGS_TRANSPARENT) {
		printf("Transparent flag set ");
	}
	f = f & !LBX_ANIM_FLAGS_TRANSPARENT;

	if(f & LBX_ANIM_FLAGS_JUNCTION) {
		printf("Junction flag set\n");
	}
	f = f & !LBX_ANIM_FLAGS_JUNCTION;

	if(f & LBX_ANIM_FLAGS_INTERNAL_PALETTE) {
		printf("Internal palette flag set\n");
	}
	f = f & !LBX_ANIM_FLAGS_INTERNAL_PALETTE;

	if(f & LBX_ANIM_FLAGS_FUNCTIONAL_COLORS) {
		printf("Functional palette flag set\n");
	}
	f = f & !LBX_ANIM_FLAGS_FUNCTIONAL_COLORS;

	if(f & LBX_ANIM_FLAGS_NO_COMPRESSION) {
		printf("No compression flag set\n");
	}
	f = f & !LBX_ANIM_FLAGS_NO_COMPRESSION;

	if( f != 0 ) {
		printf("GfxHeader unknown flag = %04X\n",f);
	}
}

res_type_t lbx_guess_type(const char * lbx_data)
{
	LBXHeader_t * header;
	LBXSoundHeader_t * sound;
	LBXGfxHeader_t * anim;
#if 0
	int resource_length;
	LBXArrayHeader_t * array;
#endif

	header = (LBXHeader_t *)lbx_data;
	lbx_check_header(header);

	sound = (LBXSoundHeader_t *)(lbx_data+header->offsets[0]);
#if 0
	array = (LBXArrayHeader_t *)(lbx_data+header->offsets[0]);
#endif
	anim = (LBXGfxHeader_t *)(lbx_data+header->offsets[0]);

	if( header->fileType == LBX_TYPE_FONT ) {
		return LBX_T_FONT;
	}

#if 0
	resource_length = header->offsets[1] - header->offsets[0];
	if( array->numElements * array->elementSize +4 == resource_length ) {
		return LBX_T_ARRAY;
	}
#endif

	if( header->fileType == LBX_TYPE_STRING ) {
		/* Hack for terrain file */
		if(header->offsets[1] == 0x0a5480) {
			return LBX_T_TERRAIN;
		}
		return LBX_T_ARRAY;
	}

	if( sound->magic == 0xdeaf ) {
		return LBX_T_SOUND;
	}

	if( anim->unknown1 == 0 ) {
		return LBX_T_ANIM;
	}

	return LBX_T_DATA;
}

/* Returns the full content of a file or NULL if error.
The returned buffer must be freed by caller */
char * lbx_read(const char * file_name)
{
	int fd;
	int ret;
	struct stat stat_buf;
	char * buf;
	ssize_t size;

	ret  = stat(file_name,&stat_buf);
	if( ret == -1 ) {
		return NULL;
	};

	buf = (char*)malloc(stat_buf.st_size);
	if( buf == NULL ) {
		return NULL;
	}

	fd = open(file_name,O_RDONLY);
	if( fd == -1 ) {
		free(buf);
		return NULL;
	}

	size = read(fd,buf,stat_buf.st_size);
	close(fd);
	if (size == -1 ) {
		free(buf);
		return NULL;
	}

	return buf;
}

void lbx_decode(const char * file_name)
{
	res_type_t type;
	char * buf;

	buf = lbx_read(file_name);

	if( buf == NULL ) {
		perror(NULL);
		printf("Error reading %s\n",file_name);
		return ;
	}

	lbx_display_header((LBXHeader_t *)buf);

	type = lbx_guess_type(buf);

	switch(type) {
	case LBX_T_SOUND:
		printf("Sound\n");
		break;
	case LBX_T_ARRAY:
		printf("Array of data\n");
		break;
	case LBX_T_ANIM:
		printf("Anim\n");
		break;
	case LBX_T_DATA:
		printf("Data\n");
		break;
	default:
		printf("Error\n");
		break;
	}

	free(buf);
}

int lbx_load_hero_names(const char * path)
{
	char file_name[2048];
	char * buf;
	LBXHeader_t * header;
	LBXArrayHeader_t * array;
	char * array_data;
	char ** hero_names;
	int i;
//	int num_heros;

	if(path) {
		sprintf(file_name,"%s/names.lbx",path);
	} else {
		sprintf(file_name,"names.lbx");
	}

	buf = lbx_read(file_name);
	if(buf == NULL) {
		perror(NULL);
		printf("Error reading %s\n",file_name);
		return -1;
	}

	header = (LBXHeader_t *)buf;
	lbx_check_header(header);
	array = (LBXArrayHeader_t *)(buf + header->offsets[0]);
	array_data = ((char *)array) + sizeof(LBXArrayHeader_t);

//	num_heros = array->numElements;

	hero_names = (char**)malloc( array->numElements * sizeof(char *));
	if(hero_names==NULL) {
		perror(NULL);
		free(buf);
		printf("Memory error\n");
		return -1;
	}

	for(i=0; i<array->numElements; i++) {
		hero_names[i] = strdup( array_data + ( i * array->elementSize) );
		if(hero_names[i]==NULL) {
			perror(NULL);
			free(buf);
			printf("Memory error\n");
			return -1;
		}

		printf("%d: %s\n",i+1,hero_names[i]);
	}

	free(buf);
	return 0;
}

/* return terminated list of string */
char ** lbx_read_array(const char * filename, int logical_file_num)
{
	char * raw_data;
	raw_data = lbx_read(filename);
	LBXHeader_t * header = NULL;
	LBXArrayHeader_t * array_header = NULL;
	char ** array;
	int i;

	if(raw_data == NULL) {
		return NULL;
	}

	header = (LBXHeader_t *)raw_data;
	lbx_check_header(header);

	if(header->fileType != LBX_TYPE_STRING) {
		printf("this is not a string list file\n");
		return NULL;
	}

	printf("file in archive = %d\n",header->numEntries);
	if( logical_file_num >= header->numEntries ) {
		printf("Incorrect logical file\n");
		return NULL;
	}

	array_header = (LBXArrayHeader_t *)(raw_data + header->offsets[logical_file_num]);

	array = (char**)malloc( (array_header->numElements+1) * sizeof(char*));
	for(i=0; i<array_header->numElements; i++) {
		array[i] = strdup(array_header->elements+(i*array_header->elementSize));
	}
	array[array_header->numElements] = NULL;

	return array;
}

LBXAnimation_t * lbx_decode_image(SDL_Renderer * render, const char * filename)
{
	char * raw_data;
	int RLE_val;
	uint32_t BitmapStart;
	uint32_t BitmapEnd;
	int x;
	int y;
	int BitmapSize;
	int BitmapIndex;
	int next_ctl;
	int long_data;
	int n_r;
	int last_pos;
	int RleLength;
	int RleCounter;
	LBXHeader_t * header = NULL;
	LBXGfxHeader_t * GfxHeader = NULL;
	LBXGfxPaletteInfo_t * GfxPaletteInfo = NULL;
	LBXGfxPaletteEntry_t * GfxPaletteEntry;
	LBXGfxPaletteEntry_t Palette[256];
	uint8_t *ImageBuffer;
	int i,j;
	char * file_start;
	LBXAnimation_t * anim = NULL;
	uint32_t old_offset;
	int read_entries = 0;
	char * desc = NULL;;
	int FirstPaletteColourIndex = 0;
	int PaletteColourCount = 256;
	int own_palette = 0;


	// Open LBX file
	raw_data = lbx_read(filename);

	if(raw_data == NULL) {
		return NULL;
	}

	header = (LBXHeader_t *)raw_data;
	lbx_check_header(header);

	if(header->fileType != LBX_TYPE_UNKNOWN) {
		printf("this is an unknown file type\n");
		return NULL;
	}

	printf("file in archive = %d\n",header->numEntries);

	anim = (LBXAnimation_t*)malloc((header->numEntries + 1) * sizeof(LBXAnimation_t));

	/* After offsets and before the beginning of the first file, there is
	description strings related to the animation */
	desc = (char*)(&header->offsets[header->numEntries+1]);
	while(desc[0]==0) {
		desc++;
	}

	for(i=0; i<256; i++) {
		Palette[i].r = default_palette[i].r;
		Palette[i].g = default_palette[i].g;
		Palette[i].b = default_palette[i].b;
	}

	j=0;
	old_offset = -1;
	while( j < header->numEntries ) {
		/* Some file seems buggy. Their offset table is wrong with several times the same offset or null offset. Trying to workaround this */
		while( header->offsets[j] == old_offset ) {
			printf("Same offset %08X, skipping\n",old_offset);
			j++;
		}
		while( header->offsets[j] == 0) {
			printf("Null offset, skipping\n");
			j++;
		}

		file_start = raw_data+header->offsets[j];
		GfxHeader = (LBXGfxHeader_t *)(file_start);
		/* The description consist in a FILENAME of 8 char max + a description */
		printf(" File %d (%s: %s)",read_entries,desc,desc+9);
		desc=desc+9;
		/* Find end of description */
		while(desc[0]!=0) {
			desc++;
		}
		/* Find start of next FILENAME */
		while(desc[0]==0) {
			desc++;
		}

		lbx_check_gfx_header(GfxHeader);

		// Read palette info if present
		if ( GfxHeader->PaletteInfoOffset > 0 )  {
			own_palette = 1;
			GfxPaletteInfo = (LBXGfxPaletteInfo_t *)(file_start+GfxHeader->PaletteInfoOffset);
			GfxPaletteEntry = (LBXGfxPaletteEntry_t *)(file_start+GfxPaletteInfo->PaletteOffset);
			printf("PaletteColourCount = %d ",GfxPaletteInfo->PaletteColourCount);
			printf("FirstPaletteColourIndex = %d ",GfxPaletteInfo->FirstPaletteColourIndex);

			PaletteColourCount = GfxPaletteInfo->PaletteColourCount;
			FirstPaletteColourIndex = GfxPaletteInfo->FirstPaletteColourIndex;
			for(i=0; i<PaletteColourCount; i++) {
				Palette[i+FirstPaletteColourIndex].r = GfxPaletteEntry[i].r << 2;
				Palette[i+FirstPaletteColourIndex].g = GfxPaletteEntry[i].g << 2;
				Palette[i+FirstPaletteColourIndex].b = GfxPaletteEntry[i].b << 2;
			}
		}

		printf(" X= %d, Y= %d  ",GfxHeader->Width,GfxHeader->Height);
		printf("BitmapCount = %d\n",GfxHeader->BitmapCount);

		anim[read_entries].frame = (SDL_Surface**)malloc( (GfxHeader->BitmapCount+1) * sizeof(SDL_Surface*) );
		anim[read_entries].tex = (SDL_Texture**)malloc( GfxHeader->BitmapCount * sizeof(SDL_Texture*));
		anim[read_entries].num_frame = GfxHeader->BitmapCount;
		anim[read_entries].current_frame = 0;
		anim[read_entries].flags = GfxHeader->flags;
		anim[read_entries].w = GfxHeader->Width;
		anim[read_entries].h = GfxHeader->Height;
		anim[read_entries].delay = ANIM_DELAY;
		anim[read_entries].prev_time = 0;

		for( i=0; i<GfxHeader->BitmapCount; i++) {
			anim[read_entries].frame[i+1]=NULL;
			anim[read_entries].frame[i]=SDL_CreateRGBSurface(0, GfxHeader->Width, GfxHeader->Height, 32, RMASK, GMASK, BMASK, AMASK);
			if(anim[read_entries].frame[i]==NULL) {
				printf("Error creating surface\n");
				return NULL;
			}

			SDL_LockSurface(anim[read_entries].frame[i]);

			// Set transparent background color
			for(x=0; x<GfxHeader->Width; x++) {
				for(y=0; y<GfxHeader->Height; y++) {
					sdl_set_pixel(anim[read_entries].frame[i],x,y,0,0,0,SDL_TRANSPARENT);
				}
			}

			//Values of at least this indicate run length values
			RLE_val = FirstPaletteColourIndex +
					  PaletteColourCount;

			BitmapStart = GfxHeader->BitmapOffsets[i];
			BitmapEnd = GfxHeader->BitmapOffsets[i+1];
			BitmapSize = BitmapEnd - BitmapStart;

			ImageBuffer = (uint8_t *)(file_start+BitmapStart);

#if 0
			//Byte 0 tells us whether to reset the image half way through an animation
			if( ImageBuffer[0]==1 && BitmapNo>0 ) {
				for(x=0; x<GfxHeader->Width; x++) {
					for(y=0; y<GfxHeader->Height; y++) {
						sdl_set_pixel(anim[read_entries].[i],x,y,0xff,0,0xff,SDL_OPAQUE);
					}
				}
			}
#endif

			//Decode bitmap
			BitmapIndex = 1; //Current index into the image buffer
			x = 0;
			y = GfxHeader->Height;
			next_ctl=0;
			long_data=0;
			n_r=0;
			last_pos=0;

			while( x < GfxHeader->Width && BitmapIndex<BitmapSize) {
				y=0;
				if(ImageBuffer[BitmapIndex]==0xff) {
					BitmapIndex++;
					RLE_val = FirstPaletteColourIndex + PaletteColourCount;
				} else {
					long_data = ImageBuffer[BitmapIndex + 2];
					next_ctl = BitmapIndex + ImageBuffer[BitmapIndex+1]+2;
					if(ImageBuffer[BitmapIndex] == 0x00) {
						RLE_val = FirstPaletteColourIndex + PaletteColourCount;
					} else if(ImageBuffer[BitmapIndex] == 0x80) {
						RLE_val = 0xE0;
					} else {
						printf("Unrecognized RLE value\n");
						return NULL;
					}

					y = ImageBuffer[BitmapIndex + 3];
					BitmapIndex += 4;

					n_r = BitmapIndex;
					while(n_r < next_ctl) {
						while( n_r < BitmapIndex+long_data && x < GfxHeader->Width) {
							if(ImageBuffer[n_r] >= RLE_val) {
								//This value is an run length, the next value is the value to repeat
								last_pos = n_r+1;
								RleLength = ImageBuffer[n_r] - RLE_val + 1;
								if( RleLength + y > GfxHeader->Height) {
									printf("RLE length overrun on y\n");
									return NULL;
								}
								RleCounter = 0;

								while( RleCounter < RleLength && y < GfxHeader->Height) {
									if (x < GfxHeader->Width && y < GfxHeader->Height && x >= 0 && y >= 0) {
										/* With default pallette, color 232 is shadow */
										if( own_palette == 0 && ImageBuffer[last_pos] == SHADOW_COLOR ) {
											sdl_set_pixel(anim[read_entries].frame[i],x,y,0,0,0,128);
										}
										/* With default pallette, color 233 is light */
										else if( own_palette == 0 && ImageBuffer[last_pos]==LIGHT_COLOR) {
											sdl_set_pixel(anim[read_entries].frame[i],x,y,255,255,255,128);
										} else {
											sdl_set_pixel(anim[read_entries].frame[i],x,y,PAL_TO_RGB(Palette[(int)ImageBuffer[last_pos]]) );
										}
									} else {
										printf("RLE length overrun on output\n");
										return NULL;
									}
									y++;
									RleCounter++;
								}
								n_r += 2;
							} else {
								// Regular single pixel
								if( x < GfxHeader->Width && y < GfxHeader->Height && x >= 0 && y >= 0) {
									/* With default pallette, color 232 is shadow */
									if( own_palette == 0 && ImageBuffer[n_r] == SHADOW_COLOR ) {
										sdl_set_pixel(anim[read_entries].frame[i],x,y,0,0,0,128);
									}
									/* With default pallette, color 233 is light */
									else if( own_palette == 0 && ImageBuffer[n_r]==LIGHT_COLOR) {
										sdl_set_pixel(anim[read_entries].frame[i],x,y,255,255,255,128);
									} else {
										sdl_set_pixel(anim[read_entries].frame[i],x,y,PAL_TO_RGB(Palette[(int)ImageBuffer[n_r]]) );
									}
								} else {
									printf("Buffer overrun\n");
									return anim;
								}
								n_r++;
								y++;
							}
						}
						if( n_r < next_ctl ) {
							// We are on another RLE on the line. Some other data are here
							y += ImageBuffer[n_r + 1]; // next pos Y to write pixels
							BitmapIndex = n_r + 2;
							long_data = ImageBuffer[n_r]; // number of data to put
							n_r += 2;
							if( n_r >= next_ctl) {
								printf("More RLE but lines too short\n");
							}

						}
					}

					BitmapIndex = next_ctl; // jump to next line
				}
				x++;
			}
			SDL_UnlockSurface(anim[read_entries].frame[i]);

			anim[read_entries].tex[i]=SDL_CreateTextureFromSurface(render,anim[read_entries].frame[i]);
		}

		old_offset = header->offsets[j];
		j++;
		read_entries++;
	}

	/* Set end of files*/
	anim[read_entries].frame = NULL;
	anim[read_entries].tex = NULL;
	anim[read_entries].num_frame = 0;
	anim[read_entries].current_frame = 0;
	anim[read_entries].flags = 0;
	anim[read_entries].w = 0;
	anim[read_entries].h = 0;

	return anim;
}

void lbx_reset_anim(LBXAnimation_t * anim)
{
	anim->prev_time=0;
	anim->current_frame=0;
}

int font_get_height(LBXFontFile_t * font, int index)
{
	int width = font->width[index];
	int height = 0;
	int cur_height = 0;
	unsigned char * ptr;

	ptr = (unsigned char *)((char*)font + font->offsets[index]);
	while(width > 0) {
		cur_height = 1;
		while(*ptr != 0x80) {
			if( *ptr > 0x80 ) {
				cur_height += (*ptr&0x7f);
			} else {
				cur_height += (*ptr>>4);
			}
			ptr++;
		}
		if(cur_height > height) {
			height = cur_height;
		}
		width--;
		ptr++;
	}

	return height;
}

LBXFontTemplate_t * lbx_decode_font(const char * filename,int font_num)
{
	char * raw_data;
	LBXHeader_t * header = NULL;
	LBXFontFile_t * font;
	int i,j;
	int y;
	int width;
	unsigned char * ptr;
	LBXFontTemplate_t * t;
	int index;

	raw_data = lbx_read(filename);

	if(raw_data == NULL) {
		return NULL;
	}

	header = (LBXHeader_t *)raw_data;
	lbx_check_header(header);

	if(header->fileType != LBX_TYPE_FONT) {
		printf("this is not a font file\n");
		return NULL;
	}

	t = (LBXFontTemplate_t*)malloc(sizeof(LBXFontTemplate_t));

	printf("file in archive = %d\n",header->numEntries);

	/* Only first file have fonts, I do not know what the other files are (FLC files ??) */
	font = (LBXFontFile_t *)(raw_data + header->offsets[0]);

	t->num_col = 0;

	for(index=0,i=font_num*LBX_FONT_CHAR_NUM; i<(font_num+1)*LBX_FONT_CHAR_NUM; i++,index++) {
		t->w[index]=font->width[i];
		t->h[index]=font_get_height(font,i);
		t->data[index]=(uint8_t*)malloc((t->w[index])*(t->h[index]));
		memset(t->data[index],0xff,t->w[index] * t->h[index]);

		/* each character is encoded from top to bottom then from left
		to right. i.e. not in the horizontal axis but vertically */
		/* 0x80 means end of column */
		/* 0x8n means skip n pixels */
		/* 0xAB means draw A pixels of color B */
		width=font->width[i];
		ptr = (unsigned char *)((char*)font + font->offsets[i]);
		while(width > 0) {
			y=0;
			while(*ptr != 0x80) {
				if( *ptr > 0x80 ) {
					y+=(*ptr&0x0f);
				} else {
					for(j=0; j<(*ptr>>4); j++) {
						t->data[index][(font->width[i]-width)+(t->w[index]*y)]=(int)(*ptr)&0x0f;
						if(((int)(*ptr)&0x0f)>t->num_col) {
							t->num_col=((int)(*ptr)&0x0f);
						}
						y++;
					}
				}
				ptr++;
			}
			width--;
			ptr++;
		}
	}

	t->num_col++;

	return t;
}

/* if shadow != 0, draw a shadow below the letter.
The color entry after the last needed color entry is the shadow color ( = num_col) */
LBXAnimation_t * lbx_generate_font(SDL_Renderer * render, LBXFontTemplate_t * font, LBXGfxPaletteEntry_t * pal,int shadow)
{
	LBXAnimation_t * anim;
	anim = (LBXAnimation_t*)malloc(LBX_FONT_CHAR_NUM*sizeof(LBXAnimation_t));
	int pal_index;
	int i,x,y;

	for(i=0; i<LBX_FONT_CHAR_NUM; i++) {
		anim[i].num_frame=1;
		anim[i].current_frame=0;
		anim[i].w=font->w[i];
		anim[i].h=font->h[i];
		anim[i].w++;
		anim[i].h++;
		anim[i].flags=0;
		anim[i].delay=0;
		anim[i].prev_time=0;

		anim[i].frame=(SDL_Surface**)malloc(sizeof(SDL_Surface *));
		anim[i].frame[0] = SDL_CreateRGBSurface(0,anim[i].w,anim[i].h,32,RMASK, GMASK, BMASK, AMASK);

		for(x=0; x<anim[i].w; x++) {
			for(y=0; y<anim[i].h; y++) {
				sdl_set_pixel(anim[i].frame[0],x,y,0,0,0,SDL_TRANSPARENT);
			}
		}

		SDL_LockSurface(anim[i].frame[0]);

		for(x=0; shadow && (x<font->w[i]) ; x++) {
			for(y=0; y<font->h[i]; y++) {
				pal_index = (int)font->data[i][x+y*font->w[i]];
				if(pal_index!=0xff) {
					sdl_set_pixel(anim[i].frame[0],x+1,y+1,
								  pal[font->num_col].r,
								  pal[font->num_col].g,
								  pal[font->num_col].b,
								  SDL_OPAQUE);
				}
			}
		}

		for(x=0; x<font->w[i]; x++) {
			for(y=0; y<font->h[i]; y++) {
				pal_index = (int)font->data[i][x+y*font->w[i]];
				if(pal_index!=0xff) {
					sdl_set_pixel(anim[i].frame[0],x,y,
								  pal[pal_index].r,
								  pal[pal_index].g,
								  pal[pal_index].b,
								  SDL_OPAQUE);
				}
			}
		}

		SDL_UnlockSurface(anim[i].frame[0]);

		anim[i].tex=(SDL_Texture **)malloc(sizeof(SDL_Texture *));;
		anim[i].tex[0]=SDL_CreateTextureFromSurface(render,anim[i].frame[0]);
	}

	return anim;
}

LBXAnimation_t * lbx_decode_terrain(SDL_Renderer * render, const char * filename)
{
	uint8_t * raw_data;
	LBXHeader_t * header = NULL;
	uint8_t * ptr;
	uint16_t width;
	uint16_t height;
	uint16_t unknown;
	LBXAnimation_t * anim = NULL;
	LBXAnimation_t * current = NULL;
	int anim_num = 0;
	int x,y;
	unsigned int col;
	uint16_t * index;
	uint16_t msb;
	uint16_t index1;
	uint16_t index2;
	int i;

	raw_data = (uint8_t *)lbx_read(filename);

	if(raw_data == NULL) {
		return NULL;
	}

	header = (LBXHeader_t *)raw_data;
	lbx_check_header(header);

	if(header->fileType != LBX_TYPE_STRING && header->offsets[1]!=0x0a5480) {
		printf("this is not a terrain file\n");
		return NULL;
	}

	ptr = raw_data + header->offsets[0];
	index = (uint16_t *)(raw_data + header->offsets[1]);

	while(*ptr == 0) {
		ptr++;
	}

	while(ptr < raw_data+header->offsets[1]) {

		width = *((uint16_t *)ptr);
		ptr += sizeof(uint16_t);
		height = *((uint16_t *)ptr);
		ptr += sizeof(uint16_t);
		unknown = *((uint16_t *)ptr);
		ptr += sizeof(uint16_t);

		if( width != 20) {
			printf("width != 20 : %d\n",width);
		}
		if( height != 18) {
			printf("height != 18 : %d\n",height);
		}
		if( unknown != 0xde0a) {
			printf("unknown != 0xde0a : %04X\n",unknown);
		}

		/* Skip the rest of the header */
		ptr+=10;

		anim_num++;
		anim = (LBXAnimation_t*)realloc(anim,(anim_num+1)*sizeof(LBXAnimation_t));

		current=&anim[anim_num-1];

		index1=index[anim_num-1];
		msb = (((index1)&(0x7f))/3)*128;
		if( index1 & 0x80 ) {
			index1 = (index1>>8)+(msb);
			index2 = index[anim_num];
			msb = (((index2)&(0x7f))/3)*128;
			index2 = (index2>>8)+(msb);

			current->num_frame=index2-index1;
		} else {
			current->num_frame=1;
		}

		current->current_frame=0;
		current->w=width;
		current->h=height;
		current->delay=200;
		current->frame=(SDL_Surface**)malloc(current->num_frame*sizeof(SDL_Surface *));
		current->tex = (SDL_Texture **)malloc( current->num_frame*sizeof(SDL_Texture *) );
		current->flags=0;
		current->prev_time=0;


		for(i=0; i<current->num_frame; i++) {
			if(i!=0) {
				/* Skip the header */
				ptr+=16;
			}
			current->frame[i] = SDL_CreateRGBSurface(0,current->w,current->h,32,RMASK, GMASK, BMASK, AMASK);
			SDL_LockSurface(current->frame[i]);

			for(x=0; x<current->w; x++) {
				for(y=0; y<current->h; y++) {
					col=(int)(*ptr);
					sdl_set_pixel(current->frame[i],x,y,
								  default_palette[col].r,
								  default_palette[col].g,
								  default_palette[col].b,
								  SDL_OPAQUE);
					ptr++;
				}
			}

			SDL_UnlockSurface(current->frame[i]);
			current->tex[i]=SDL_CreateTextureFromSurface(render,current->frame[i]);

			/* Skip the footer */
			ptr+=8;
		}
	}

	current=&anim[anim_num];
	current->frame=NULL;

	printf("%d terrain tiles\n",anim_num);

	return anim;
}

Mix_Chunk * lbx_decode_sound(const char * filename,int file_num)
{
	char * raw_data;
	LBXHeader_t * header = NULL;
	char * file_start;
	LBXSoundHeader_t * sound_header;
	int fd;
	size_t size;
	Mix_Chunk * chunk;

	raw_data = lbx_read(filename);

	if(raw_data == NULL) {
		return NULL;
	}

	header = (LBXHeader_t *)raw_data;
	lbx_check_header(header);

	printf("file in archive = %d\n",header->numEntries);

	file_start = raw_data+header->offsets[file_num];
	sound_header = (LBXSoundHeader_t *)(file_start);

	if( sound_header->magic != 0xdeaf ) {
		printf("This is not a sound resource (%d)\n",file_num);
		return NULL;
	}

	if( sound_header->type != SOUND_VOC ) {
		printf("Resource %d is not a VOC resource (%d),\n",file_num,sound_header->type);
		return NULL;
	}

	fd = open(SOUND_TMP,O_CREAT | O_RDWR | O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if(fd==-1) {
		perror(NULL);
		printf("Error opening %s (%d)\n",SOUND_TMP,errno);
		return NULL;
	}

	size = write(fd,&sound_header->data,header->offsets[file_num+1]-header->offsets[file_num]);

	close(fd);
	if( size != header->offsets[file_num+1]-header->offsets[file_num] ) {
		printf("Error writing to %s\n",SOUND_TMP);
		return NULL;
	}

	chunk = Mix_LoadWAV(SOUND_TMP);
	if(chunk == NULL ) {
		printf("Error decoding resource %d : %s\n",file_num,Mix_GetError());
		return NULL;

	}

	return chunk;
}

Mix_Music * lbx_decode_music(const char * filename,int file_num)
{
	char * raw_data;
	LBXHeader_t * header = NULL;
	char * file_start;
	LBXSoundHeader_t * sound_header;
	int fd;
	size_t size;
	Mix_Music * music;

	raw_data = lbx_read(filename);

	if(raw_data == NULL) {
		return NULL;
	}

	header = (LBXHeader_t *)raw_data;
	lbx_check_header(header);

	printf("file in archive = %d\n",header->numEntries);

	file_start = raw_data+header->offsets[file_num];
	sound_header = (LBXSoundHeader_t *)(file_start);

	if( sound_header->magic != 0xdeaf ) {
		printf("This is not a sound resource (%d)\n",file_num);
		return NULL;
	}

	if( sound_header->type != SOUND_MID ) {
		printf("Resource %d is not a MIDI resource (%d),\n",file_num,sound_header->type);
		return NULL;
	}

	fd = open(SOUND_TMP,O_CREAT | O_RDWR | O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if(fd==-1) {
		perror(NULL);
		printf("Error opening %s (%d)\n",SOUND_TMP,errno);
		return NULL;
	}

	size = write(fd,&sound_header->data ,header->offsets[file_num+1]-header->offsets[file_num]);

	close(fd);
	if( size != header->offsets[file_num+1]-header->offsets[file_num] ) {
		printf("Error writing to %s\n",SOUND_TMP);
		return NULL;
	}

	music = Mix_LoadMUS(SOUND_TMP);
	if(music == NULL ) {
		printf("Error decoding resource %d : %s\n",file_num,Mix_GetError());
		return NULL;

	}

	return music;
}
