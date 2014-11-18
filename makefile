.PHONY: all profile clean

CC := g++
SRCDIR := src
BUILDDIR := build
TARGET := build/drafts
INC := -I include

LIB := -lncurses -lboost_program_options

all: $(TARGET) profile

$(TARGET): $(SRCDIR)/drafts.cpp
	$(CC) -std=c++11 -O3 $(SRCDIR)/drafts.cpp -o $(TARGET) $(INC) $(LIB)

profile: $(SRCDIR)/drafts.cpp
	$(CC) -pg -std=c++11 -O3 $(SRCDIR)/drafts.cpp -o $(TARGET)_profiling $(INC) $(LIB)

clean:
	rm -f drafts drafts_profiling analysis.txt gmon.out $(BUILDDIR)/*

