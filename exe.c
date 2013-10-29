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
#include <ctype.h>
#include "exe.h"

extern const char * data_path;

static char * exe_data;
static race_t * race_data = NULL;
static def_unit_t * unit_data = NULL;

#define STRING_OFFSET 0x294a0
#define RACE_LOCATION 0x2b226
#define UNIT_LOCATION 0x2963C

/* Returns the full content of a file or NULL if error.
The returned buffer must be freed by caller */
char * exe_read(const char * file_name)
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

char * exe_init(const char * filename)
{
	char path[2048];
        char lower_case[2048];
        int i=0;

        while( filename[i] != 0 ) {
                lower_case[i] = tolower(filename[i]);
                i++;
        }
        lower_case[i]=0;

	if(data_path != NULL) {
                sprintf(path,"%s/%s",data_path,filename);
		exe_data = exe_read(path);
		if(exe_data != NULL) {
			goto exe_data_ok;
		}

		/* try lower case */
		sprintf(path,"%s/%s",data_path,lower_case);
		exe_data = exe_read(path);
		if(exe_data != NULL) {
			goto exe_data_ok;
		}
	}

	sprintf(path,"./%s",filename);
	exe_data = exe_read(path);
	if(exe_data != NULL) {
		goto exe_data_ok;
	}

        /* and with lowercase */
        sprintf(path,"./%s",lower_case);
	exe_data = exe_read(path);
	if(exe_data != NULL) {
		goto exe_data_ok;
	}

        printf("Could not load %s\n",filename);

	return NULL;

exe_data_ok:
	race_data=(race_t*)(exe_data+RACE_LOCATION);
	unit_data=(def_unit_t*)(exe_data+UNIT_LOCATION);

	return exe_data;
}

const char * exe_get_race_name(int index)
{
	return ( exe_data+STRING_OFFSET+race_data[index].name_ptr );
}

const char * exe_get_unit_name(int index)
{
	return ( exe_data+STRING_OFFSET+unit_data[index].name_ptr );
}

def_unit_t * exe_get_unit_def(int index)
{
	return ( &unit_data[index] );
}
