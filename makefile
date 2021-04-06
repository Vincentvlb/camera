SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
CPPFLAGS= -g2 $(OPT) -Wall $(INCL) $(shell sdl2-config --cflags) $(shell pkg-config --cflags opencv4) -std=c++17 
LDFLAGS= $(shell sdl2-config --libs) $(shell pkg-config --libs opencv4) $(LIBS) -lSDL2_ttf
EXE=camera

all: $(OBJS)
	g++ -g2 $(OBJS) $(LDFLAGS)  -o $(EXE)
#-lasan -fsanitize=address

clean: 
	rm -f core
	rm -f *.a
	rm -f *.so
	rm -f $(OBJS)
	rm -f $(EXE)