#!/bin/bash

astyle -r --indent=force-tab  --style=1tbs "*.c"
astyle -r --indent=force-tab  --style=1tbs "*.h"

#sudo apt-get install libsdl-mixer1.2-dev

CFLAGS=`sdl2-config --cflags`
LIBS=`sdl2-config --libs`

g++ -g -O0 -Wall -Wno-write-strings -o omom main.c lbx.c sdl.c scr_intro.c scr_start.c item.c scr_newgame.c game.c exe.c terrain.cpp scr_play.c scr_city.c llist.c scr_combat.c $CFLAGS $LIBS -lconfig -lGL -lnoise -lSDL_mixer
 
#tools
gcc -g -O0 -Wall -o tools/lbx_decode tools/lbx_decode.c lbx.c sdl.c $CFLAGS $LIBS -lGL -lSDL_mixer
