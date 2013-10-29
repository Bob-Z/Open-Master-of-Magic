#!/bin/bash

#sudo apt-get install libsdl-mixer1.2-dev

CFLAGS=`pkg-config --cflags sdl`
LIBS=`pkg-config --libs sdl`

g++ -g -O0 -Wall -Wno-write-strings -o omom main.c lbx.c sdl.c opengl.c scr_intro.c scr_start.c item.c scr_newgame.c game.c exe.c terrain.cpp scr_play.c scr_city.c llist.c scr_combat.c $CFLAGS $LIBS -lconfig -lGL -lnoise -lSDL_mixer
 
#tools
gcc -g -O0 -Wall -o tools/lbx_decode tools/lbx_decode.c lbx.c sdl.c opengl.c $CFLAGS $LIBS -lGL -lSDL_mixer
