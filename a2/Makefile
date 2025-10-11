.PHONY: all
TARGET = checkp
LDFLAGS =
CXXFLAGS = -Wall
SOURCES = main.cpp checkp.cpp
OBJECTS = $(subst .cpp,.o,$(SOURCES))

CXX = g++
LD = g++

checkp: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(OBJECTS): %.o : %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $<

all: $(TARGET)

clean: Makefile
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f *~ checkp

