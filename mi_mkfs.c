#include "ficheros_basico.h"

void writeBlocks(int numBloques){
	
	//char *punt;
    
	unsigned char buffer[BLOCKSIZE];			//para trabajar sin memoria dinamica
	memset(buffer, 0, BLOCKSIZE);
    
    
	//punt = (char*) malloc(BLOCKSIZE*sizeof(char));	//Reservamos espacio de memoría dinámica con el tamaño de un bloque 1024 bytes
	//memset(punt, 0, BLOCKSIZE);			//Rellenamos con 0 todo el espacio en memoria virtual que hemos creado antes
    
	int i;
	for(i=0; i<numBloques; i++){	// Hemos cambiado un <= por <
		bwrite(i, buffer);			//Escribimos tantas veces como número de bloques hemos indicado por parámetro
        // en lugar de punt -> buffer.
	}
    
	//free(punt);					//Liberamos el espacio de memoria dinámica ocupada anteriormente.
    // no haría falta si no se usa mem dináminca.
}

int main (int argc, char *argv[]){
 	
	char *pnfile;			//Puntero al nombre del disco duro virtual, que será introducido por parámetros.
	int cblock;			//En esta variable guardaremos la cantidad de bloques que contendrá nuestro VHDD
	pnfile = argv[1];		//Guardamos en el puntero la primera posición de memoria del array del nombre de nuestro VHDD
	cblock = atoi(argv[2]);		//Guardamos el valor integer del segundo parametro (número de bloques)

	bmount(pnfile);			//Montamos el VHDD con el nombre que le hemos indicado
	writeBlocks(cblock);		//Escribimos los bloques indicados

	int ci = cantInodes(cblock);
	initSB(cblock, ci);
	initMB(cblock);
	initAI(ci);
    
    struct mifs_inode root;
    root.type = 'd';
    root.mod = 7;
    escribir_inode(root,0);
    
    int bfisico;
    //traducir_bloque_inode(0,400004,&bfisico,'0');
	
	
	//unsigned char pou = leer_bit(posSB+1);
	
	//printf("bit leido: %i\n",pou);

	//reservar_inode('e', 7);

	bumount();			//Desmontamos el VHDD

	exit(0);
}