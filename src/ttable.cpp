#include "boardrep.h"
#include "ttable.h"



Htable hata;

Htable::Htable()
{
	init_rng();
	clear();
	accesscounter = 0;
}

void Htable::clear()
{
	accesscounter = 0;
	for (int i =0; i<TTBSIZE; i++)
		table[i].active = false;
}

void Htable::init_rng()
{
	srand (static_cast <unsigned> (time(0)));	
	for (int i=0; i<32; i++) for(int j=0; j<16; j++)
	{
		uint32_t x = rand32();
		rnums[i][j] = x;
	}
}

uint32_t Htable::hash(State * sp)
{
	uint32_t h = 0;

	for (int i=0; i<32; i++)
	{
		h = h xor (rnums[i][ sp->data[i] ]);
	}

	h = h xor (rnums[7][ (sp->turn)%16]);
	h = h xor (rnums[9][ 7*(sp->draw)]);

	return h;
}
