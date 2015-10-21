CC=g++
LIBP=-L/usr/lib64/
LIBS=-lGL -lGLU -lglut -lm -lX11

INCP=-I/usr/include/GL/

background:
	-$(CC) $@.c $(INCP) $(LIBP) $(LIBS) -o $@ 

#Remove files

clean: 
	-clear
	-/bin/rm mssgs5 d c *.o core 
	-clear
