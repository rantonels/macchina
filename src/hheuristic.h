#ifndef __HHEURISTIC_H__
#define __HHEURISTIC_H__

#include <stdint.h>
#include "boardrep.h"

class Hheur {
	public:
		uint32_t value[64][64];

		Hheur();
		void clear();
};

extern Hheur hheur;

uint32_t evalhheur (Move m);

bool sorthheur(Move i, Move j);

#endif
