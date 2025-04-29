SHELL = /bin/sh

GCC     = gcc
CFLAGS  = -g -mavx512f -O3 -funroll-loops -ffast-math -flto
LDFLAGS = -lpthread -lm

GCC_EXE = workshop.gcc

srcdir = ./src
builddir = ./build

gcc: $(GCC_EXE)
all: $(GCC_EXE)

OBJS = $(builddir)/util.o $(builddir)/thrmodel.o $(builddir)/mul.o $(builddir)/vector.o $(builddir)/main.o 

workshop.gcc: $(srcdir)/mul.c $(srcdir)/mul.h $(srcdir)/vector.c $(srcdir)/vector.h $(srcdir)/util.c $(srcdir)/thrmodel.c $(srcdir)/main.c 
	mkdir -p $(builddir)
	$(GCC) $(CFLAGS) -c $(srcdir)/util.c -o $(builddir)/util.o -D_LINUX
	$(GCC) $(CFLAGS) -c $(srcdir)/thrmodel.c -o $(builddir)/thrmodel.o -D_LINUX
	$(GCC) $(CFLAGS) -c $(srcdir)/mul.c -o $(builddir)/mul.o -D_LINUX
	$(GCC) $(CFLAGS) -c $(srcdir)/vector.c -o $(builddir)/vector.o -D_LINUX
	$(GCC) $(CFLAGS) -c $(srcdir)/main.c -o $(builddir)/main.o -D_LINUX
	$(GCC) $(CFLAGS) -g $(OBJS) -o workshop $(LDFLAGS)

clean:
	@rm -rf $(OBJS) $(GCC_EXE)
