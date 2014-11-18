#include "boardrep.h"
#include "tree.hh"


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
