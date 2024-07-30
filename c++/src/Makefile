CXX=g++
CXXFLAGS=-pedantic -Wall -I../include  -g -ggdb -O5 -ffast-math
CXXFLAGS+=-fopenmp -D_GLIBCXX_PARALLEL

MMAP_ALLOC_DIR=/home/mfp/Devel/github/mmap_allocator

CXXFLAGS_MAP_ALLOC=$(CXXFLAGS) -std=c++17 -I$(MMAP_ALLOC_DIR)
CXXFLAGS+=-std=c++23


LD=$(CXX)
LDFLAGS=-L$(MMAP_ALLOC_DIR) -L/usr/lib/gcc/x86_64-linux-gnu/11/ -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib/x86_64-linux-gnu/
LDLIBS=-lmmap_allocator -lgomp
OBJS=main.o process.o read.o read_raw.o

ALL: main

main : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

read_raw.o : read_raw.cpp
	$(CXX) $(CXXFLAGS_MAP_ALLOC) -c -o $@ $^

sort_test : sort_test.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean :
	$(RM) $(RMFLAGS) $(OBJS)
