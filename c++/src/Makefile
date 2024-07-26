CXX=g++
CXXFLAGS=-pedantic -Wall -I../include -std=c++17 -g -ggdb -O0

MMAP_ALLOC_DIR=/home/mfp/Devel/github/mmap_allocator
CXXFLAGS+=-I$(MMAP_ALLOC_DIR)

LD=$(CXX)
LDFLAGS=-L$(MMAP_ALLOC_DIR)
LDLIBS=-lmmap_allocator
OBJS=read.o process.o main.o

ALL: main

main : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean :
	$(RM) $(RMFLAGS) $(OBJS)
