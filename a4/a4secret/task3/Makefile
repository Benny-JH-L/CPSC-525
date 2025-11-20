include ../config.mk

EXES = guess3 secret3a secret3b

.PHONY: all clean install
all: $(EXES)

$(EXES) : Makefile

secret3a: secret3.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"cake\" -DDELAY=0.2 $< -o $@

secret3b: secret3.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"much0harderz\" -DDELAY=0.1 $< -o $@

# secret3c:
#     gcc -O2 -Wall -static -DSTORED_SECRET=\"z0z0z0z0\" -DDELAY=0.01 $< -o $@

install: secret3a secret3b
	install -m 111 secret3a $(INSTALLDIR)
	install -m 111 secret3b $(INSTALLDIR)

guess3: guess3.cpp driver3.cpp
	g++ -O2 -Wall guess3.cpp driver3.cpp -o guess3

clean: Makefile
	/bin/rm -f *~
	/bin/rm -f $(EXES)
