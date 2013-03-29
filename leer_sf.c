#include "ficheros_basico.h"


int main(int argc, char *argv[]){

	char *pnfile;
	pnfile = argv[1];
	bmount(pnfile);
	struct mifs_superbloque SB;
	int i = bread(posSB, &SB);
	
	printf("Posición primer bloque MB: %i\n", SB.posPrimerBloqueMB);
	printf("Posición último bloque MB: %i\n", SB.posUltimoBloqueMB);
	printf("Posición primer bloque AI: %i\n", SB.posPrimerBloqueAI);
	printf("Posición último bloque AI: %i\n", SB.posUltimoBloqueAI);
	printf("Posición primer bloque Datos: %i\n", SB.posPrimerBloqueDatos);
	printf("Posición último bloque Datos: %i\n", SB.posUltimoBloqueDatos);
	printf("Posición de Inode Root: %i\n", SB.posInodeRoot);
	printf("Posición primer inode libre: %i\n", SB.posPrimerInodeLibre);
	printf("Bloques libres: %i\n", SB.cantBloquesLibres);
	printf("Inodes libres: %i\n", SB.cantInodesLibres);
	printf("Total bloques: %i\n", SB.totalBloques);
	printf("Total inodes: %i\n\n", SB.totalInodes);	


	bumount(pnfile);

}
