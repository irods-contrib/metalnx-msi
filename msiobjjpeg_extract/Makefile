GCC=g++

INC=-I/usr/include/irods/ -I/usr/include/libexif -I/usr/include/irods/boost/ 

LIBS=-lexif

all: libmsiobjjpeg_extract

libmsiobjjpeg_extract:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp ${LIBS}
     
clean:
	@rm -f ./*.so