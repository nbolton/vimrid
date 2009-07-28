BIN_DIR=bin
BIN_MKDIR=if ! test -d ${BIN_DIR}; then mkdir ${BIN_DIR}; fi
VER=1
VER_FULL=1.0.0
LIB=libvimrid.so
SRC=src/*.cpp src/dicom/*.cpp src/imaging/*.cpp src/imaging/processing/*.cpp src/types/*.cpp

BUILD_ARGS=-g -fpic -shared -Wl,-soname,${LIB}.${VER} -o ${BIN_DIR}/${LIB}.${VER_FULL} ${SRC}
BUILD_GPP_X86=g++ -m32 ${BUILD_ARGS}
BUILD_GPP=g++ ${BUILD_ARGS}

SO_LN=ln -sf ${LIB}.${VER_FULL} ${BIN_DIR}/${LIB}.${VER}; \
	ln -sf ${LIB}.${VER_FULL} ${BIN_DIR}/${LIB}

all:
	echo "Usage: make [ia64|x86_64|x86]"

ia64:
	make default

x86_64:
	make default

default:	
	${BIN_MKDIR}; ${BUILD_GPP}; ${SO_LN}
	
x86:
	${BIN_MKDIR}; ${BUILD_GPP_X86}; ${SO_LN}

install:
	cp ${BIN_DIR}/${LIB}.${VER_FULL} /usr/lib/; \
	ln -sf ${LIB}.${VER_FULL} /usr/lib/${LIB}.${VER};\
	ln -sf ${LIB}.${VER_FULL} /usr/lib/${LIB}

clean:
	rm -fr ${BIN_DIR}/*
