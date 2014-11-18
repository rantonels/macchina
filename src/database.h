#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "boardrep.h"
#include <map>
#include <array>

class compare_boards {
	public:
		bool operator()(Cell* x, Cell* y) 
		{
			for (int i=0;i<32;i++)
			{
				if(x[i]<y[i])
					return true;
				if(x[i]>y[i])
					return false;
			}
		}

};

struct Dbentry {
	char depth;
	strategy strat;
};

typedef map<array<Cell,32>, Dbentry> dbtype;

class Database {
	public:
		dbtype db;
		Database();
		strategy query(State *s);
		void insert(State s, char depth, strategy strat, bool force);
		void push();
		void pull();
		void generate_openings(int depth);

};

extern Database database;


#endif
