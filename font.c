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

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	int fd;
	unsigned char buf[128000];
	unsigned char * size;
	uint16_t * offset;
	int i;
	int j;
	int num_line;
	unsigned char * ptr;
	int not1;
	char result[128];
	int res_i;

	fd = open("../data_1_3_1_patch/FONTS.LBX",O_RDONLY);
	if(fd==-1){
		printf("Error opening file\n");
		exit(0);
	}
	read(fd,buf,0x6000);

	size = buf+0x4ba;
	offset = (uint16_t *)(buf+0x7ba);

//	for(i=672;i<768;i++){
	for(i=0;i<768;i++){
		not1 = 0;
		res_i = 0;
		num_line = size[i];
		printf("%c. %d lines\n",(i%96)+32,num_line);

		ptr = buf + 0x320 + offset[i];
		printf("offset = %04x\n",offset[i]);
	
		while(num_line > 0) {
			while(*ptr != 0x80) {
				printf("%02X ",*ptr);
				if( *ptr > 0x80 ) {
					for(j=0;j<(*ptr&0x7f);j++) {
						result[res_i]=' ';
						res_i++;
					}
				}
				else {
					if((*ptr&0x0f) != 1) {
						not1=1;
					}
					for(j=0;j<(*ptr>>4);j++) {
//						result[res_i]='@';
						result[res_i]='0' + (*ptr&0x0f);
						res_i++;
					}
				}
				ptr++;
			}
			printf("%02X ",*ptr);
			result[res_i]='\n';
			res_i++;
			num_line--;
			ptr++;
		}

		printf("\n");
		printf("\n");
		result[res_i]=0;
		printf("%s",result);
		printf("\n");
		if(not1) {
			printf("Not 1\n");
		}
		printf("\n");
	}

	close(fd);
	return 0;
}
