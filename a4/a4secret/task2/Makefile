include ../config.mk

EXES = guess2 secret2a secret2b

.PHONY: all clean install
all: $(EXES)

$(EXES) : Makefile

secret2a: secret2.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"easy1\" -DDELAY=0.5 $< -o $@

secret2b: secret2.c Makefile
	gcc -O2 -Wall -static -DSTORED_SECRET=\"d1ff1cult990z\" -DDELAY=0.1 $< -o $@

# secret2c:
# 	gcc -O2 -Wall -static -DSTORED_SECRET=\"133eex89293f92z363012826f1c4c89d521210c17dcdcecca30cd20705332d44\" -DDELAY=0.154 $< -o $@

install: secret2a secret2b
	install -m 111 secret2a $(INSTALLDIR)
	install -m 111 secret2b $(INSTALLDIR)

guess2: guess2.cpp driver2.cpp
	g++ -O2 -Wall guess2.cpp driver2.cpp -o guess2

clean: Makefile
	/bin/rm -f *~
	/bin/rm -f $(EXES)
