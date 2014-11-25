#ifndef __ENGINE_H
#define __ENGINE_H


#include "boardrep.h"
#include "ttable.h"
#include "hheuristic.h"
#include "database.h"
#include <algorithm>
#include <stdint.h>
#include <ncurses.h>


const int PIECEVAL = 10;

static unsigned char PVAL = 10;
static unsigned char QUEENVAL = 25;
static unsigned char FROWVAL = 4;
static unsigned char CENTERVAL = 1;


const unsigned char	M_ROOT=0x01;
const unsigned char	M_GRAPH=0x02;
const unsigned char	M_GRAPHCOLOR=0x04;


uint32_t pack_genome(char PV,char QUEENV, char FROMV, char CENTERV);


void unpack_genome(uint32_t genome);

string genrep(uint32_t genome);

//--EVALUATION FUNCTION--

string valrep(float val);

int center(int c);




strategy compute( State *original, bool turn, int depth, unsigned char mode=M_ROOT, float alphabetalim=-INFTY);


#endif
