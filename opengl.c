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

#define GL_GLEXT_PROTOTYPES
#include <SDL.h>
#include <SDL_opengl.h>
#include "lbx.h"
#include "sdl.h"
#include "item.h"

GLuint render_tex = -1;
GLuint frame_buf = -1;

extern int window_w;
extern int window_h;

double cw;
double ch;
double cx;
double cy;

void opengl_clear_screen()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glClearColor(0, 0, 0, 0);
        glClear( GL_COLOR_BUFFER_BIT );
        glClearDepth(0.0f);
}

void opengl_clear_fbo()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buf);
        glClearColor(0, 0, 0, 0);
        glClear( GL_COLOR_BUFFER_BIT );
        glClearDepth(0.0f);
}

void opengl_init(int w, int h)
{
	/* Init screen buffer */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        glEnable( GL_TEXTURE_2D );
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	opengl_clear_screen();

	/* Init frame buffer */
	if(render_tex == (GLuint)(-1) || frame_buf == (GLuint)(-1)) {
		/* Create render texture */
		glGenTextures(1, &render_tex);
		glBindTexture(GL_TEXTURE_2D, render_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//NULL means reserve texture memory, but texels are undefined
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ORIGINAL_GAME_WIDTH, ORIGINAL_GAME_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

		/* Create frame buffer */
		glGenFramebuffersEXT(1, &frame_buf);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buf);

		/* Attach 2D texture to this FBO */
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, render_tex, 0);

		/* Does the GPU support current FBO configuration? */
		GLenum status;
		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				//printf("Frame buffer OK\n");
				break;
			default:
				printf("Frame buffer error\n");
				exit(-1);
		}

		glEnable( GL_TEXTURE_2D ); // Need this to display a texture
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		opengl_clear_screen();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	/* Initialize Matrix stack*/
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

        return;
}

void opengl_cleanup()
{
   //Delete resources
   glDeleteTextures(1, &render_tex);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glDeleteFramebuffersEXT(1, &frame_buf);
}

GLuint opengl_create_texture(SDL_Surface * surf)
{
	GLenum texture_format;
	GLint  nOfColors;
	GLuint tex;

	// get the number of channels in the SDL surface
	nOfColors = surf->format->BytesPerPixel;
	if (nOfColors == 4)     // contains an alpha channel
	{
		if (surf->format->Rmask == 0x000000ff)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	} else if (nOfColors == 3)     // no alpha channel
	{
		if (surf->format->Rmask == 0x000000ff)
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	} else {
		printf("Not a true color image\n");
		return -1;
	}
	glEnable(GL_TEXTURE_2D);
	glGenTextures( 1, &tex );
	glBindTexture( GL_TEXTURE_2D, tex );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surf->w, surf->h, 0,
			texture_format, GL_UNSIGNED_BYTE, surf->pixels );

        return tex;;
}

void opengl_blit_frame(LBXAnimation_t * anim, SDL_Rect * rect, int frame_num)
{
	double w;
	double h;

	if( anim == NULL ) {
		return;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buf);

	glViewport( 0, 0, ORIGINAL_GAME_WIDTH, ORIGINAL_GAME_HEIGHT );

	w = (double)rect->w; 
	h = (double)rect->h;

        // Bind the texture to which subsequent calls refer to
        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, anim->tex[frame_num] );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, ORIGINAL_GAME_WIDTH, ORIGINAL_GAME_HEIGHT, 0, 1, -1);

        glTranslatef(rect->x,rect->y, 0.0);

        glBegin( GL_QUADS );
        // Top-left vertex (corner)
        glTexCoord2i( 0, 0 );
        glVertex3f( 0.0, 0.0, 0.0 );

        // Bottom-left vertex (corner)
        glTexCoord2i( 0, 1 );
        glVertex3f( 0.0, h, 0.0 );

        // Bottom-right vertex (corner)
        glTexCoord2i( 1, 1 );
        glVertex3f( w, h, 0.0 );

        // Top-right vertex (corner)
        glTexCoord2i( 1, 0 );
        glVertex3f( w, 0, 0.0 );
        glEnd();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

//return 1 if end of anim reached
int opengl_blit_anim(LBXAnimation_t * anim, SDL_Rect * rect, int start, int end)
{
	Uint32 time = SDL_GetTicks();

	opengl_blit_frame(anim,rect,anim->current_frame);
	if( anim->prev_time == 0 ) {
		anim->prev_time = time;
	}
	if( time >= anim->prev_time + anim->delay) {
		(anim->current_frame)++;
		anim->prev_time = time;
		if( end != -1 ) {
			if(anim->current_frame >= end) {
				anim->current_frame = start;
				return 1;
			}
		}
		else {
			if(anim->current_frame >= anim->num_frame) {
				anim->current_frame = 0;
				return 1;
			}
		}
	}

	return 0;
}

int opengl_blit_item(item_t * item)
{
	
	if( item->frame_normal == -1 ) {
		return opengl_blit_anim(item->anim,&item->rect,item->anim_start,item->anim_end);
	}
	else{
		opengl_blit_frame(item->anim,&item->rect,item->current_frame);
	}

	if( item->font != NULL && item->string != NULL ) {
		sdl_print_item(item,item->font,item->string);
	}

	return 0;
}

/* Transpose coordinate from physical screen space to game screen space */
void opengl_screen_coord(SDL_Rect * rect)
{
	double ax = (double)rect->x;
	double ay = (double)rect->y;

	ax = (ax - cx) / cw * (double)ORIGINAL_GAME_WIDTH;
	ay = (ay - cy) / ch * (double)ORIGINAL_GAME_HEIGHT;

	rect->x = (int)ax;
	rect->y = (int)ay;
}

void opengl_blit_to_screen()
{
	int w = sdl_get_win_w();
	int h = sdl_get_win_h();

	cw = (double)w;
	ch = (double)h;
	cx = 0.0;
	cy = 0.0;

	/* take care of aspect ratio */
	if(cw/ch > DEFAULT_ASPECT_RATIO) {
		cw = ch * DEFAULT_ASPECT_RATIO;
		cx = ((double)w-cw) /2.0;
	}
	else {
		ch = cw / DEFAULT_ASPECT_RATIO;
		cy = ((double)h-ch) /2.0;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        glViewport( cx, cy, cw, ch );

	opengl_clear_screen();
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, render_tex);

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho(0, cw, ch, 0, 1, -1);

	glTranslatef(0,0, 0.0);

	glBegin( GL_QUADS );
	glTexCoord2i( 0, 1 );
        glVertex3f( 0.0, 0.0, 0.0 );

	glTexCoord2i( 0, 0 );
	glVertex3f( 0.0, ch, 0.0 );

	glTexCoord2i( 1, 0 );
	glVertex3f( cw, ch, 0.0 );

	glTexCoord2i( 1, 1 );
	glVertex3f( cw, 0, 0.0 );
	glEnd();

	SDL_GL_SwapBuffers();
}

void opengl_blit_item_list(item_t * list, int num)
{
	int i;

	for(i=0;i<num;i++) {
		opengl_blit_item(&list[i]);
	}
}
