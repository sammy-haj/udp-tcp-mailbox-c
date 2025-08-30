default : tsock

all : tsock

clean :
	rm *.o tsock

tsock : tsock.o \
Fonctions.o Liste_chaine.o
	gcc -Wall tsock.o Fonctions.o \
Liste_chaine.o -o $@

%.o : %.c
	gcc -Wall -c $<
Fonctions.o : Fonctions.h
Liste_chaine.o : Liste_chaine.h