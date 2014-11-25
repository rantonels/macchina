#include "utils.h"


uint32_t rand32(){

	uint32_t x;
	x = rand() & 0xff;
	x |= (rand() & 0xff) << 8;
	x |= (rand() & 0xff) << 16;
	x |= (rand() & 0xff) << 24;

	return x;
}

int log2(int n)
{
	int t = n;

	if(t==0)
		return -1;

	int out = 0;
	while(t>>=1)
		++out;
	return out;
}
