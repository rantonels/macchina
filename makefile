.PHONY: all clean

CC := g++
CFLAGS := -std=c++11 -O3
SRCDIR := src
BUILDDIR := build
TARGET := bin/drafts
INC := -I include

LIB := -lncurses -lboost_program_options

all: drafts


$(BUILDDIR)/boardrep.o : $(SRCDIR)/boardrep.cpp $(SRCDIR)/boardrep.h
	g++ $(CFLAGS) -c $(SRCDIR)/boardrep.cpp -o $(BUILDDIR)/boardrep.o $(INC) $(LIB)

$(BUILDDIR)/drafts.o : $(SRCDIR)/drafts.cpp $(SRCDIR)/boardrep.h
	g++ $(CFLAGS) -c $(SRCDIR)/drafts.cpp -o $(BUILDDIR)/drafts.o $(INC) $(LIB)

$(TARGET): $(BUILDDIR)/drafts.o $(BUILDDIR)/boardrep.o
	$(CC) $(CFLAGS) $(BUILDDIR)/drafts.o $(BUILDDIR)/boardrep.o -o $(TARGET) $(INC) $(LIB)

drafts : $(TARGET)
	cp $(TARGET) drafts

#profile: $(SRCDIR)/drafts.cpp
#	$(CC) -pg -std=c++11 -O3 $(SRCDIR)/drafts.cpp -o $(TARGET)_profiling $(INC) $(LIB)

clean:
	rm -f drafts bin/* $(BUILDDIR)/*
