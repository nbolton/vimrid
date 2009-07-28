BIN_DIR=bin
BIN_MKDIR=if ! test -d ${BIN_DIR}; then mkdir ${BIN_DIR}; fi
BIN=${BIN_DIR}/vimrid-viewer
LIB=-lGL -lglut -lGLU -lSDL

TRACKD=-ltrackdAPI -DUSE_TRACKD
VIMRID=-L"../vimrid/bin" -I"../vimrid" -lvimrid

SRC=src/*.cpp \
	src/demos/cubedemos/*.cpp \
	src/demos/filterdemos/*.cpp \
	src/demos/interface/*.cpp \
	src/demos/mri/*.cpp \
	src/demos/texturing/*.cpp \
	src/glut/*.cpp \
	src/input/*.cpp \
	src/mri/*.cpp \
	src/sdl/*.cpp \
	src/sprites/*.cpp \
	src/stereo/*.cpp \
	src/test/*.cpp \
	src/textures/*.cpp \
	src/tools/*.cpp \
	src/ui/*.cpp \
	src/viewer/*.cpp
	
BUILD=g++ -g -o ${BIN} ${LIB} ${VIMRID}
STANDARD=${BIN_MKDIR}; ${BUILD}

all:
	echo "Usage: make [with-trackd|no-trackd]"

no-trackd:
	${STANDARD} ${SRC}

with-trackd:
	${STANDARD} ${TRACKD} ${SRC}

install:
	cp ${DEBUG} /usr/bin

clean:
	rm -fr ${BIN_DIR}/*
