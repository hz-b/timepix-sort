CXX=g++
CXXFLAGS=-pedantic -Wall -I../include -std=c++17 -g -ggdb -O5 -ffast-math
CXXFLAGS+=-fopenmp #-D_GLIBCXX_PARALLEL

MMAP_ALLOC_DIR=/home/mfp/Devel/github/mmap_allocator
CXXFLAGS+=-I$(MMAP_ALLOC_DIR)

LD=$(CXX)
LDFLAGS=-L$(MMAP_ALLOC_DIR) -L/usr/lib/gcc/x86_64-linux-gnu/11/ -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib/x86_64-linux-gnu/
LDLIBS=-lmmap_allocator -lgomp
OBJS=read.o process.o main.o

ALL: main

main : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean :
	$(RM) $(RMFLAGS) $(OBJS)
