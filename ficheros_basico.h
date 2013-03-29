#include <time.h>
#include <limits.h>
#include "bloques.h"
#include <math.h>


#define posSB 0
#define T_INODE 128
#define npunteros 256

struct mifs_superbloque{
	unsigned int posPrimerBloqueMB;
	unsigned int posUltimoBloqueMB;
	unsigned int posPrimerBloqueAI;
	unsigned int posUltimoBloqueAI;
	unsigned int posPrimerBloqueDatos;
	unsigned int posUltimoBloqueDatos;
	unsigned int posInodeRoot;
	unsigned int posPrimerInodeLibre;
	unsigned int cantBloquesLibres;
	unsigned int cantInodesLibres;
	unsigned int totalBloques;
	unsigned int totalInodes;
	char padding[BLOCKSIZE-12*sizeof(unsigned int)];
};

struct mifs_inode{
	unsigned char type;
	unsigned char mod;
	unsigned char alignReserved[2];
	time_t atime;
	time_t mtime;
	time_t ctime;
	unsigned int cantLinksDirectory;
	unsigned int tam;
	unsigned int cantBloquesOcupados;
	unsigned int bloquesDirectos[12];
	unsigned int bloquesIndirectos[3];
	char padding[T_INODE-106];
};

int tamMB(unsigned int nbloques);
int cantInodes(unsigned int nbloques);
int tamAI(unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB(unsigned int nbloques);
int initAI(unsigned int ninodos);

//ETAPA 3
int escribir_bit(unsigned int nbloques, unsigned int bit);
unsigned char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
int escribir_inode(struct mifs_inode I, unsigned int ninode);
struct mifs_inode leer_inode(unsigned int ninode);
int reservar_inode(unsigned char type, unsigned char mod);

//ETAPA 4
int traducir_bloque_inode(unsigned int inode, unsigned int blogico, unsigned int *bfisico, char reserva);
int liberar_inode(unsigned int inode);
int liberar_bloques_inode(unsigned int inode, unsigned int blogico);