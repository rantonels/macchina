using namespace std;

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <map>
#include "tree.hh"
#include <limits>
#include <time.h>
#include <ncurses.h> 
//#include <locale.h>
#include "boost/program_options.hpp" 
#include <stack>
#include <fstream>
#include <array>
#include <stdint.h>
#include <bitset>

// OPZIONI
namespace po = boost::program_options;

po::variables_map vm;

void parse_options(int ac, char* av[]){

	po::options_description desc("Options");
	desc.add_options()
		("help", "print help message")
		("gui,g", "run GUI (default)")
		("openings,o", "generate openings database (use with -d)")
		("depth,d", po::value<int>()->default_value(11), "set search depth for cmdline computation")
		("ptest", "run a test for profiling")
		("evolution","enter evolution mode")
		;

	//	po::variables_map vm;
	po::store(po::parse_command_line(ac,av,desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		exit(0);
	}


};

//UTILITA'

uint32_t rand32(){

	uint32_t x;
	x = rand() & 0xff;
	x |= (rand() & 0xff) << 8;
	x |= (rand() & 0xff) << 16;
	x |= (rand() & 0xff) << 24;

	return x;
}

//PARAMETRI GLOBALI

const int MAXSTACKSIZE = 30; //dimensione massima stack di mosse di sicurezza per evitare ripetizioni


	template <typename T>
string NToS ( T Number )
{
	stringstream ss;
	ss << Number;
	return ss.str();
}

char NToC (int n)
{
	return (char)('0' + n);
}


//definizione del tipo di cella

typedef char Cell;

const char VOID=0;
const char WHITE=1;
const char BLACK=2;
const char WQUEEN=3;
const char BQUEEN=4;

//white mask : (n || 0x01)
//black mask : (n-1) || 0x01

//def tipo di mossa

typedef vector<char> Move;


//codici mossa speciali

const char SURRENDER = 32;
const char NO_MOVES = 33;
const char PROPOSE_DRAW = 34;
const char ACCEPT_DRAW = 35;
const char NOT_DATABASE = 36;
const char ENDMOVE_CARRIAGE = 37;




//overloading == (non necessario, built in!)
//bool Move::operator==(const Move &other) const {
//    	if(size() != other.size())
//		return 0;
//	for(int i=0; i<size(); i++)
//	{
//		if 
//	}
//}


//printare una mossa

string cellrep(char n)
{
	int row = 8-n/4;
	int column = (n/4+1)%2 + 2*(n%4);
	string s = string(1,char('a'+column)) + NToS(row);
	return s;
}



string moverep(Move m)
{
	if (m[0] == ACCEPT_DRAW)
		return "Accepts Draw.";

	string out;
	if (m[0] == SURRENDER)
	{
		return "Surrenders.";
	};
	if (m[0] == NO_MOVES)
	{
		return "No moves available.";
	};

	if (m[0] >= 32)
		return "?";

	string suffix = "";
	if (m.back() == PROPOSE_DRAW)
	{
		suffix = " (prop. draw)";
		m.pop_back();
	}



	if (m.size() == 2)
	{
		out =  cellrep(m[0]) + "-" + cellrep(m[1]);
	}
	else
	{
		out = "";
		for(Move::iterator it = m.begin(); it != m.end(); ++it) {
			out +=  cellrep(*it) + " ";}
	};
	return out + suffix;
};

void printmove(Move m)
{
	cout << moverep(m) << endl;
}


//parsing posizione
char parsepos(string tb)
{
	cout << "parsing " << tb << "..." << endl;
	int column = (tb[0]-(int)'A')/2;
	int row = 8-(int(tb[1]) - (int)'1'+1);

	return (char)(4*row + column);

}

//parsing mossa
Move parsemove(string ss)
{
	Move m;

	istringstream stream(ss);
	string word;

	while(getline(stream,word,','))
	{
		m.push_back(parsepos(word));
	}

	return m;

}

//infinito

const int INFTY = INT_MAX;
const int INFTIME = INT_MAX - 1;

//lookup table per gli spostamenti in avanti di uno

const char NULLP[2] = {99,99};

const char FORW[32][2] = {
	{99,99}, //0
	{99,99}, //1
	{99,99}, //2
	{99,99}, //3
	{0,99}, //4
	{0,1}, //5
	{1,2}, //6
	{2,3}, //7
	{4,5}, //8
	{5,6}, //9
	{6,7}, //10
	{7,99}, //11
	{8,99}, //12
	{8,9}, //13
	{9,10}, //14
	{10,11}, //15
	{12,13}, //16
	{13,14}, //17
	{14,15}, //18
	{15,99}, //19
	{16,99}, //20
	{16,17}, //21
	{17,18}, //22
	{18,19}, //23
	{20,21}, //24
	{21,22}, //25
	{22,23}, //26
	{23,99}, //27
	{24,99}, //28
	{24,25}, //29
	{25,26}, //30
	{26,27} //31
};

//lookup table per gli spostamenti in ogni direzione di uno

const char KING[32][4] = {
	{99,99,5,4}, //0
	{99,99,6,5}, //1
	{99,99,7,6}, //2
	{99,99,7,99}, //3
	{0,99,8,99}, //4
	{0,1,9,8}, //5
	{1,2,10,9}, //6
	{2,3,11,10}, //7
	{4,5,13,12}, //8
	{5,6,14,13}, //9
	{6,7,15,14}, //10
	{7,99,15,99}, //11
	{8,99,16,99}, //12
	{8,9,17,16}, //13
	{9,10,18,17}, //14
	{10,11,19,18}, //15
	{12,13,21,20}, //16
	{13,14,22,21}, //17
	{14,15,23,22}, //18
	{15,99,23,99}, //19
	{16,99,24,99}, //20
	{16,17,25,24}, //21
	{17,18,26,25}, //22
	{18,19,27,26}, //23
	{20,21,29,28}, //24
	{21,22,30,29}, //25
	{22,23,31,30}, //26
	{23,99,31,99}, //27
	{24,99,99,99}, //28
	{24,25,99,99}, //29
	{25,26,99,99}, //30
	{26,27,99,99} //31
};


//lookup table per i salti (primi due: pedina)

const char SJUMP[32][4] = {
	{99,99,99,9},	 //0
	{99,99,8,10},	 //1
	{99,99,9,11},	 //2
	{99,99,10,99},	 //3
	{99,99,99,13},	 //4
	{99,99,12,14},	 //5
	{99,99,13,15},	 //6
	{99,99,14,99},	 //7
	{99,1,99,17},	 //8
	{0,2,16,18},	 //9
	{1,3,17,19},	 //10
	{2,99,18,99},	 //11
	{99,5,99,21},	 //12
	{4,6,20,22},	 //13
	{5,7,21,23},	 //14
	{6,99,22,99},	 //15
	{99,9,99,25},	 //16
	{8,10,24,26},	 //17
	{9,11,25,27},	 //18
	{10,99,26,99},	 //19
	{99,13,99,29},	 //20
	{12,14,28,30},	 //21
	{13,15,29,31},	 //22
	{14,99,30,99},	 //23
	{99,17,99,99},	 //24
	{16,18,99,99},	 //25
	{17,19,99,99},	 //26
	{18,99,99,99},	 //27
	{99,21,99,99},	 //28
	{20,22,99,99},	 //29
	{21,23,99,99},	 //30
	{22,99,99,99}	 //31

};



char cellascii(Cell c)
{
	switch(c)
	{
		case VOID: return ' ';
		case WHITE: return 'w';
		case BLACK: return 'b';
		case WQUEEN: return 'W';
		case BQUEEN: return 'B';
		default: return '?';
	}
};


struct strategy {
	float value;
	Move optimal;

};


class State {
	public:
		State();
		Cell data[32];
		vector<Move> movestack;
		bool draw;
		char turn;

		void copyfrom(State *s);
		void drawascii();
		vector<Move> raw_movelist();
		void clear();
		void setup();
		void apply_move(Move m);
		void flip();
		int value_function();
		void random();
};

void State::random()
{
	for(int i=0; i<32; i++)
		data[i] = (rand()%5)*(rand()%2)*(rand()%2);
}

void State::copyfrom(State* s)
{
	for(int i=0;i<32;i++)
		data[i] = s->data[i];
	movestack = s->movestack;
	draw = s->draw;
	turn = s->turn;
}

State::State() {

};

void State::setup() {
	for (int i=0; i<12; i++)
		data[i] = BLACK;
	for (int i=12; i<20; i++)
		data[i] = VOID;
	for (int i=20; i<32; i++)
		data[i] = WHITE;
	draw = false;
	turn = 0;
};

void State::clear() {
	for (int i=0; i<32; i++)
		data[i] = VOID;
	draw=false;
}

void State::drawascii() {

	cout << " A B C D E F G H" << endl;	
	cout << "|-|-|-|-|-|-|-|-|" << endl;
	for (int j=0; j<8; j++)
	{
		if (not (j%2))
			cout << "| ";
		for (int i=0; i<4; i++)
		{
			cout << "|" << cellascii(data[4*j+i]);
			if (i<3)
				cout << "| ";
		}
		if ( (j%2))
			cout << "| |";
		else
			cout << "|";
		cout << 8-j << endl;
		cout << "|-|-|-|-|-|-|-|-|" << endl;
	};
};

void State::apply_move(Move m)
{
	turn++;
	if(draw and m[0] == ACCEPT_DRAW)
		return;


	draw = false;	

	if(m.back() == PROPOSE_DRAW)
	{
		draw=true;
		m.pop_back();
	}


	if(m[0] >= 32)
		return;	

	//	if(m.size()<=1)
	//		{return;}

	//	for (Move::iterator it = m.begin(); it != m.end(); it++)
	//		if ((*it) >= 32)
	//		{cout << int(*it) << endl; exit(1);};
	if((m.size() > 2) or (abs(m[0]-m[1]) >=6)) //questi sono i salti
	{
		//clear stack
		movestack = vector<Move>();

		Move::iterator prev = m.begin();
		for(Move::iterator it = m.begin(); it != m.end()-1; ++it)
		{
			char from = (*it);
			char toc = (*(it+1));
			char jumpd = (from + toc + 1 - (from/4)%2)/2;
			data[jumpd] = VOID;
		}
		data[m.back()] = data[m[0]];
		data[m[0]] = VOID;
		if (m.back()<=3)
			data[m.back()] = WQUEEN;
	}
	else
	{
		//se e' un pezzo normale, pulisci lo stack
		if (data[m[0]] == WHITE)
			movestack = vector<Move>();
		else
		{
			//	Cell *k = find(data,data+32,WQUEEN);
			//	if (k != data+32)
			movestack.push_back(m);
		}


		data[m[1]] = data[m[0]];
		data[m[0]] = VOID;
		if (m[1] <= 3)
			data[m[1]] = WQUEEN;
	}

	if(movestack.size()>MAXSTACKSIZE)
	{
		movestack.erase(movestack.begin());
	}

}

const char fliplookup [8] = {
	VOID,
	BLACK,
	WHITE,
	BQUEEN,
	WQUEEN,
	0,0,0
};

char flipdame(char d)
{
	return fliplookup[d];
	//switch(d)
	//{
	//case VOID: return VOID;break;
	//case WHITE: return BLACK;break;
	//case BLACK: return WHITE;break;
	//case WQUEEN: return BQUEEN;break;
	//case BQUEEN: return WQUEEN;break;
	//};
	//return VOID;
}

void State::flip()
{
	//char tmp[32];
	//for (int i=0; i<32; i++)
	//tmp[i] = data[31-i];
	//for (int j=0; j<32; j++)
	//data[j] = (Cell)flipdame(tmp[j]);

	char tmp;
	for (int i=0; i<16; i++)
	{
		tmp = data[i];
		data[i] = flipdame(data[31-i]);
		data[31-i] = flipdame(tmp);
	}

}

Move flipmove(Move m)
{
	Move out;
	for(Move::iterator it=m.begin(); it!=m.end(); it++)
	{
		if ((*it) <= 31)
			out.push_back(31-(*it));
		else
			out.push_back(*it);
	}
	return out;
}

Move mpair(char from, char to)
{

	Move m;
	m.push_back(from);
	m.push_back(to);	
	return m;
}

bool isbeatable(int nlen, Cell other)
{
	if (other == BLACK)
		return true;
	if ((other == BQUEEN) and (nlen ==4))
		return true;

	return false;

}

bool isblack(Cell cc)
{
	return (not(cc & 0x01)) and (cc > 0);
}

struct jumppos {
	char cell;
	bool done;
};


vector<Move> State::raw_movelist()
{

	vector<Move> outlist;
	outlist.reserve(7);



	//generazione salti
	for (int i=0;i<32;i++)
	{
		if (data[i] & 0x01)  //((data[i] == WHITE) or (data[i] == WQUEEN))
		{
			//	cout << "PROCESSING CELL" << cellrep(i) << endl;
			//un numero utile: 2 per le pedine, 4 per le dame
			int nlen = data[i] + 1;    // 2*(data[i]==WHITE)+4*(data[i]==WQUEEN);        (le comparison sono il male)
			//	cout << "nlen = "<< nlen << endl;

			//check preliminare per risparmiare
			bool tuttoinutile = true;
			for (int j=0;j<nlen;j++)
			{
				if(nlen==2)
					if (isblack(data[FORW[i][j]])) //((data[FORW[i][j]] == BLACK) or (data[FORW[i][j]] == BQUEEN))
					{tuttoinutile = false; break;}
				if (nlen==4)
					if (isblack(data[KING[i][j]]))
					{tuttoinutile = false;break;}
				//	if ((nlen!=2) and (nlen!=4))
				//		cout << "WARNING: unvalid piece has leaked into preliminary check" << endl;
			}
			if (tuttoinutile)
				continue;

			//	cout << "check preliminare passato" << endl;


			//qui si dovrebbe pensare a generare i salti per la postazione i

			//	cout << "preparazione albero di iterazione" << endl;

			//preparo l'albero con il tronco
			tree<jumppos> jtree;
			tree<jumppos>::iterator top, beg;
			tree<jumppos>::leaf_iterator leaf, endleaf;
			top=jtree.begin();
			jumppos startpos;
			startpos.cell = i;
			startpos.done = false;
			beg = jtree.insert(top,startpos);

			//	cout << "pronto" << endl;



			//loop principale di costruzione dell'albero

			int control;
			for(control=0; control<50; control++)//contatore di controllo per evitare loop infiniti
			{
				//		cout << "CICLO ITERATIVO." << endl;

				bool exitv = false;

				//itera fra le foglie per trovare una non processata
				leaf = jtree.begin_leaf();
				endleaf = jtree.end_leaf();
				while(true)
				{
					if (not (leaf->done))
						break;

					leaf++;

					if(leaf==endleaf)
					{
						exitv = true;
						break;
					}

				}


				//esci se non ci sono più foglie da processare
				if (exitv)
				{
					//			cout << "fine foglie non fatte." << endl;
					break;
				}


				//		cout << "prima foglia non fatta: " << cellrep(leaf->cell) << endl;

				//genera le destinazioni successive e aggiungi all'albero
				char s = (leaf->cell);
				int destcounter = 0;

				//ma prima, prepara una lista di pedine gia' mangiate

				tree<jumppos>::iterator track = leaf;
				vector<char> blacklist;
				while(track!=beg)
				{
					char t = track->cell;
					char c = jtree.parent(track)->cell;
					blacklist.push_back((c + t + 1 - (t/4)%2)/2);

					track = jtree.parent(track);

				}


				for (int d=0; d<nlen; d++)
				{
					char dest = SJUMP[s][d];
					if (dest == 99)
						continue;
					//			cout << "considering destination" << int(dest) << endl;

					char eventualjump = (s + dest + 1 - (s/4)%2)/2;

					if (find(blacklist.begin(), blacklist.end(), eventualjump)!=blacklist.end())//ignora la mangiata se e' nella blacklist
						continue;


					//se il salto c'e', aggiungi all'albero
					if ((data[dest] == VOID) and isbeatable(nlen,data[ (s+dest+1-(s/4)%2)/2 ]))
					{
						jumppos ne;
						ne.cell = dest;
						ne.done = false;
						jtree.append_child(leaf,ne);
						destcounter ++;
						//				cout << "destination added." << endl;
					}
				}

				(*leaf).done = true;

				//		cout << "dalla cella" << cellrep(leaf->cell) << " abbiamo" << destcounter << " destinazioni." << endl;

				if (control == 49)
					cout << "WARNING: reached critical control counter in tree generation loop." << endl;

			};

			//	cout << "albero costruito." << endl;

			//ricostruisci i percorsi a partire dalle foglie e aggiungili alle mosse

			if (control == 0)
				continue;

			leaf = jtree.begin_leaf();
			endleaf = jtree.end_leaf();

			while(leaf != endleaf)
			{
				//		cout << "* ricostruisco dalla cella" << cellrep(leaf->cell) << endl;
				Move m;
				m.reserve(jtree.depth(leaf)+1);
				tree<jumppos>::iterator backtrans = leaf;
				while (true)				
				{
					//			cout << i << ": " << cellrep(backtrans->cell) << endl;
					m.insert(m.begin(), backtrans->cell );
					if (backtrans != beg)
						backtrans = jtree.parent(backtrans);
					else
						break;
				}
				if (m.size() > 1)
					outlist.push_back(m);
				leaf++;
			}



		}//fine if bianco
	}//fine ciclo sulle caselle

	//se c'e' almeno un salto, niente mosse normali
	if (outlist.size()>0)
		return outlist;


	//generazione lista mosse normali
	int counter = 0;
	for (int i=0;i<32;i++)
	{
		counter = 0;
		switch(data[i])
		{ 
			case WHITE: //if there's a white piece

				for(int k=0;k<2;k++)
					if ((FORW[i][k] != 99) && (data[FORW[i][k]] == VOID))
					{
						Move m = mpair(i,FORW[i][k]);


						outlist.push_back(m);
						counter ++;
					}


				break;
			case WQUEEN: //if there's a white king
				for (int j=0; j<4; j++)
				{
					if ((KING[i][j] != 99) && (data[KING[i][j]] == VOID))
					{
						Move m = mpair(i,KING[i][j]);


						//cerca mossa nello stack
						int reps = count(movestack.begin(), movestack.end(), m);
						if (reps > 1)
						{
							continue;
						}	

						outlist.push_back(m);
						counter++;
					}
				}
				break;
		}


	};


	if(draw)
	{
		outlist.push_back(Move(1,ACCEPT_DRAW));
	}



	return outlist;
}

//HISTORY EURISTIC

class Hheur {
	public:
		uint32_t value[64][64];

		Hheur();
		void clear();
} hheur;

void Hheur::clear()
{
	for(int i=0;i<64;i++)
		for(int j=0;j<64;j++)
			value[i][j] = 0;
}

Hheur::Hheur()
{
	clear();
}

uint32_t evalhheur (Move m) { return hheur.value[m.front()][m.back()]; }

bool sorthheur (Move i, Move j) { return (evalhheur(i)>evalhheur(j)); }


//----DB IN/OUT--------

const char* DBFILE = "db/database";

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

} database;

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

//--GENETIC CODE

const int PIECEVAL = 10;

unsigned char PVAL = 10;
unsigned char QUEENVAL = 25;
unsigned char FROWVAL = 4;
unsigned char CENTERVAL = 1;

uint32_t pack_genome(char PV,char QUEENV, char FROMV, char CENTERV)
{
	uint32_t out =	 (int)PV +
		((int)QUEENV	<< 8) +
		((int)FROMV	<< 16) +
		((int)CENTERV << 24);
	return out;
}


void unpack_genome(uint32_t genome)
{
	PVAL 		= (genome & 0x000000FF);
	QUEENVAL	= (genome & 0x0000FF00)>>8;
	FROWVAL		= (genome & 0x00FF0000)>>16;
	CENTERVAL	= (genome & 0xFF000000)>>24;
}

string genrep(uint32_t genome)
{
	char buffer[8];
	sprintf(buffer,"%08X",genome);
	string out = buffer;
	return out;
}

//--EVALUATION FUNCTION--

string valrep(float val)
{
	if (val>=100000)
		return "Win.";
	if (val<=-100000)
		return "Loss.";

	int v = val;///10.0;
	char buffer[10];

	sprintf(buffer,"%d",v);

	return string(buffer);
}

int center(int c)
{
	return - (c*(c-3) )/2;
}


int State::value_function()
{
	if(turn >= 100) //fine tempo
	{
		int wsum = 0;
		int bsum = 0;
		for (int i=0; i<32; i++)
		{
			wsum += (data[i]==WHITE)+2*(data[i]==WQUEEN);
			bsum += (data[i]==BLACK)+2*(data[i]==BQUEEN);
		}
		if (wsum>bsum)
			return INFTIME;
		else
			if(bsum>wsum)
				return -INFTIME;
			else
				return 0;

	}

	int val = 0;
	int b=0;
	int n=0;
	int tmp;
	for (int i=0; i<32; i++)
	{
		Cell d = data[i];
		//conteggio bianchi
		tmp = (PIECEVAL+FROWVAL*((i/4)==7))*(d == WHITE) + QUEENVAL*(d == WQUEEN);
		b += 10*tmp + CENTERVAL*center(i%4)*(tmp/10); //moltiplica per 1.1

		//conteggio neri
		tmp = (PIECEVAL+FROWVAL*((i/4)==0))*(d == BLACK) + QUEENVAL*(d == BQUEEN);
		n += 10*tmp + CENTERVAL*center(i%4)*(tmp/10); //moltiplica per 1.1
	}	

	//-infinito se non ci sono piu' bianchi
	if (b == 0)
		return -INFTY; 

	val += b;

	//+infinito se non ci sono piu' neri
	if (b == 0)
		return INFTY; 

	val -= n;

	return val;

}

//--TRANSPOSITION TABLE--

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

} hata;

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






//----MINIMAX----

const unsigned char	M_ROOT=0x01;
const unsigned char	M_GRAPH=0x02;
const unsigned char	M_GRAPHCOLOR=0x04;

//la funzione ritorna il punteggio stimato sempre per il giocatore turn!

strategy compute( State *original, bool turn, int depth, unsigned char mode=M_ROOT, float alphabetalim=-INFTY)
{
	//cout << "depth: " << depth << endl;
	//la board e' flippata da dopo l'ingresso a dopo la generazione delle mosse
	if (turn==1) //turn == 0 bianco; turn == 1 nero
		original->flip();


	//se siamo su una foglia dell'albero di iterazione
	if (depth==0)
	{
		strategy s;
		s.value = original->value_function(); //le valutazioni sono fatte sulla board flippata e dunque valgono per turn
		return s;
	}

	//cout << "beyond flipping & leaf test" << endl;


	//operazioni preliminari sul nodo root

	if((mode & M_ROOT)==M_ROOT)
	{


		//cerca nel database
		strategy res = database.query(original);
		if(not (res.optimal[0] == NOT_DATABASE))
		{
			if ((mode & M_GRAPH)==M_GRAPH)
			{
				move(1,20);
				addstr(("Found in DB. ("+valrep(res.value)+")").c_str());
			}
			return res;
		}


		//pulizia auto HT
		if (hata.accesscounter >= (TTBSIZE>>2))
		{
			hata.clear();
		}

	}
	else
	{
		//operazioni preliminari sul nodi non root

		//cout << "pronto per la ricerca in ht..." << endl;
		//ricerca in hashtable
		uint32_t h = hata.hash(original);
		if((hata.table[h%TTBSIZE].active) and (hata.table[h%TTBSIZE].depth >= depth) and (hata.table[h%TTBSIZE].hash == h) )
		{

			//trovato nella hashtable. Ricopiare semplicemente
			//cout << "found in ht" << endl;
			hata.accesscounter++;
			return hata.table[h%TTBSIZE].optimal;

		}
		//cout << "fine ricerca in ht..." << endl;

	}


	//generazione movestack
	vector<Move> ls = original->raw_movelist();

	//cout << "moveset generated" << endl;

	//se non ci sono mosse
	if (ls.size() == 0) //movearray vuoto vuol dire o perdita o stallo
	{
		strategy s;
		s.value = original->value_function();
		if (s.value != - INFTY) //se non abbiamo perso, deve essere stallo
		{
			s.value = 0; //lo stallo ha punteggio 0
			s.optimal.insert(s.optimal.begin(),  NO_MOVES);
		}

		return s;
	}

	//cout << "beyond no moves" << endl;

	//contatore di alfa-beta
	float  abcounter = INFTY;

	//branching
	float * points;
	points = new float[ls.size()];

	//history heuristic sort
	sort(ls.begin(),ls.end(),sorthheur);

	

	//preparazione display mosse
	if((mode & M_GRAPH) == M_GRAPH)
	{

		char col = ((mode & M_GRAPHCOLOR) == M_GRAPHCOLOR);
		attron(COLOR_PAIR(col));
		for (int i=0; i<20; i++){
			move(1+i,20);
			addstr("                   ");};

		for (int i=0; i<ls.size(); i++)
		{
			move(1+i,27);
			addstr(moverep(ls[i]).c_str());
		};
		attroff(COLOR_PAIR(col));
		refresh();
	};


	for(int i=0; i<ls.size(); i++)
	{
		//	cout << "branch " << i << ": ";
		//	printmove(ls[i]);
		if(ls[i][0] == ACCEPT_DRAW)
		{
			int countpiec = 0;
			for (int o=0; o<32; o++)
				countpiec += (original->data[o] != VOID);

			points[i] = 50*(countpiec>8);
		}
		else
		{
			State tmp;
			//copy original->tmp
			tmp.copyfrom(original);

			tmp.apply_move(ls[i]);
			if (turn)
				tmp.flip();


			strategy strat = compute(&tmp, not turn, depth-1, 0 ,-abcounter);
			points[i] = strat.value;

			//update contatore di alfa-beta
			if (points[i]<abcounter)
				abcounter = points[i];

			//tronca se alfa-beta
			if (points[i]<alphabetalim)
			{ 	//CUTOFF	
				//cout << "LOL" << endl;
				//cout << moverep(ls[i]) << endl;
				//exit(0);
				hheur.value[ls[i].front()][ls[i].back()] += (0x000001 << (depth));
				break;
			}
		}

		//update display
		if((mode & M_GRAPH) == M_GRAPH)
		{
			char col = ((mode & M_GRAPHCOLOR) == M_GRAPHCOLOR);
			move(1+i,20);
			attron(COLOR_PAIR(col));
			addstr(valrep(-points[i]).c_str());
			attroff(COLOR_PAIR(col));
			refresh();
		}

	}


	// i punteggi in arrivo dalle branches sono quelli dell'avversario, quindi vanno minimizzati!

	int minindex=0;
	float minnow = INFTY;

	for (int i = 0; i<ls.size(); i++)
	{
		if (points[i] <= minnow)
		{
			minnow = points[i];
			minindex = i;
		}
	}

	//il punteggio in uscita e' l'opposto del peggior punteggio dell'avversario
	strategy out;
	out.value = - minnow;
	out.optimal = ls[minindex];

	delete[] points;

	//	cout << "points: " << out.value << endl;

	//se si perde in ogni caso, la mossa è arrendersi
	if (out.value <= -INFTIME)
	{
		out.optimal[0] = SURRENDER;
	}


	if (out.value < INFTIME) //se è sicuro di vincere, non dovrebbe proporre la patta
	{
		//conteggio endgame sole dame
		int dcountw = 0;
		int dcountb = 0;
		for (int i=0; i<32; i++)
		{
			if ((original->data[i] == WHITE))
			{ dcountw = -1; break;}

			if(original->data[i] == WQUEEN)
				dcountw ++;
			if(original->data[i] == BQUEEN)
				dcountb ++;
		}
		if (dcountw != -1)
		{
			bool adddraw=false;

			if ((dcountw <= 3) and (dcountw<=dcountb))
				adddraw = true;

			if ((dcountw <= 3) and (dcountb <= 3) and (abs(dcountw - dcountb)<=1))
				if (out.value <= 0) //a meno che non sia sicuro di passare in vantaggio
					adddraw = true;

			if (adddraw)	
				out.optimal.push_back(PROPOSE_DRAW);
		}
	}

	//salva la strategia nell'hashtable
	//cout << "pronto per il salvataggio nell'ht..." << endl;
	uint32_t h = hata.hash(original);
	if( (not(hata.table[h%TTBSIZE].active)) or (hata.table[h%TTBSIZE].depth < depth) )
	{
		hata.table[h%TTBSIZE].active = true;
		hata.table[h%TTBSIZE].optimal = out;
		hata.table[h%TTBSIZE].depth = depth;
		hata.table[h%TTBSIZE].hash = h;
	};

	return out;
}






//-------GUI--------

const int MAXGUIDEPTH = 14;

void playagainst()
{
	State s;
	s.setup();

	while(true)
	{
		s.drawascii();
		cout << "Your turn." << endl;
		string ms;
		getline(cin,ms);
		Move m = parsemove(ms);

		s.apply_move(m);
		s.drawascii();

		s.flip();
		cout << "Thinking..." << endl;
		strategy os = compute(&s,0,7);
		s.apply_move(os.optimal);
		s.flip();

	}
}

void compvscomp(int wdepth, int bdepth)
{
	State s;
	s.setup();

	for (int i=0; i<100; i++)
	{
		s.drawascii();
		cout << "computing white move" << endl;
		strategy os = compute(&s,0,wdepth);
		if(os.optimal[0] == SURRENDER)
		{
			cout << "White surrenders." << endl;
			break;
		}

		s.apply_move(os.optimal);

		s.drawascii();

		s.flip();
		cout << "computing black move" << endl;
		os = compute(&s,0,bdepth);
		if(os.optimal[0] == SURRENDER)
		{
			cout << "Black surrenders." << endl;
			break;
		}



		s.apply_move(os.optimal);
		s.flip();

	}

	s.drawascii();

}



void moveton(char n)
{
	int x = 4 + 2*(n%4) + ((n/4)+1)%2 ;
	int y = 3 + n/4;
	move(y,x);
};

struct Message{
	string str;
	int col;
};

class message_holder{
	deque<Message> queue;
	public:
	void message(string m);
	void message(string m, int col);
	private:
	void drawm();
} mes;

void message_holder::drawm()
{

	for(int i=0; i<queue.size(); i++)
	{
		move(15+i,1);
		addstr("                          ");
		move(15+i,1);
		int col = queue.at(queue.size()-1-i).col;
		attron(COLOR_PAIR(col));
		addstr(queue.at(queue.size()-1-i).str.c_str());
		attroff(COLOR_PAIR(col));
	}

}

void message_holder::message(string m, int col)
{
	Message nmes;
	nmes.str = m;
	nmes.col = col;
	queue.push_back(nmes);
	if (queue.size() > 8)
		queue.pop_front();
	drawm();
}

void message_holder::message(string m)
{
	message(m,0);
}

void message(string mm)
{
	mes.message(mm);
}

class GUI
{
	int depth;	

	public:
	bool flipcolor;
	void runGUI();
	void drawback();

	uint32_t whitegenome,blackgenome;
} gui;


void GUI::drawback()
{
	move(1,3);
	addstr("          ");
	move(2,3);
	addstr(" abcdefgh ");
	for (int i=0;i<8;i++)
	{
		move(3+i,2);
		addch(NToC(8-i));
		move(3+i,13);
		addch(NToC(8-i));
	}
	move(11,3);
	addstr(" abcdefgh ");

	for (int i=0;i<15;i++)
	{
		move(1+i,20);
		addstr("                                      ");

	}

	move(1,40);
	char buff[20];
	sprintf(buff,"Depth: %d",depth);

	addstr(buff);

	move(2,40);
	sprintf(buff,"DB size: %d",database.db.size());
	addstr(buff);


	move(3,40);
	sprintf(buff,"HT accesses: %d", hata.accesscounter);
	addstr(buff);

	move(4,40);
	addstr(genrep(gui.whitegenome).c_str());
	move(5,40);
	addstr(genrep(gui.blackgenome).c_str());
}


void display(State s)
{
	int startx = 4;
	int starty = 3;

	attron(COLOR_PAIR(2));
	for(int j=0; j<64; j++)
	{
		move(starty+j/8,startx+j%8);
		addch(' ');
	}
	attroff(COLOR_PAIR(2));	

	for(int i=0; i<32;i++)
	{
		move(starty + i/4, startx + 2*(i%4) + ((i/4)+1)%2 );
		switch(s.data[i])
		{
			case VOID:

				attron(COLOR_PAIR(1));
				addch(' ');
				attroff(COLOR_PAIR(1));
				break;

			case WHITE:	
				attron(COLOR_PAIR(gui.flipcolor));
				addch('=');
				attroff(COLOR_PAIR(gui.flipcolor));
				break;
			case WQUEEN:	
				attron(COLOR_PAIR(gui.flipcolor));
				addch('#');
				attroff(COLOR_PAIR(gui.flipcolor));
				break;
			case BLACK:	
				attron(COLOR_PAIR(not gui.flipcolor));
				addch('=');
				attroff(COLOR_PAIR(not gui.flipcolor));
				break;
			case BQUEEN:	
				attron(COLOR_PAIR(not gui.flipcolor));
				addch('#');
				attroff(COLOR_PAIR(not gui.flipcolor));
				break;
			default:
				addch('?');

		};

	}

	if (s.draw)
	{
		move(starty+8,startx-2);
		addstr("DRAW PROPOSED");
	};

	if(s.turn<100)
	{
		move(starty-2,startx+3);
		char buf[3];
		sprintf(buf,"%2d",(int)s.turn);
		addstr(buf);
	}
	else
	{
		move(starty-2,startx);
		addstr("TIMEOUT");
	}

}

//GENOMI

//0104190A - default, manuale
//0E07D719 - lucky fellow
//04021799 - little beast


void GUI::runGUI()
{
	//	unpack_genome(0x0E07D719); //lucky fellow!
	uint32_t gen_default = pack_genome(PVAL,QUEENVAL,FROWVAL,CENTERVAL);

	whitegenome = gen_default;
	blackgenome = gen_default;

	initscr();

	start_color();			
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_GREEN, COLOR_YELLOW);
	init_pair(4, COLOR_GREEN, COLOR_RED);

	cbreak();
	noecho();
	keypad(stdscr,true);

	drawback();

	State s;
	s.setup();
	vector<State> history;

	depth = 5;

	bool exitall = true;

	while(exitall)
	{
		//		drawback();
		display(s);
		refresh();
		char c = getch();
		strategy os;
		Move flos;
		vector<Move> ls,lsf;
		bool chosen;
		int choice;
		char curpos=0;
		bool drawswitch = false;
		int chocho;
		bool weredun;

		State h;
		if(true)//(history.size()==0) or (history.back() != s))
		{
			h.copyfrom(&s);
			history.push_back(h);
		}

		switch(c)
		{
			case 'q':
				exitall=false;
				break;
			case 'r':
				s.setup();
				hata.clear();
				hheur.clear();
				drawback();display(s);
				mes.message("Board reset.");
				break;
			case 'g':
				move(4,40);
				char instr[8];
				echo();
				getstr(instr);
				noecho();
				whitegenome = (uint32_t)strtol(instr, NULL, 16);

				echo();
				move(5,40);
				getstr(instr);
				noecho();

				blackgenome = (uint32_t)strtol(instr, NULL, 16);

				drawback();
				refresh();
				break;

			case 'h':
				if(history.size()==0)
				{
					mes.message("History empty.");
					break;
				}
				chocho=history.size()-1;
				weredun=true;
				while (weredun)
				{
					drawback();	
					for(int i=chocho; i>=max(0,chocho-8); i--)
					{
						if(i == chocho)
							attron(COLOR_PAIR(3));
						move(1+(chocho-i),25);
						addstr(NToS(i).c_str());
						if(i == chocho)
							attroff(COLOR_PAIR(3));
					}
					display(history[chocho]);
					refresh();
					int gh = getch();
					switch(gh)
					{
						case KEY_UP:
							chocho = min(chocho+1,(int)(history.size()-1));
							break;
						case KEY_DOWN:
							chocho = max(0,chocho-1);
							break;
						case KEY_ENTER:
						case ' ':
						case '\n':
							weredun=false;
							break;
					}
				}			
				s.copyfrom(&history[chocho]);
				drawback();display(s);refresh();
				break;
			case 'c':
				hata.clear();
				hheur.clear();
				mes.message("Transposition table and refutation history cleared.");
				break;
			case 't':
				drawback();
				refresh();
				unpack_genome(whitegenome);
				os = compute(&s,0,depth, M_ROOT | M_GRAPH);
				s.apply_move(os.optimal);
				mes.message(moverep(os.optimal),flipcolor);
				refresh();
				break;
			case 'f':
				flipcolor = not flipcolor;
				break;
			case 'k':
				while (true)
				{
					//message(NToS(s.movestack.size()));
					drawback();
					unpack_genome(whitegenome);
					os = compute(&s,0,depth,M_ROOT | M_GRAPH);
					s.apply_move(os.optimal);
					message(moverep(os.optimal));
					if (s.turn>=100)
						message("TIMEOUT");

					if((s.turn>=100) or (os.optimal[0] == SURRENDER) or (os.optimal[0] == ACCEPT_DRAW))
						break;

					display(s);refresh();

					drawback();s.flip();
					unpack_genome(blackgenome);
					os = compute(&s,0,depth,M_ROOT | M_GRAPH | M_GRAPHCOLOR);
					s.apply_move(os.optimal);
					mes.message(moverep(os.optimal),1);
					s.flip();

					if (s.turn>=100)
						message("TIMEOUT");


					if((s.turn>=100) or (os.optimal[0] == SURRENDER) or (os.optimal[0] == ACCEPT_DRAW))
						break;

					display(s);refresh();


				}
				display(s);refresh();
				break;
			case 's':
				depth = max(2,depth-1);
				message("Depth lowered to "+NToS(depth));
				break;
			case 'd':
				depth = min(MAXGUIDEPTH, depth+1);
				message("Depth raised to "+NToS(depth));
				break;
			case 'y':
				refresh();
				s.flip();
				unpack_genome(blackgenome);
				os = compute(&s,0,depth,M_ROOT | M_GRAPH | M_GRAPHCOLOR);
				s.apply_move(os.optimal);
				flos = flipmove(os.optimal);
				mes.message(moverep(flos),not flipcolor);
				s.flip();
				refresh();
				break;
			case 'e':
				message("Input correction");
				refresh();
				chosen = false;
				for (int i=0; i<5; i++)
				{
					move(3+i,20);
					addstr(NToS(i).c_str());
				}
				while(not chosen)
				{
					display(s);
					moveton(curpos);
					attron(COLOR_PAIR(3));
					addch('E');
					attroff(COLOR_PAIR(3));
					int gh = getch();
					switch(gh)
					{
						case KEY_LEFT:
							curpos = (curpos + 1)%32;
							break;
						case KEY_RIGHT:
							curpos = (curpos - 1 + 32)%32;
							break;
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
							s.data[curpos] = (Cell)(gh-'0');
							chosen = true;
							break;
						default:
							chosen = true;
					}

				}	
				message("Modified.");
				break;

			case 'm':
				s.flip();
				ls = s.raw_movelist();
				s.flip();
				lsf = vector<Move>();
				for (int i=0; i<ls.size(); i++)
					lsf.push_back(flipmove(ls[i]));
				chosen = false;
				choice = 0;
				//	message("Choose move.");
				while (not chosen)
				{
					display(s);
					moveton(lsf[choice].back());
					attron(COLOR_PAIR(3));
					addch('X');
					attroff(COLOR_PAIR(3));
					moveton(lsf[choice].front());
					attron(COLOR_PAIR(4));
					addch('.');
					attroff(COLOR_PAIR(4));

					for (int i=0; i<ls.size(); i++)
					{
						move(1+i,20);
						if (choice == i)
							attron(COLOR_PAIR(2));
						addstr(moverep(lsf[i]).c_str());
						if (choice == i)
							attroff(COLOR_PAIR(2));
					}
					move(1+ls.size(),20);
					attron(COLOR_PAIR(1));
					if (drawswitch)
						addstr("[+]");
					else
						addstr("[ ]");
					addstr(" PR.DRAW (p)");
					attroff(COLOR_PAIR(1));
					int gh = getch();

					switch(gh)
					{
						case 'q':
						case KEY_BACKSPACE:
							chosen = true;
							choice = -1;
							break;
						case KEY_ENTER:
						case ' ':
						case '\n':
							chosen = true;
							break;
						case KEY_UP:
							choice = (choice+ls.size()-1)%ls.size();
							break;
						case KEY_DOWN:
							choice = (choice+1)%ls.size();
							break;
						case 'p':
							drawswitch = not drawswitch;
					}


				}
				if (choice != -1)
				{	
					s.flip();
					if (drawswitch)
						ls[choice].push_back(PROPOSE_DRAW);

					s.apply_move(ls[choice]);
					s.flip();
					mes.message(moverep(flipmove(ls[choice])),not flipcolor);
				}
				for (int i=0; i<15; i++){
					move(3+i,20);
					addstr("               ");};

				drawback();display(s);refresh();

				break;

			default:
				message("Invalid command.");	
		}

	}

	endwin();
}

void Database::generate_openings(int dd)
{
	cout << "Generating opening database... (depth " << dd << ")" << endl;
	State s;
	s.setup(); s.flip();

	vector<Move> ls = s.raw_movelist();

	for(int i=0; i<ls.size(); i++)
	{
		cout << "Move " << i << " of " << ls.size() << endl;
		s.setup();
		s.flip();

		s.apply_move(ls[i]);

		s.flip();

		strategy outo = compute(&s,0,dd,M_ROOT);	

		insert(s,dd,outo,true);

	}

	push();

}

//--EVOLUTION--

const int POPSIZE = 15;
const int MATCHES = 50;
const int EVDEPTH = 7;

struct Organism {
	uint32_t genome;
	int points;

};

uint32_t mutate(uint32_t genome)
{

	return (genome xor (0x00000001 << (rand()%32)));
};

bool sortorgs (Organism i,Organism j) { return (i.points<j.points); }


void print_pop(vector<Organism> *popp)
{
	for (int i=0; i<(*popp).size(); i++)
		cout << hex << ((*popp)[i].genome) << dec << " - " << (*popp)[i].points << endl;
}

uint32_t son(uint32_t f, uint32_t m)
{

	int sec = rand()%100;

	if(sec<10)
		return f;
	if(sec<20)
		return m;

	if(sec<80)
	{
		//crossover
		int expn = rand()%32;
		uint32_t mask = (0xFFFFFFFF <<expn);

		uint32_t out = (f & mask) | (m & (~mask))  ;
		return out;
	}
	//mutation
	bool r = rand()%2;
	return mutate(r*f + (1-r)*m);
}

int probs[16] = {
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	3,
	3,
	4
};

void evolution()
{
	//	unpack_genome(0xabcdef00);
	//	cout << (int)QUEENVAL << endl;
	//	exit(0);
	//	uint32_t mask = (0xFFFFFFFF <<15);

	//	uint32_t out = (0x11111111 & mask) | (0x222222 & (~mask))  ;

	//	cout << hex << out << dec << endl; 	exit(0);


	uint32_t origen = pack_genome(PVAL,QUEENVAL,FROWVAL,CENTERVAL);
	vector<Organism> population;

	cout << "Generating initial population" << endl;

	srand (static_cast <unsigned> (time(NULL)));	
	for(int i=0; i<POPSIZE; i++)
	{

		Organism o;
		o.genome = rand32();
		if(true)
			o.genome = mutate(o.genome);
		o.points = 0;
		population.push_back(o);
	}


	print_pop(&population);
	int gennum = 0;
	while(true)
	{
		cout << endl;
		cout << "GENERATION #" << gennum << endl;

		print_pop(&population);
		cout << endl;

		for(int j=0; j<MATCHES; j++)
		{
			cout << "Match " << j << endl;
			int white = rand()%population.size();
			int black = rand()%population.size();
			if (black == white)
				black = (black+1)%population.size();
			cout << white << " against " << black << endl;

			State s;
			s.setup();
			hata.clear();

			int win = 0;

			for(int trun = 0; trun <= 50; trun ++)
			{
				cout << "#" << flush;

				unpack_genome(population[white].genome);	
				strategy os = compute(&s,0,EVDEPTH,M_ROOT);

				if(os.optimal[0] == SURRENDER)
				{
					win = -1;break;
				}

				if(os.optimal[0] == ACCEPT_DRAW)
				{
					win = 0;break;
				}

				s.apply_move(os.optimal);

				s.flip();

				unpack_genome(population[black].genome);
				os = compute(&s,0,EVDEPTH,M_ROOT);

				if(os.optimal[0] == SURRENDER)
				{
					win = 1;break;
				}

				if(os.optimal[0] == ACCEPT_DRAW)
				{
					win = 0;break;
				}

				s.apply_move(os.optimal);

				s.flip();

				if (s.turn>=100)
				{
					win=0;break;
				}

				//if((s.turn>=100) or (os.optimal[0] == SURRENDER) or (os.optimal[0] == ACCEPT_DRAW))

			}
			cout << endl;

			cout << "Win status: " << win << endl;

			population[white].points += win;
			population[black].points -= win;

			cout << endl;

			//			print_pop(&population);

		}


		sort(population.begin(),population.end(),sortorgs);
		print_pop(&population);
		uint32_t father,mother;
		vector<Organism> newpop;
		for (int i=0; i<population.size(); i++)
		{	
			int ps = population.size();
			father = population[ps - 1 - pow(rand()%ps,2)/ps ].genome; //population[population.size()-1-(rand()%3)].genome;
			mother = population[ps - 1 - pow(rand()%ps,2)/ps ].genome;//population[population.size()-2-(rand()%3)].genome;
			Organism newborn;
			newborn.genome = son(father,mother);
			newborn.points = 0;
			newpop.push_back(newborn);
		}
		population = newpop;

		gennum++;
	}
}



void TEST_database()
{
	cout << database.db.size() << endl;
	cout << database.db.begin()->first.data() << endl;
	database.push();
	database.pull();
	cout << database.db.size() << endl;

	cout << database.db.begin()->first.data() << endl;

	State s;
	s.setup();
	strategy ls = compute(&s,0,5,M_ROOT);
	s.apply_move(ls.optimal);
	s.drawascii();	
}

void TEST_hashtable()
{
	hata.clear();
	State s;
	s.setup();
	cout << hata.hash(&s) << endl;


	while(true)
	{
		s.random();
		s.drawascii();
	
		cout << "value: " << s.value_function() << endl;

	};

}

void TEST_profiling()

{
	State s;
	s.setup();
	cout << "Scramblin'" << endl;
	for(int i=0;i<3;i++)
		s.apply_move(s.raw_movelist()[0]);
	cout << "Computin'" << endl;
	strategy ss = compute(&s,0,5,M_ROOT);
	cout << "Done." << endl;


}

int main(int ac, char* av[])
{
//	TEST_hashtable();exit(0);

	//	setlocale(LC_CTYPE,"C-UTF-8");
	parse_options(ac,av);

	//	TEST_hashtable(); exit(0);


	if(vm.count("openings"))
	{
		database.generate_openings(vm["depth"].as<int>() );
		exit(0);
	}

	if(vm.count("ptest"))
	{
		TEST_profiling();
		exit(0);
	}

	if(vm.count("evolution"))
	{
		evolution();
		exit(0);
	}

	cout << "Loading database into memory..." << endl;	
	database.pull();
	gui.runGUI();
	exit(0);
}
