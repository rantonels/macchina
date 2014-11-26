using namespace std;

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <boost/timer.hpp>


using boost::timer;

//#include "tree.hh"
#include <limits>
#include <time.h>
#include <ncurses.h> 
//#include <locale.h>
#include "boost/program_options.hpp" 
#include <stack>

//#include <array>
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

#include "utils.h"


#include "boardrep.h"

#include "hheuristic.h"

#include "database.h"

#include "engine.h"

#include "graphics.h"
 

string elapsedrep(double letime)
{
	char buffer [10];
	if(letime >= 0.5)
	{
		sprintf(buffer, " (%.1f s)", letime);
	}
	else
	{
		sprintf(buffer, " (%3.0f ms)", letime*1000);
	}
	return string(buffer);
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

//MESSAGES

const string currentDateTime() {		// da http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

#include <fstream>

struct Message{
	string str;
	int col;
};

const char * MESLOGFNAME = "log";

class message_holder{
		deque<Message> queue;
		ofstream meslogfile;
	public:
		message_holder();
		~message_holder();
		void message(string m);
		void message(string m, int col);
	private:
		void drawm();
} mes;

message_holder::message_holder()
{
	meslogfile.open(MESLOGFNAME, ios::out | ios::app); 
	meslogfile << endl;
	meslogfile << "-----------------" << endl;
	meslogfile << "* New session" << endl;
	meslogfile << currentDateTime() << endl;
}

message_holder::~message_holder()
{
	meslogfile.close();
}

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
	
	meslogfile << "[" << col << "]\t" << m << endl;
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


void display(State * s, bool flip = false)
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
		switch(s->data[i])
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

	if (s->draw)
	{
		move(starty+8,startx-2);
		addstr("DRAW PROPOSED");
	};

	if(s->turn<100)
	{
		move(starty-2,startx+3);
		char buf[3];
		sprintf(buf,"%2d",(int)s->turn);
		addstr(buf);
	}
	else
	{
		move(starty-2,startx);
		addstr("TIMEOUT");
	}

	graphBoard(s,flip);
	graphRefresh();
}

//GENOMI

//0104190A - default, manuale
//0E07D719 - lucky fellow
//04021799 - little beast


void GUI::runGUI()
{
	timer tm;
	//	unpack_genome(0x0E07D719); //lucky fellow!
	uint32_t gen_default = pack_genome(PVAL,QUEENVAL,FROWVAL,CENTERVAL);

	whitegenome = gen_default;
	blackgenome = gen_default;

	//init ncurses
	initscr();

	start_color();			
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_GREEN, COLOR_YELLOW);
	init_pair(4, COLOR_GREEN, COLOR_RED);

	cbreak();
	noecho();
	keypad(stdscr,true);

	//init graphical board
	startupWindow();


	
	drawback();

	State s;
	s.setup();
	vector<State> history;

	depth = 5;

	bool exitall = true;

	while(exitall)
	{
		//		drawback();
		display(&s,flipcolor);
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
				message("#################");
				message("BOARD RESET");
				s.setup();
				hata.clear();
				hheur.clear();
				drawback();display(&s,flipcolor);
				//mes.message("Board reset.");
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
					display(&history[chocho],flipcolor);
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
				drawback();display(&s,flipcolor);refresh();
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
				tm.restart();
				os = compute(&s,0,depth, M_ROOT | M_GRAPH);
				s.apply_move(os.optimal);
				mes.message(moverep(os.optimal) + elapsedrep(tm.elapsed()),flipcolor);
				refresh();
				break;
			case 'f':
				flipcolor = not flipcolor;
				break;
			case 'k':
				message("#################");
				message("NEW CPU-CPU MATCH");
				message(genrep(whitegenome)+" vs "+genrep(blackgenome));
				message("depth "+to_string(depth));
				message("");
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

					display(&s,flipcolor);refresh();

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

					display(&s,flipcolor);refresh();


				}
				display(&s,flipcolor);refresh();
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
				tm.restart();
				mes.message(moverep(flos) + elapsedrep(tm.elapsed()),not flipcolor);
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
					display(&s,flipcolor);
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
					display(&s,flipcolor);
					graphPointers(lsf[choice]);
					graphRefresh();
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

				drawback();display(&s,flipcolor);refresh();

				break;

			default:
				0;//message("Invalid command.");	
		}

	}

	endwin();
	graphCleanup();
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

void TEST_graphics()
{

	cout << "Beginning graphics test" << endl;

	startupWindow();
	
	
	State s;
	s.setup();
	
	graphBoard(&s);
	graphRefresh();

	sleep(3);
	
	graphCleanup();
	
}

int main(int ac, char* av[])
{
//	TEST_graphics(); exit(0);
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
