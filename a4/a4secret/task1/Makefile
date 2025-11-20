include ../config.mk

EXES = guess1 secret1a secret1b

.PHONY: all clean install
all: $(EXES)

$(EXES) : Makefile

secret1a: secret1.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"987\" $< -o $@

secret1b: secret1.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"zo0\" $< -o $@

install: secret1a secret1b
	install -m 111 secret1a $(INSTALLDIR)
	install -m 111 secret1b $(INSTALLDIR)

guess1: guess1.cpp driver1.cpp
	g++ -O2 -Wall guess1.cpp driver1.cpp -o guess1

clean: Makefile
	/bin/rm -f *~
	/bin/rm -f $(EXES)
 