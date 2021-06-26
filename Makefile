# Makefile for the TTFTPS program
# For C++
CC=g++
CFLAGS= -Wall -Werror -pedantic-errors -DNDEBUG *.cpp
CXXFLAGS=-std=c++11
CCLINK=$(CC)
OBJS=ttftps.o server.o
RM=rm -f

# Create the  executable
ttftps: $(OBJS)
	$(CCLINK) $(CXXFLAGS) -o ttftps $(OBJS)
	
# Create the object files
ttftps.o: ttftps.cpp ttftps.h server.h
server.o: server.cpp server.h ttftps.h

# Clean old files before new make
.PHONY: clean
clean:
	rm -f ttftps $(OBJS)
	$(RM) $(TARGET) *.o *~ "#"* core.*