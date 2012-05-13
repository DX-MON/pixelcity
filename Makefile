GCC ?= gcc
GCC_VER = $(shell gcc -dumpversion | cut -d . -f 1)
ifeq ($(shell if [ $(GCC_VER) -ge 4 ]; then echo 1; else echo 0; fi), 1)
GCC_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
else
GCC_FLAGS = 
endif
CC = $(GCC) $(GCC_FLAGS)
EXTRA_CFLAGS = $(shell pkg-config --cflags libGTK++)
CFLAGS = -c -O2 -Wall $(EXTRA_CFLAGS) -o $@ $<
LIBS = $(shell pkg-config --libs libGTK++)
LFLAGS = $(O) $(LIBS) -o $@

O = Building.o Camera.o Car.o Deco.o Entity.o glBbox.o glMatrix.o glQuat.o glRgba.o glVector2.o glVector3.o Light.o Math.o Mesh.o Random.o Render.o Sky.o Texture.o Visible.o Win.o World.o

EXE = PixelCity.out

default: all

all: $(EXE)

install: all
	install -m755 -D $(EXE) PixelCity

$(EXE): $(O)
	$(CC) $(LFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)

clean:
	rm -f *.o *~

Building.o: Building.cpp
Camera.o: Camera.cpp
Car.o: Car.cpp
Deco.o: Deco.cpp
Entity.o: Entity.cpp
glBbox.o: glBbox.cpp
glMatrix.o: glMatrix.cpp
glQuat.o: glQuat.cpp
glRgba.o: glRgba.cpp
glVector2.o: glVector2.cpp
glVector3.o: glVector3.cpp
Light.o: Light.cpp
Math.o: Math.cpp
Mesh.o: Mesh.cpp
Random.o: Random.cpp
Render.o: Render.cpp
Sky.o: Sky.cpp
Texture.o: Texture.cpp
Visible.o: Visible.cpp
Win.o: Win.cpp
World.o: World.cpp
