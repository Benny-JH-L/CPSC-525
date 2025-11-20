include ../config.mk

EXES = guess4 secret4a secret4b

.PHONY: all clean install
all: $(EXES)

$(EXES) : Makefile

secret4a: secret4.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"easy123\" -DDELAY=0.5 $< -o $@

secret4b: secret4.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"deadbeefc0decak3\" -DDELAY=0.1 $< -o $@

install: secret4a secret4b
	install -m 111 secret4a $(INSTALLDIR)
	install -m 111 secret4b $(INSTALLDIR)

guess4: guess4.cpp driver4.cpp
	g++ -O2 -Wall guess4.cpp driver4.cpp -o guess4

clean: Makefile
	/bin/rm -f *~
	/bin/rm -f $(EXES)
