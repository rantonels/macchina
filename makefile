.PHONY: all clean

CC := g++
CFLAGS := -std=c++11 -O3
SRC := src
BUILDDIR := build
TARGET := bin/drafts
INC := -I include

LIB := -lncurses -lboost_program_options

all: drafts


$(BUILDDIR)/boardrep.o : $(SRC)/boardrep.cpp $(SRC)/boardrep.h
	g++ $(CFLAGS) -c $(SRC)/boardrep.cpp -o $(BUILDDIR)/boardrep.o $(INC) $(LIB)

$(BUILDDIR)/hheuristic.o : $(SRC)/hheuristic.cpp $(SRC)/hheuristic.h $(SRC)/boardrep.h
	g++ $(CFLAGS) -c $(SRC)/hheuristic.cpp -o $(BUILDDIR)/hheuristic.o $(INC) $(LIB)

$(BUILDDIR)/database.o : $(SRC)/database.cpp $(SRC)/database.h $(SRC)/boardrep.h
	g++ $(CFLAGS) -c $(SRC)/database.cpp -o $(BUILDDIR)/database.o $(INC) $(LIB)

$(BUILDDIR)/drafts.o : $(SRC)/drafts.cpp $(SRC)/boardrep.h $(SRC)/hheuristic.h $(SRC)/database.h
	g++ $(CFLAGS) -c $(SRC)/drafts.cpp -o $(BUILDDIR)/drafts.o $(INC) $(LIB)

$(TARGET): $(BUILDDIR)/drafts.o $(BUILDDIR)/boardrep.o $(BUILDDIR)/hheuristic.o $(BUILDDIR)/database.o
	$(CC) $(CFLAGS) $(BUILDDIR)/drafts.o $(BUILDDIR)/boardrep.o $(BUILDDIR)/hheuristic.o $(BUILDDIR)/database.o -o $(TARGET) $(INC) $(LIB)

drafts : $(TARGET)
	cp $(TARGET) drafts

#profile: $(SRC)/drafts.cpp
#	$(CC) -pg -std=c++11 -O3 $(SRC)/drafts.cpp -o $(TARGET)_profiling $(INC) $(LIB)

clean:
	rm -f drafts bin/* $(BUILDDIR)/*
