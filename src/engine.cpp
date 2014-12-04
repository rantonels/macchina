
#include "engine.h"

//--GENETIC CODE




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
		
		//int wsum = 0;
		//int bsum = 0;
		//for (int i=0; i<32; i++)
		//{
			//wsum += (data[i]==WHITE)+2*(data[i]==WQUEEN);
			//bsum += (data[i]==BLACK)+2*(data[i]==BQUEEN);
		//}
		
		int wsum = bboardSum(data.PMASK & (~data.BMASK)) + bboardSum(data.PMASK & data.KMASK & (~data.BMASK));
		int bsum = bboardSum(data.PMASK & (data.BMASK)) + bboardSum(data.PMASK & data.KMASK & (data.BMASK));
		
		if (wsum>bsum)
			return INFTIME;
		else
			if(bsum>wsum)
				return -INFTIME;
			else
				return 0;

	}

	int val = 0;
	//int b=0;
	//int n=0;
	//int tmp;
	//for (int i=0; i<32; i++)
	//{
		//Cell d = data[i];
		////conteggio bianchi
		//tmp = (PIECEVAL+FROWVAL*((i/4)==7))*(d == WHITE) + QUEENVAL*(d == WQUEEN);
		//b += 10*tmp + CENTERVAL*center(i%4)*(tmp/10); //moltiplica per 1.1

		////conteggio neri
		//tmp = (PIECEVAL+FROWVAL*((i/4)==0))*(d == BLACK) + QUEENVAL*(d == BQUEEN);
		//n += 10*tmp + CENTERVAL*center(i%4)*(tmp/10); //moltiplica per 1.1
	//}	

	uint32_t WM = data.PMASK & (~data.BMASK) & (~data.KMASK);	//uomini bianchi non dame
	uint32_t WK = data.PMASK & (~data.BMASK) & (data.KMASK);	//dame bianche
	uint32_t BM = data.PMASK &  (data.BMASK) & (~data.KMASK);	//uomini neri non dame
	uint32_t BK = data.PMASK & (data.BMASK) & (data.KMASK);		//dame nere
	
	int tmp;
	
	tmp = PIECEVAL*bboardSum(WM) + (FROWVAL - PIECEVAL)*bboardSum(WM & 0xF0000000); //maschera per l'ultima riga
	tmp += QUEENVAL * bboardSum(WK);
	int b = tmp;
	
	
	tmp = PIECEVAL*bboardSum(BM) + (FROWVAL - PIECEVAL)*bboardSum(BM & 0x0000000F); //maschera per la prima riga
	tmp += QUEENVAL * bboardSum(BK);
	int n = tmp;
	
	
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







//----MINIMAX----


//la funzione ritorna il punteggio stimato sempre per il giocatore turn!

strategy compute( State *original, bool turn, int depth, unsigned char mode, float alphabetalim)
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
		if(original->turn <=2)
		{
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
		int hix = h%TTBSIZE;
		if((hata.table[hix].active) and (hata.table[hix].depth >= depth) and (hata.table[hix].hash == h) )
		{

			//trovato nella hashtable. Ricopiare semplicemente
			//cout << "found in ht" << endl;
			hata.accesscounter++;
			return hata.table[hix].optimal;

		}
		//cout << "fine ricerca in ht..." << endl;

	}


	//generazione movestack
	vector<Move> ls = original->raw_movelist();

	//cout << "moveset generated" << endl;

	//se non ci sono mosse
	if ((ls.size() == 0) or (ls.back()[0] == NO_MOVES)) //movearray vuoto vuol dire o perdita o stallo
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

	//se c'e' una sola mossa
	if ( ((mode & M_ROOT) == M_ROOT) and (ls.size() == 1))
	{
		strategy s;
		s.value = 0;
		s.optimal = ls[0];
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
			move(1+i,17);
			addstr("                      ");};

		for (int i=0; i<ls.size(); i++)
		{
			move(1+i,17);
			int hval = hheur.value[ls[i].front()][ls[i].back()];
			if (hval>0)
			{
				char bufff[2];
				sprintf(bufff,"%2d",log2(hval));
				addstr(bufff);
			}
			move(1+i,27);
			addstr(moverep(ls[i]).c_str());
		};
		attroff(COLOR_PAIR(col));
		refresh();
	};

	State tmp; //l'allocazione dello stato temporaneo e' spostata fuori dal loop. 1 ordine di allocazioni in meno!
	
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
				hheur.value[ls[i].front()][ls[i].back()] +=  (0x00008000 >> (depth));
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
	int hix = h%TTBSIZE;
	if( (not(hata.table[hix].active)) or (hata.table[hix].depth < depth) )
	{
		hata.table[hix].active = true;
		hata.table[hix].optimal = out;
		hata.table[hix].depth = depth;
		hata.table[hix].hash = h;
	};

	return out;
};

//struct computeargs {
	//State * original;
	//bool turn;
	//int depth;
	//unsigned char mode;
	//float alphabetalim;
	//strategy * out;
//};

//void *compute_wrapped(void *argpoint)
//{
		//computeargs ar = (computeargs)argpoint;
		//(*out) = compute(ar.original, ar.turn, ar.depth, ar.mode, ar.alphabetalim);
//}

//const int BRANCHINGRATIO = 10;

//strategy compute_timelimited(State *original, bool turn, int msecs, int sdepth, unsigned char mode, float alphabetalim)
//{
	//float TSTEP = 10;
	//promise<
	//thread thr;

	//thr = thread(compute, original, turn, sdepth, mode, alphabetalim);

	//for(int i=0; i<msecs/TSTEP; i++)
	//{
		//auto status = future.wait_for
	//}
//}
