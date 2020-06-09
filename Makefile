CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -pthread -ggdb

BIN		:= bin
SRC		:= ./
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:= -lncurses -lasound
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run : 
	clear
	./$(BIN)/$(EXECUTABLE)


build: clean all
$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

buildRun: build run

clean:
	-rm $(BIN)/*

