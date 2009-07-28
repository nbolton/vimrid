BIN_DIR=bin
BIN_MKDIR=if ! test -d ${BIN_DIR}; then mkdir ${BIN_DIR}; fi
VER=1
VER_FULL=1.0.0
LIBIMEBRA=libimebra.so
SRC=src/base/src/*.cpp src/imebra/src/*.cpp

BUILD_ARGS=-g -shared -Wl,-soname,${LIBIMEBRA}.${VER} -o ${BIN_DIR}/${LIBIMEBRA}.${VER_FULL} ${SRC}
BUILD_GPP_X86=g++ -m32 ${BUILD_ARGS}

SO_LN=ln -sf ${LIBIMEBRA}.${VER_FULL} ${BIN_DIR}/${LIBIMEBRA}.${VER}; \
	ln -sf ${LIBIMEBRA}.${VER_FULL} ${BIN_DIR}/${LIBIMEBRA}

all:
	make gpp-x86
	
gpp-x86:
	${BIN_MKDIR}; ${BUILD_GPP_X86}; ${SO_LN}

install:
	cp ${BIN_DIR}/${LIBIMEBRA}.${VER_FULL} /usr/lib/; \
	ln -sf ${LIBIMEBRA}.${VER_FULL} /usr/lib/${LIBIMEBRA}.${VER};\
	ln -sf ${LIBIMEBRA}.${VER_FULL} /usr/lib/${LIBIMEBRA}

clean:
	rm -fr ${BIN_DIR}/*
