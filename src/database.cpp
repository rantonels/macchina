#include "database.h"
#include <fstream>

//----DB IN/OUT--------

const char* DBFILE = "db/database";

Database database;

strategy Database::query(State *s)
{
	strategy outs;
	array<Cell,32> ar;
	for (int i=0; i<32; i++)
		ar[i] = s->data[i];


	dbtype::iterator it = db.find(ar);

	if(it == db.end())
	{
		outs.value=-INFTY;
		outs.optimal=Move(1,NOT_DATABASE);
	}
	else
	{
		outs = (*it).second.strat;
	}

	return outs;

};

void Database::insert(State s, char depth, strategy strat, bool force=false)
{
	array<Cell,32> ar;
	for (int i=0; i<32; i++)
		ar[i] = s.data[i];

	dbtype::iterator it = db.find(ar);
	if((it == db.end()) or (force or  ((*it).second.depth < depth)))
	{

		if(it != db.end())
			db.erase(it);	

		Dbentry n;
		n.depth = depth;
		n.strat = strat;

		//		Cell *o = new Cell[32];
		//		for (int i=0;i<32;i++)
		//			o[i] = s.data[i];

		db[ar] = n;
	}
	else
	{
		//		cout << "WARNING: insert aborted." << endl;
	}
};

Database::Database() //basic db setup
{
	db.clear();

	//ol' faithful
	State s;
	s.setup();
	Move oldf;
	oldf.push_back(21);oldf.push_back(17);
	strategy oldfs;
	oldfs.optimal = oldf;
	insert(s,99,oldfs);

}



void Database::push()
{
	ofstream ofile;
	ofile.open(DBFILE, ios::out | ios::trunc | ios::binary);

	dbtype::iterator it;

	char buffer[16];

	for(it = db.begin(); it != db.end(); it++)
	{
		//Il file e' diviso in settori da 64 byte.
		//I primi 32 byte sono allocati per la board (e' uno spreco, ma e' veloce)
		ofile.write( (*it).first.data(), 32);

		//Poi 16 byte alla mossa ottimale
		for (int i=0; i<16; i++)
			buffer[i] = ENDMOVE_CARRIAGE;
		for (int i=0; i<(*it).second.strat.optimal.size(); i++)
			buffer[i] = (*it).second.strat.optimal[i];
		ofile.write(buffer,16);

		//Infine 16 byte per ora vuoti
		ofile.write(buffer,16);


	}

	ofile.close();

}

void Database::pull()
{
	ifstream ifile;
	ifile.open(DBFILE, ios::in | ios::binary);

	db.clear();

	char buffer[64];

	while(true)
	{
		ifile.read(buffer,64);
		if(not ifile)
		{
			if (ifile.gcount()>0)
				cout << "WARNING: STRAY CHARS " << ifile.gcount() << endl;
			break;
		}
		array<Cell,32> br;

		for(int i=0; i<32; i++)
			br[i] = buffer[i];

		Move m;
		for (int j=0; j<16; j++)
		{
			if (buffer[32+j]==ENDMOVE_CARRIAGE)
				break;
			m.push_back(buffer[32+j]);
		}

		strategy ott;
		ott.value=99;
		ott.optimal = m;
		Dbentry n;
		n.strat = ott;
		n.depth = 11;		

		db[br] = n;
	}

	ifile.close();
}
