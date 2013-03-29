/*fichero bloques.h */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* Modos de apertura y función open()*/
#include <stdlib.h> /* Funciones write() y close() */
#include <string.h>

#define BLOCKSIZE 1024 //bytes

static int DESCRIPTOR = 0;

int bmount(const char *camino);
int bumount();
int bwrite(unsigned int bloque, const void *buf);
int bread(unsigned int bloque, void *buf);
