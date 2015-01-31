.PHONY: all clean

CC := g++
CFLAGS := -std=c++11 -O3
SRC := src
BUILDDIR := build
TARGET := bin/drafts
INC := -I include 
#-I /usr/include

SDLLIB := -lSDL2 -lSDL2_image -lpng12 
LIB := -pthread -lncurses -lboost_program_options $(SDLLIB)

OLIST := $(BUILDDIR)/boardrep.o $(BUILDDIR)/hheuristic.o $(BUILDDIR)/database.o $(BUILDDIR)/ttable.o $(BUILDDIR)/utils.o $(BUILDDIR)/engine.o $(BUILDDIR)/graphics.o

all: drafts

#utilities

$(BUILDDIR)/utils.o : $(SRC)/utils.cpp $(SRC)/utils.h
	g++ $(CFLAGS) -c $(SRC)/utils.cpp -o $(BUILDDIR)/utils.o $(INC) $(LIB)

#board representation

$(BUILDDIR)/boardrep.o : $(SRC)/boardrep.cpp $(SRC)/boardrep.h
	g++ $(CFLAGS) -c $(SRC)/boardrep.cpp -o $(BUILDDIR)/boardrep.o $(INC) $(LIB)

#engine utilities/data structures

$(BUILDDIR)/hheuristic.o : $(SRC)/hheuristic.cpp $(SRC)/hheuristic.h $(SRC)/boardrep.h
	g++ $(CFLAGS) -c $(SRC)/hheuristic.cpp -o $(BUILDDIR)/hheuristic.o $(INC) $(LIB)

$(BUILDDIR)/database.o : $(SRC)/database.cpp $(SRC)/database.h $(SRC)/boardrep.h
	g++ $(CFLAGS) -c $(SRC)/database.cpp -o $(BUILDDIR)/database.o $(INC) $(LIB)

$(BUILDDIR)/ttable.o : $(SRC)/ttable.cpp $(SRC)/ttable.h $(SRC)/boardrep.h $(SRC)/utils.h
	g++ $(CFLAGS) -c $(SRC)/ttable.cpp -o $(BUILDDIR)/ttable.o $(INC) $(LIB)

#graphics and gui

$(BUILDDIR)/graphics.o : $(SRC)/graphics.cpp $(SRC)/boardrep.h
	g++ $(CFLAGS) -c $(SRC)/graphics.cpp -o $(BUILDDIR)/graphics.o $(INC) $(LIB)

#engine

$(BUILDDIR)/engine.o : $(SRC)/engine.cpp $(SRC)/engine.h $(SRC)/boardrep.h $(SRC)/hheuristic.h $(SRC)/database.h
	g++ $(CFLAGS) -c $(SRC)/engine.cpp -o $(BUILDDIR)/engine.o $(INC) $(LIB)

#main program

$(BUILDDIR)/drafts.o : $(SRC)/drafts.cpp $(SRC)/boardrep.h $(SRC)/hheuristic.h $(SRC)/database.h $(SRC)/ttable.h $(SRC)/engine.h $(SRC)/graphics.h
	g++ $(CFLAGS) -c $(SRC)/drafts.cpp -o $(BUILDDIR)/drafts.o $(INC) $(LIB)

#linking

$(TARGET): $(BUILDDIR)/drafts.o $(OLIST)
	$(CC) $(CFLAGS) $(BUILDDIR)/drafts.o $(OLIST) -o $(TARGET) $(INC) $(LIB)
	$(CC) -pg $(CFLAGS) $(BUILDDIR)/drafts.o $(OLIST) -o $(TARGET)_profiling $(INC) $(LIB)


drafts : $(TARGET)
	cp $(TARGET) drafts
	cp $(TARGET)_profiling drafts_profiling


#test

$(BUILDDIR)/test.o: $(SRC)/test.cpp $(SRC)/boardrep.h $(SRC)/engine.h
	$(CC) $(CFLAGS) -c $(SRC)/test.cpp -o $(BUILDDIR)/test.o
	
bin/test : $(BUILDDIR)/test.o $(BUILDDIR)/boardrep.o $(BUILDDIR)/utils.o $(BUILDDIR)/engine.o $(BUILDDIR)/hheuristic.o $(BUILDDIR)/database.o $(BUILDDIR)/ttable.o
	$(CC) $(CFLAGS) $(BUILDDIR)/test.o $(BUILDDIR)/boardrep.o $(BUILDDIR)/utils.o $(BUILDDIR)/engine.o $(BUILDDIR)/hheuristic.o $(BUILDDIR)/database.o $(BUILDDIR)/ttable.o -o bin/test $(INC) $(LIB)

test : bin/test
	cp bin/test test

clean:
	rm -f drafts test bin/* $(BUILDDIR)/*
