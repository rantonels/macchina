all:
	g++ -std=c++11 -O3 -lncurses -lboost_program_options drafts.cpp -o drafts
clean:
	rm drafts

