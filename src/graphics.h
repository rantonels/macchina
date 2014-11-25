#ifndef __GRAPHICS_H_
#define __GRAPHICS_H_

#include "boardrep.h"

void startupWindow();

void graphTile(int x, int y, unsigned char tileval);

unsigned char graphFlip(unsigned char v, bool flip);
void graphBoard(State * sp, bool flip = false);

void graphPointers(Move m);

void graphRefresh();

void graphCleanup();

#endif
