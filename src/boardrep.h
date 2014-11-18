#ifndef __BOARDREP_H__
#define __BOARDREP_H__

#include <vector>
#include <string>
#include <climits>
#include <iostream>
#include <sstream>

using namespace std;

//PARAMETRI GLOBALI

const int MAXSTACKSIZE = 30; //dimensione massima stack di mosse di sicurezza per evitare ripetizioni



//utilita' (spostare poi!)
template <typename T>
string NToS ( T Number )
{
	stringstream ss;
	ss << Number;
	return ss.str();
}

char NToC (int n);




//def tipo di cella
typedef char Cell;

//def tipo di mossa
typedef vector<char> Move;

//def struttura di strategia
struct strategy {
	float value;
	Move optimal;

};



//costanti

const char VOID=0;
const char WHITE=1;
const char BLACK=2;
const char WQUEEN=3;
const char BQUEEN=4;

//white mask : (n || 0x01)
//black mask : (n-1) || 0x01



//codici mossa speciali

const char SURRENDER = 32;
const char NO_MOVES = 33;
const char PROPOSE_DRAW = 34;
const char ACCEPT_DRAW = 35;
const char NOT_DATABASE = 36;
const char ENDMOVE_CARRIAGE = 37;


//infinito

const int INFTY = INT_MAX;
const int INFTIME = INT_MAX - 1;


string cellrep(char n);

string moverep(Move m);

void printmove(Move m);

char parsepos(string tb);

Move parsemove(string ss);

Move flipmove(Move m);

char cellascii(Cell c);

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















#endif
