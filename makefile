.PHONY: all profile clean

all: drafts drafts_profiling

drafts: drafts.cpp
	g++ -std=c++11 -O3 -lncurses -lboost_program_options drafts.cpp -o drafts

drafts_profiling: drafts.cpp
	g++ -pg -std=c++11 -O3 -lncurses -lboost_program_options drafts.cpp -o drafts_profiling

clean:
	rm -f drafts drafts_profiling analysis.txt gmon.out

