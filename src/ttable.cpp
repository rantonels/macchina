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
	{
		table[i].active = false;
		table[i].depth = 111;
		table[i].hash = 0;
	}
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

//uint32_t Htable::hash(State * sp)
//{
	//uint32_t h = 0;

	//for (int i=0; i<32; i++)
	//{
		//h = h xor (rnums[i][ sp->data[i] ]);
	//}

	//h = h xor (rnums[7][ (sp->turn)%16]);
	//h = h xor (rnums[9][ 7*(sp->draw)]);

	//return h;
//}

uint32_t Htable::hash3232(uint32_t x)
{
	
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x);
    return x;

}

uint32_t Htable::hash(State * sp)
{
		uint32_t ph = hash3232(sp->data.PMASK);
		uint32_t bh = hash3232((sp->data.PMASK)&(sp->data.BMASK));
		uint32_t kh = hash3232((sp->data.PMASK)&(sp->data.KMASK) );
	return ph ^ bh ^ kh;
}
