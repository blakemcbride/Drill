
#  Makefile for Linux & Mac

drill : drill.c getch.c
	cc -o drill drill.c getch.c

clean:
	rm -f *.o *~ *.obj drill.exe

realclean: clean
	rm -f drill

