PROG = shooter
CC = gcc
CXX = g++

include common.mk

CXXFLAGS += `sdl2-config --cflags`
CXXFLAGS += -Wall -g -lefence -DDEBUG

LDFLAGS += `sdl2-config --libs` -lsqlite3 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lm -lsqlite3

# linking the program.
$(PROG): $(OBJS)
	@echo $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)
