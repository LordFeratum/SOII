#include "bloques.h"

int bmount(const char *camino){
	DESCRIPTOR = open(camino, O_RDWR|O_CREAT, 0666);
 	if(DESCRIPTOR == -1){
		printf("Has nadado como un pececillo!");
		exit(1);
	}
	return DESCRIPTOR;
}

int bumount(){
	return close(DESCRIPTOR);
}

int bwrite(unsigned int bloque, const void * buf){
	int err;
	int err2;
	int desplazamiento;

	desplazamiento = bloque * BLOCKSIZE;

	err2 = lseek(DESCRIPTOR, desplazamiento, SEEK_SET);

	if(err2<0){
		printf("Has nadado con el lseek del bwrite");
		exit(1);
	}

	err = write(DESCRIPTOR, buf, BLOCKSIZE);

	if(err != BLOCKSIZE){
		printf("Has nadado como un pececillo, AGAIN!");
		exit(1);
	}

	return err;
}

int bread(unsigned int bloque, void * buf){
	int desplazamiento;
	desplazamiento = bloque * BLOCKSIZE;
	if(lseek(DESCRIPTOR, desplazamiento, SEEK_SET)<0){
		printf("Has nadado con el lseek del bread");
	}
	return read(DESCRIPTOR, buf, BLOCKSIZE);
}
