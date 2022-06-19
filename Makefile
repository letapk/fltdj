#Makefile for fltdj, The Daily Journal
#Copyright, (c) 2001 onwards, Kartik Patel, letapk@gmail.com
#Last modified 27 July, 2003

RM = rm -f
CC = g++

SHELL = /bin/sh

CFLAGS = -g
LDFLAGS=
DEFINES = 
INCLUDES = -I. -I/usr/X11R6/include -I/usr/local/include/ 
#LIBS = -L./. -L/usr/lib -L/usr/X11R6/lib -lfltk_images -lpng -lfltk_jpeg -lfltk -lXext -lX11 -lm
LIBS = -L/usr/X11R6/lib -lfltk_images -lfltk -lpng -lfltk_jpeg -lXext -lX11 -lm
OBJECTFILES = 	fltdj.o \
		fltdjmain.o \
		fltdjnotes.o\
		fltdjfile.o \
		fltdjappt.o \
		fltdjcontacts.o \
		fltdjholidays.o \
		fltdjprefs.o \
  		fltdjsearch.o \
	     fltdjglobalsearch.o \
    		fltdjcal.o \
		cmultitabs.o

# Rule to create .o files from .cpp files
.cpp.o:
	$(RM) $@
	$(CC) -c $(CFLAGS) $(INCLUDES) $*.cpp

all:: fltdj
		
#Dependencies
fltdjmain.o: fltdjmain.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjmain.cpp

fltdjnotes.o: fltdjnotes.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjnotes.cpp

fltdjfile.o: fltdjfile.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjfile.cpp
	
fltdjappt.o: fltdjappt.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjappt.cpp
	
fltdjcontacts.o: fltdjcontacts.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjcontacts.cpp
	
fltdjholidays.o: fltdjholidays.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjholidays.cpp
	
fltdjprefs.o: fltdjprefs.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjprefs.cpp

fltdjsearch.o: fltdjsearch.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjsearch.cpp

fltdjglobalsearch.o: fltdjglobalsearch.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjglobalsearch.cpp

fltdjcal.o: fltdjcal.cpp fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdjcal.cpp

fltdj.o: fltdj.cpp fltdj.h fltdjfunctions.h
	$(CC) -c $(CFLAGS) $(INCLUDES) fltdj.cpp

cmultitabs.o: cmultitabs.cxx cmultitabs.h
	$(CC) -c $(CFLAGS) $(INCLUDES) cmultitabs.cxx

#Executable
fltdj:  $(OBJECTFILES)
	$(RM) $@
	#Normal link sequence. According to user feedback, it may not work
	#under FreeBSD. If that is the case, comment out the next line:
	$(CC) -o $@ $(CFLAGS) $(OBJECTFILES) $(LIBS)

	#and uncomment the line below. Suggested by Jean-Phillipe for FreeBSD.
	#$(CC) -o $@ $(CFLAGS) $(LIBS) $(OBJECTFILES)
	
	@if test -d ~/.fltdj; then \
		echo "====Directory .fltdj exists"; \
	else \
		echo "====Creating directory .fltdj"; mkdir $(HOME)/.fltdj; \
	fi;
	cp doc/* $(HOME)/.fltdj/.
	cp COPYING $(HOME)/.fltdj/COPYING

static:  $(OBJECTFILES)
	$(RM) fltdj
	$(CC) -static -o fltdj $(CFLAGS) $(OBJECTFILES) $(LIBS) -lpthread -lz

	@if test -d ~/.fltdj; then \
		echo "====Directory .fltdj exists"; \
	else \
		echo "====Creating directory .fltdj"; mkdir $(HOME)/.fltdj; \
	fi;
	cp doc/* $(HOME)/.fltdj/.
	cp COPYING $(HOME)/.fltdj/COPYING
     
clean:
	rm -f *.o core fltdj

install:
	make all
	strip fltdj
	cp fltdj /usr/local/bin/fltdj
