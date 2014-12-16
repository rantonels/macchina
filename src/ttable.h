#ifndef __TTABLE_H__
#define __TTABLE_H__

#include "utils.h"

const int TTBSIZE = 65536;

struct htentry {
	bool active;
	uint32_t hash;
	char depth;
	State state;
	strategy optimal;
};

class Htable {
	public:
		Htable();	
			
		//[posizione][tipopedina]
		uint32_t rnums[32][16];
		void init_rng();
		uint32_t hash(State * sp);

		int accesscounter;

		htentry table[TTBSIZE];	
		void clear();		
	private:
		uint32_t hash3232(uint32_t x);

};

extern Htable hata;


#endif
