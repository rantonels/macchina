all:
	g++  -lncurses -lboost_program_options drafts.cpp -o drafts
clean:
	rm drafts

