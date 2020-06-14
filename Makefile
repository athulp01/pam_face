CC = g++
CFLAGS = -Wall -fPIC -O3
LDFLAGS = -ldlib -lcblas -llapack -lgif

all: pam_face

src/capture.o: src/capture.c include/capture.h
	$(CC) $(CFLAGS) -c src/capture.c -o src/capture.o

src/face_recog.o: src/face_recog.cpp include/face_recog.h
	$(CC) $(CFLAGS) -c src/face_recog.cpp -o src/face_recog.o

src/pam_face.o: src/pam_face.cpp
	$(CC) $(CFLAGS) -c src/pam_face.cpp -o src/pam_face.o

pam_face: src/capture.o src/face_recog.o src/pam_face.o
	$(CC) $(CFLAGS) $(LDFLAGS) -shared src/capture.o src/pam_face.o src/face_recog.o -o libpamface.so

install:
	sudo cp libpamface.so /lib/security/pam_face.so

clean:
	rm src/*.o libpamface.so
