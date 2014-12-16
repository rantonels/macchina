#include "boardrep.h"
#include "tree.hh"
#include <strings.h>


char NToC(int n)
{
	return (char)('0' + n);
}



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

//lookup table per l'8-bit reversal

static const unsigned char BitReverseTable256[] = 
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

//32-bit reversal

uint32_t bboardFlip(uint32_t v)
{
return	(BitReverseTable256[v & 0xff] << 24) | 
		(BitReverseTable256[(v >> 8) & 0xff] << 16) | 
		(BitReverseTable256[(v >> 16) & 0xff] << 8) |
		(BitReverseTable256[(v >> 24) & 0xff]);
}

//move generation

uint32_t ROT1 (uint32_t B)
{
		return B>>4;
}

uint32_t ANTIROT1 (uint32_t B)
{
		return B<<4;
}

uint32_t ROT2 (uint32_t B)
{
	return ((B & 0xE0E0E0E0) >> 5) | ((B & 0x07070707) >> 3);
}

uint32_t ANTIROT2 (uint32_t B)
{
	return ((B & 0xE0E0E0E0) << 3) | ((B & 0x07070707) << 5);
}


uint32_t Bitboard::jumpers()
{
	uint32_t BM = PMASK & BMASK & ~KMASK;
	uint32_t WM = PMASK & ~BMASK & ~KMASK;
	
	
	uint32_t whitemanjumpers = WM &  (
										(	ANTIROT1(BM) | ANTIROT2(BM)		)
									&	(	((~PMASK)<<7) | ((~PMASK)<<9)	)
									);
									
	return whitemanjumpers | (PMASK & ~BMASK & KMASK);
};


uint32_t Bitboard::steppers()
{
	uint32_t WM = PMASK & ~BMASK & ~KMASK;
	
	uint32_t whitemansteppers = WM & (ANTIROT1(~PMASK) | ANTIROT2(~PMASK));
	
	return whitemansteppers | (PMASK & ~BMASK & KMASK);
}

//Hamming weight (somma dei bit)

int bboardSum(uint32_t i)
{
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


unsigned char bboardGetbit(uint32_t bb,int i)
{
	return (bb >> i) & 0x00000001;
}


Cell Bitboard::operator[](int i)
{
	unsigned char ispiece	= bboardGetbit(PMASK,i);
	unsigned char isblack	= bboardGetbit(BMASK & PMASK,i);
	unsigned char isking	= bboardGetbit(KMASK & PMASK,i);
	
	return ispiece*(1 + isblack + 2*isking);
}

void setbit(uint32_t * bp, bool bit, int i)
{
	if (bit)
		(*bp) |=  (0x01 << i);
	else
		(*bp) &= ~(0x01 << i);
}

void Bitboard::setCell(int i,Cell val)
{
	unsigned char ispiece	= (val != 0);
	unsigned char isblack	= ispiece*(not (val%2));
	unsigned char isking	= (val >= 3);
	
	setbit(&PMASK,ispiece,i);
	setbit(&BMASK,isblack,i);
	setbit(&KMASK,isking,i);
}

void State::setCell(int i,Cell val)
{
	data.setCell(i,val);
}

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




void State::random()
{
	for(int i=0; i<32; i++)
		setCell(i, (rand()%5)*(rand()%2)*(rand()%2));
}

void State::copyfrom(State* s)
{
	//for(int i=0;i<32;i++)
	//{
		//data[i] = s->data[i];
		
	//}
	data.PMASK = s->data.PMASK;
	data.BMASK = s->data.BMASK;
	data.KMASK = s->data.KMASK;
	//movestack = s->movestack;
	draw = s->draw;
	turn = s->turn;
}

State::State() {

};

void State::setup() {
	for (int i=0; i<12; i++)
		setCell(i,BLACK);
	for (int i=12; i<20; i++)
		setCell(i,VOID);
	for (int i=20; i<32; i++)
		setCell(i,WHITE);
	draw = false;
	turn = 0;
};

void State::clear() {
	for (int i=0; i<32; i++)
		setCell(i,VOID);
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
	if(m[0] == NO_MOVES)
		return;

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
		//movestack = vector<Move>();

		Move::iterator prev = m.begin();
		for(Move::iterator it = m.begin(); it != m.end()-1; ++it)
		{
			char from = (*it);
			char toc = (*(it+1));
			char jumpd = (from + toc + 1 - (from/4)%2)/2;
			setCell(jumpd,VOID);
		}
		setCell(m.back(),data[m[0]]);
		setCell(m[0],VOID);
		if (m.back()<=3)
			setCell(m.back(), WQUEEN);
	}
	else
	{
		//se e' un pezzo normale, pulisci lo stack
		//if (data[m[0]] == WHITE)
			//movestack = vector<Move>();
		//else
		//{
			////	Cell *k = find(data,data+32,WQUEEN);
			////	if (k != data+32)
			////movestack.push_back(m);
		//}


		setCell(m[1],data[m[0]]);
		setCell(m[0], VOID);
		if (m[1] <= 3)
			setCell(m[1], WQUEEN);
	}

	//if(movestack.size()>MAXSTACKSIZE)
	//{
		//movestack.erase(movestack.begin());
	//}

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

	//char tmp;
	//for (int i=0; i<16; i++)
	//{
		//tmp = data[i];
		//setCell(data[i], flipdame(data[31-i]));
		//setCell(data[31-i],flipdame(tmp));
	//}
	
	data.PMASK = bboardFlip(data.PMASK);
	data.BMASK = ~bboardFlip(data.BMASK);
	data.KMASK = bboardFlip(data.KMASK);

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



	//generazione mask potenziali saltatori
	
	uint32_t JMASK = data.jumpers();
	
	
	if(JMASK != 0x00000000)
	{
		//generazione salti
		for (int i=ffs(JMASK)-1;i<32;i++)
		{
			if (bboardGetbit(JMASK,i)) //if (data[i] & 0x01)  //((data[i] == WHITE) or (data[i] == WQUEEN))
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
	}

	//generazione lista mosse normali
	int counter = 0;
	
	uint32_t MMASK = data.steppers();
	if (MMASK != 0)
	{
	for (int i=ffs(MMASK)-1;i<32;i++)
	{
		if(bboardGetbit(MMASK,i))
		{
			counter = 0;
			
				if(not bboardGetbit(data.KMASK,i))
				{
					for(int k=0;k<2;k++)
						if ((FORW[i][k] != 99) && (data[FORW[i][k]] == VOID))
						{
							Move m = mpair(i,FORW[i][k]);
	
	
							outlist.push_back(m);
							counter ++;
						}
	
	
				}	
				else //if there's a white king
				{
					for (int j=0; j<4; j++)
					{
						if ((KING[i][j] != 99) && (data[KING[i][j]] == VOID))
						{
							Move m = mpair(i,KING[i][j]);
	
	
							//cerca mossa nello stack
							//int reps = count(movestack.begin(), movestack.end(), m);
							//if (reps > 1)
							//{
								//continue;
							//}	
	
							outlist.push_back(m);
							counter++;
						}
					}
				}
			

		}
	}
	}


	if(draw)
	{
		outlist.push_back(Move(1,ACCEPT_DRAW));
	}

	if(outlist.size()==0)
		outlist.push_back(Move(1,NO_MOVES));

	return outlist;
}
