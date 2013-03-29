#include "ficheros_basico.h"
#include <time.h>
#include <stdbool.h>

unsigned int iter = 0;

int tamMB(unsigned int nbloques){
	
	int bloquesBit;
	bloquesBit = nbloques/8;
    
	if(bloquesBit%BLOCKSIZE == 0){
		return (bloquesBit/BLOCKSIZE);
	}
	else{
		return ((bloquesBit/BLOCKSIZE)+1);
	}
    
}

int cantInodes(unsigned int nbloques){
	//De manera heurística
	return (nbloques/4);
}

int tamAI(unsigned int ninodes){
    
	int nintam = ninodes * T_INODE;
	
	if(nintam%BLOCKSIZE == 0){
		return (nintam/BLOCKSIZE);
	}
	else{
		int aux = nintam/BLOCKSIZE;
		return (aux + 1);
	}
    
}

int initSB(unsigned int nbloques, unsigned int ninodes){
    
	struct mifs_superbloque SB;
    
	int tMB = tamMB(nbloques);
	int tAI = tamAI(ninodes);
    
	SB.posPrimerBloqueMB = posSB + 1;
	SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tMB - 1;
	SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
	SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tAI - 1;
	SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
	SB.posUltimoBloqueDatos = nbloques - 1;
	SB.posInodeRoot = 0;
	SB.posPrimerInodeLibre = 1;
	SB.cantBloquesLibres = nbloques - tMB - tAI;
	SB.cantInodesLibres = ninodes - 1;
	SB.totalBloques = nbloques;
	SB.totalInodes = ninodes;
    
	bwrite(posSB, &SB);
    
}

int initMB(unsigned int nbloques){
    
	struct mifs_superbloque SB;
	bread(posSB,&SB);
	
	unsigned char mb[BLOCKSIZE];
	memset(mb, 2, sizeof(mb));
    
	int i = SB.posPrimerBloqueMB;
	int k = SB.posUltimoBloqueMB;
    
	for(i; i<=k; i++){
		bwrite(i,mb);
	}
    
    int ocupats = 1 + tamMB(nbloques) + tamAI(nbloques/4);
    
    printf("Ocupats = %i\n",ocupats);
    
    int j;
    for (j=0; j<=ocupats; j++) {
        escribir_bit(j,1);
    }
}

int initAI(unsigned int ninodes){
	
	struct mifs_superbloque SB;
	bread(posSB,&SB);
    
	struct mifs_inode I[BLOCKSIZE/T_INODE];
	memset(I, 3, sizeof(I));
    
	int i, k;
    
	i = SB.posPrimerBloqueAI;
    
	k = SB.posUltimoBloqueAI;
	
	int x = 1;
	int j = 0;
    
	for (i; i<=k; i++){
		for (j; j<BLOCKSIZE/T_INODE; j++) {
			I[j].type = 'l';
			
			//AQUI TERMINA ESTO!!
            
			if (x<ninodes){
				I[j].bloquesDirectos[0] = x;
				x++;
			}
			else{
				I[j].bloquesDirectos[0] = UINT_MAX;
				j=BLOCKSIZE/T_INODE;
			}
		}
		bwrite(i,I);
	}
    
    printf("Sizeof I[0]: %li\n", sizeof(I[0]));
}

//ETAPA 3------------------------------------------------
//-------------------------------------------------------

int escribir_bit(unsigned int nbloque, unsigned int bit){ 	//nbloque es en que posicion esta el bloque el cual queremos indicar en el mapa de bits si
	int ok;
	struct mifs_superbloque SB;
	ok = bread(posSB, &SB);                         		//Leo el superbloque
	if(ok==-1){
        return -1;
	}
	int posPrimerBloqueMB = SB.posPrimerBloqueMB;
	int posbyte = nbloque / 8;                          	//Posicion del byte dentro del mapa de bits
	int posbit = nbloque % 8;                          		//Posicion del bit dentro del byte
	int posMB = posbyte / BLOCKSIZE;                		//Hemos de determinar el bloque del mapa de bits que tenemos que leer, para ello
	posMB = posMB + posPrimerBloqueMB;               		//hacemos estas dos operaciones.
	posbyte = posbyte % BLOCKSIZE;                  		//Modificamos la posicion del byte para que se ajuste a un bloque
	unsigned char mascara = 128; 							//mask 1000 0000
	mascara>>=posbit; 										//Desplazamiento de bits a la derecha.
	unsigned char bufferMB[BLOCKSIZE];
	bread(posMB,&bufferMB); 								//leo el bloque que he buscado y meto su contenido en el buffer y ahora le aplicare la mascara.
	if(bit==1){
        bufferMB[posbyte] |= mascara; 					//si el bit es 1 hago la or para bits.
        if(bwrite(posMB, bufferMB) == -1){ 				//Aqui se supone que actualizo el MB
            printf("Error al actualizar");
        }
	}else if(bit==0){
        bufferMB[posbyte] &= ~mascara; 					//Si el bit es 0 hago and y la not para bits
        if(bwrite(posMB, bufferMB) == -1){ 				//Aqui se supone que actualizo el MB
            printf("Error al actualizar");
        }
	}else{
        printf("No se ha introducido un valor de bit correcto.");
        return -1; 										//Cuando el valor del bit no es ni 0 ni 1.
	}
}

unsigned char leer_bit(unsigned int nbloque){   //Esto deberia estar
	
	struct mifs_superbloque SB;
	bread(posSB,&SB);
	
	int posbyte = nbloque / 8;
	int posbit = nbloque % 8;
	unsigned char mask = 128;	// 1000 0000
	unsigned char bufferMB[BLOCKSIZE];
	int bloqueMB = (posbyte/BLOCKSIZE) + SB.posPrimerBloqueMB;
    
    posbyte = posbyte % BLOCKSIZE;
    
    bread(bloqueMB,bufferMB);           //leemos el bloque
	
	mask >>= posbit;
	mask &= bufferMB[posbyte];
	mask >>= (7-posbit);
	
	return mask;
}

int reservar_bloque(){
	
	struct mifs_superbloque SB;
	bread(posSB,&SB);
    
	int pMB = SB.posPrimerBloqueMB;
	
	unsigned char mask = 128;	// 1000 0000
	
	if (SB.cantBloquesLibres != 0){
		
		unsigned char bfaux[BLOCKSIZE];
		unsigned char bfMB[BLOCKSIZE];
        
		memset(bfaux, 255, BLOCKSIZE);
        
		bool found = false;
		
		while(!found){  // ENCONTRAR PRIMER BLOQUE LIBRE
            
			bread(pMB, bfMB);
            
			if(memcmp(bfMB, bfaux, BLOCKSIZE) != 0){
				found = true;
			}
            
			pMB++;
			
		}
        
		pMB = pMB - 1;
        
		int posByte = 0;
        
		while(bfMB[posByte]>= 255){
			posByte++;
		}
        
		int posBit = 0;
        
		while(bfMB[posByte] & mask){
            
			bfMB[posByte] <<= 1;
			posBit++;
            
		}
        
		int numBloque = (((pMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posByte) * 8 + posBit);
        
		int err = 0;
		
		if(escribir_bit(numBloque, 1) != -1){
			
			SB.cantBloquesLibres -= 1;
			err =  bwrite(posSB, &SB);
		}
        
		if(err == -1) numBloque = -1;
		return numBloque;
        
	}
	else{
		printf("No queda espacio en el vHDD\n");
	}
}

int liberar_bloque(unsigned int nbloque){
	
	escribir_bit(nbloque, 0);
	
	struct mifs_superbloque SB;
	bread(posSB,&SB);
    
	SB.cantBloquesLibres = SB.cantBloquesLibres + 1;
	
	bwrite(posSB, &SB);
    
	return nbloque;
    
}


int escribir_inode(struct mifs_inode I, unsigned int ninode){
	
	struct mifs_superbloque SB;
	bread(posSB,&SB);
	
	int pAI = SB.posPrimerBloqueAI;
	int tamArray = BLOCKSIZE / T_INODE;
	int nBloque = ninode / tamArray;
	int numInode = ninode % tamArray;
	
	struct mifs_inode Inode[tamArray];
	
	int errorRead = bread((pAI + nBloque), &Inode);
	
	if(errorRead != -1){
		Inode[numInode] = I;
		return bwrite((pAI + nBloque), &Inode);
	}else{
		return -1;
	}
	
}

struct mifs_inode leer_inode(unsigned int ninode){
	
	struct mifs_superbloque SB;
	bread(posSB,&SB);
	
	int pAI = SB.posPrimerBloqueAI;
	int tamArray = BLOCKSIZE / T_INODE;
	int nBloque = ninode / tamArray;
	
	struct mifs_inode Inode[tamArray];
	
	bread((pAI + nBloque), &Inode);
	
	return Inode[ninode%tamArray];
	
}

int reservar_inode(unsigned char type, unsigned char mod){
	
	struct mifs_superbloque SB;
	bread(posSB,&SB);
	
	int pInodeLibre = SB.posPrimerInodeLibre;
	
	struct mifs_inode Inode;
	
    // Inicializamos el inode
	Inode.type = type;
	Inode.mod = mod;
	Inode.cantLinksDirectory = 1;
	Inode.tam = 0;
	Inode.atime = time(NULL);
	Inode.mtime = time(NULL);
	Inode.ctime = time(NULL);
	Inode.cantBloquesOcupados = 0;
	
    // Inicializamos los punteros directos e indirectos.
	memset(Inode.bloquesDirectos, 0, sizeof(Inode.bloquesDirectos));
	memset(Inode.bloquesIndirectos, 0, sizeof(Inode.bloquesIndirectos));
	
    // Leemos el primer inode libre
    struct mifs_inode freeinode = leer_inode(pInodeLibre);
    int posSiguienteInodeLibre = freeinode.bloquesDirectos[0];
    
    // Escribimos un inode
	escribir_inode(Inode, pInodeLibre);
    
    // Asignamos la posición del siguiente inode libre
    SB.posPrimerInodeLibre = posSiguienteInodeLibre;
    
    // Sobreescritura del SuperBloque
    bwrite(posSB,&SB);
	
}

//ETAPA 4------------------------------------------------
//-------------------------------------------------------

int obtener_bfisico(unsigned int blogico,unsigned int anterior,unsigned int actual){        // blogico, 0, 11, 0 primera iter
    
    if(iter<=3){
        
        if(anterior<=blogico && blogico<=actual){                                           // vol dir que hem probat el el bfisic
            
            return blogico - anterior;                                                      // retornam el valor relatiu
            
        }else{                                                                              // sino feim una crida recursiva
            actual = actual +1;
            iter++;
            int potencia = (int) pow(npunteros,iter);
            return obtener_bfisico(blogico, actual, actual + potencia-1);
        }
        
    }else{
        return -1;
    }
}

void obtener_punteros(unsigned int blogico, unsigned int longitud, unsigned int *p_array){
    
    
    switch (longitud) {
            
        case 2:                                         // vol dir que es indirectes 1
            
            p_array[0]=blogico/npunteros;                 // obtenemos las direcciones de los punteros
            p_array[1]=blogico%npunteros;
            break;
            
        case 3:                                         // vol dir que es indirectes 2
            
            p_array[0] = blogico / (npunteros*npunteros); // obtenemos las direcciones de los punteros
            blogico = blogico % (npunteros*npunteros);
            p_array[1] = blogico / npunteros;
            p_array[2] = blogico % npunteros;
            
        default:
            break;
    }
}

int traducir_bloque_inode(unsigned int ninode, unsigned int blogico, unsigned int *bfisico, char reservar){
    
    struct mifs_inode I = leer_inode(ninode);               // creamos y leemos el inodo en question
    
    iter = 0;                                                   // creamos un iterador
    
    int bfisic = obtener_bfisico(blogico,0,11);                 // obtenemos el bloque fisico
    
    unsigned int array [iter];
    
    if(iter==1){                                                // rellenamos el array segun parezca
        
        array[0] = bfisic;
        
    }else{
        
        obtener_punteros(bfisic,iter,array);
        
        printf("Arrays %i %i %i\n", array[0],array[1],array[2]);
    }
    
    
    if(reservar==1){                                                    // ESCRITURA
        
        if(bfisic>11){                                                  // no es punter directe
            
            switch (iter) {
                    
                    unsigned int buffer1 [256], buffer2[256], buffer3[256];   // buffer auxiliar de 256 int que contendran les "direccions" de dades
                    
                    int pos;
                    
                    
                    
                case 1:
                    
                    if(I.bloquesIndirectos[0]==0){                     // PRIMER - DADES
                        
                        I.bloquesIndirectos[0] = reservar_bloque();    // reservam es primer bloc de punters
                        
                        buffer1[array[0]] = reservar_bloque();          // reservam es bloc de dades
                        
                        pos = I.bloquesIndirectos[0];                  // asignam sa direccio des bloc reservat
                        
                        bwrite(pos,&buffer1);                           // sobreescrivim
                        
                    }else{                                              // LLEGIR PRIMER (EXISTEIX)
                        
                        pos = I.bloquesIndirectos[0];                  // calculam es bloc que apunta es punter indirecte
                        
                        bread(pos,&buffer1);                            // llegim
                        
                        if (buffer1[array[0]]==0) {                     // DADES
                            
                            buffer1[array[0]]= reservar_bloque();       // reservam es bloc de dades
                            
                        }
                    }
                    
                    bfisico = &buffer1[array[0]];                       // asignam es punter de dades
                    
                    break;
                    
                case 2:
                    
                    if(I.bloquesIndirectos[1]==0){                     // PRIMER - SEGON - DADES
                        
                        I.bloquesIndirectos[1] = reservar_bloque();    // cream es primer bloc
                        
                        pos = I.bloquesIndirectos[1];                  // calculam es bloc que apunta es punter indirecte
                        
                        buffer1[array[0]] = reservar_bloque();          // cream es segon bloc
                        
                        bwrite(pos,&buffer1);                           // escrivim a memoria
                        
                        pos = buffer1[array[0]];                        // refrescam sa posicio
                        
                        buffer2[array[1]] = reservar_bloque();          // apuntam al bloc de dades
                        
                        bfisico = &buffer2[array[1]];                   // asignam es punter de dades
                        
                        bwrite(pos,&buffer2);                           // sobrescrivim
                        
                    }else{                                              // LLEGIR PRIMER (EXISTEIX)
                        
                        pos = I.bloquesIndirectos[1];                  // agafam sa direccio des bloc de punters
                        
                        bread(pos, &buffer1);                           // llegim es bloc de punters
                        
                        pos = buffer1[array[0]];                        // recalculam sa posicio d'escriptura
                        
                        if (buffer1[array[0]]==0) {                     // SEGON - DADES
                            
                            buffer1[array[0]] =  reservar_bloque();     // reservam es segon bloc
                            
                            bwrite(pos, &buffer1);                      // escrivim a sa posicio des primer bloc, sa direccio des segon bloc
                            
                            pos = buffer2[array[1]];                    // refrescam sa posicio
                            
                            buffer2[array[1]] = reservar_bloque();      // reservam es bloc de dades
                            
                            bwrite(pos,&buffer2);                       // escrivim a sa posicio des segon bloc, sa direccio des bloc de dades
                            
                        }else{                                          // LLEGIR SEGON (EXISTEIX)
                            
                            bread(pos, &buffer2);                       // llegim es segon bloc
                            
                            if(buffer2[array[1]]==0){                   // DADES
                                
                                buffer2[array[1]] = reservar_bloque();  // reservam es bloc de dades
                                
                                pos = buffer2[array[1]];                // refrescam sa posicio
                                
                                bwrite(pos, &buffer2);                  // escrivim a sa posicio des segon bloc, sa direccio des bloc de dades
                                
                            }                                           // si existeix es bloc de dades
                            
                        }
                        
                        bfisico = &buffer2[array[1]];                   // asignam es punter de dades
                        
                    }
                    
                    break;
                    
                case 3:
                    
                    if (I.bloquesIndirectos[2]==0) {                   // PRIMER - SEGON - TERCER - DADES
                        
                        I.bloquesIndirectos[2] = reservar_bloque();    // cream es primer bloc
                        
                        pos = I.bloquesIndirectos[2];                  // calculam es bloc que apunta es punter indirecte
                        
                        buffer1[array[0]] = reservar_bloque();          // cream es segon bloc
                        
                        bwrite(pos,&buffer1);                           // escrivim es bloc 1 amb sa posicio des bloc 2
                        
                        pos = buffer1[array[0]];                        // refrescam sa posicio
                        
                        buffer2[array[1]] = reservar_bloque();          // apuntam en es tercer bloc
                        
                        bwrite(pos,&buffer2);                           // escrivim es bloc 2 amb sa posicio des bloc 3 dedins
                        
                        pos = buffer2[array[1]];                        // refrescam sa posicio
                        
                        buffer3[array[2]] = reservar_bloque();          // apuntam al bloc de dades
                        
                        bwrite(pos, &buffer3);                          // escrivim es bloc 3 amb sa posicio des bloc de dades dedins
                        
                        bfisico = &buffer3[array[2]];                   // asignam
                        
                    }else{                                              // LLEGIR PRIMER (EXISTEIX)
                        
                        pos = I.bloquesIndirectos[2];                  // agafam sa direccio des bloc de punters
                        
                        bread(pos, &buffer1);                           // llegim es primer bloc de punters
                        
                        if (buffer1[array[0]]==0) {                     // SEGON - TECER - DADES
                            
                            buffer1[array[0]] =  reservar_bloque();     // reservam es segon bloc
                            
                            bwrite(pos, &buffer1);                      // escrivim es primer bloc amb sa direccio des segon bloc dedins
                            
                            pos = buffer1[array[0]];                    // recalculam sa posicio d'escriptura
                            
                            buffer2[array[1]] = reservar_bloque();      // reservam es tercer bloc
                            
                            bwrite(pos,&buffer2);                       // escrivim es bloc 2 amb sa direccio des tercer bloc dedins
                            
                            pos = buffer2[array[1]];                    // recalculam sa posicio
                            
                            buffer3[array[2]] = reservar_bloque();      // reservam es bloc de dades
                            
                            bwrite(pos,&buffer3);                       // escrivim es bloc 3
                            
                            bfisico = &buffer3[array[2]];               // retornam es bloc de dades reservat
                            
                            
                        }else{                                          // LLEGIR SEGON (EXISTEIX)
                            
                            pos = buffer1[array[0]];                    // recalculam sa posicio d'escriptura
                            
                            bread(pos, &buffer2);                       // llegim es segon bloc
                            
                            if(buffer2[array[1]]==0){                   // TERCER - DADES
                                
                                buffer2[array[1]] = reservar_bloque();  // reservam es tercer bloc
                                
                                bwrite(pos, &buffer2);                  // escrivim es segon bloc amb sa direccio des tercer bloc
                                
                                pos = buffer2[array[1]];                // refrescam sa posicio
                                
                                buffer3[array[2]] = reservar_bloque();  // reservam es bloc de dades
                                
                                bwrite(pos, &buffer3);                  // escrivim es tercer bloc amb sa direccio des bloc de dades dedins
                                
                            }else{                                      // LLEGIR TERCER (EXISTEIX)
                                
                                pos = buffer2[array[1]];                // refrescam amb sa direccio des tercer bloc
                                
                                bread(pos, &buffer3);                   // llegim es tercer bloc
                                
                                if(buffer3[array[2]]==0){               // DADES
                                    
                                    buffer3[array[2]] = reservar_bloque();  // reservam es bloc de dades
                                    
                                    bwrite(pos,&buffer3);               // escrivim es bloc 3 amb
                                    
                                }
                            }
                            
                            bfisico = &buffer3[array[2]];               // asignam es bloc de dades
                            
                        }
                        
                    }
                    
                    break;
                    
                default:
                    
                    break;
            }
            
        }else{                                                          // vol dir que es un punter directe
            
            if(I.bloquesDirectos[array[0]]==0){                        // si no existeix
                
                I.bloquesDirectos[array[0]] = reservar_bloque();       // reservam es bloc de dades
            }
            
            bfisico = &I.bloquesDirectos[array[0]];                     // asignam
            
        }
        
    }else{      //LECTURA
        
        if(bfisic>11){                                                  // PUNTER INDIRECTE
            
            unsigned int buffer1 [256], buffer2[256], buffer3[256];   // buffer auxiliar de 256 int que contendran les "direccions" de dades
            
            int pos;
            
            switch (iter) {
                    
                    
                case 1:
                    
                    if(I.bloquesIndirectos[0]==0){                     // vol dir que no hi ha creat el array de punter IND 0
                        
                        bfisico = -1;                                   // sortim
                        
                    }else{                                              // en es cas de que estigui creat nomes cal llegir
                        
                        pos = I.bloquesIndirectos[0];              // calculam es bloc que apunta es punter indirecte
                        
                        bread(pos,&buffer1);                             // llegim
                        
                        bfisico = &buffer1[array[0]];                    // asignam es punter de dades
                        
                        bwrite(pos,&buffer1);                            // sobreescrivim
                    }
                    
                    break;
                    
                case 2:
                    
                    if(I.bloquesIndirectos[1]==0){               // vol dir que no hi ha creat s'array de punter IND 1
                        
                        bfisico = -1;                               // sortim
                        
                    }else{                                          // vol dir que hi ha creat es primer bloc de punters
                        
                        pos = I.bloquesIndirectos[1];          // agafam sa direccio des bloc de punters
                        
                        bread(pos, &buffer1);                       // llegim es bloc de punters
                        
                        pos = buffer1[array[0]];                    // recalculam sa posicio d'escriptura
                        
                        if (buffer1[array[0]]==0) {                 // vol dir que es segon bloc no existeix
                            
                            bfisico = -1;                           // sortim
                            
                        }else{                                      // vol dir que es segon bloc si existeix
                            
                            bread(pos, &buffer2);                   // llegim es bloc
                            
                            if(buffer2[array[1]]==0){               // vol dir que es bloc de dades no existeix
                                
                                bfisico = -1;                       // sortim
                                
                            }else{                                       // si existeix es bloc de dades
                                
                                bfisico = &buffer2[array[1]];           // asignam es punter de dades
                                
                            }
                        }
                        
                    }
                    
                    break;
                    
                case 3:
                    
                    if (I.bloquesIndirectos[2]==0) {               // vol dir que es primer bloc de punters no existeix i per tant hem de crear tota s'estructura
                        
                        bfisico = -1;                       // sortim
                        
                    }else{                                          // vol dir que es primer bloc existeix
                        
                        
                        pos = I.bloquesIndirectos[2];          // agafam sa direccio des bloc de punters
                        
                        bread(pos, &buffer1);                       // llegim es bloc de punters
                        
                        if (buffer1[array[0]]==0) {                 // vol dir que es segon bloc no existeix
                            
                            bfisico = -1;                       // sortim
                            
                        }else{                                      // vol dir que es segon bloc si existeix
                            
                            pos = buffer1[array[0]];                    // recalculam sa posicio d'escriptura
                            
                            bread(pos, &buffer2);                   // llegim es bloc
                            
                            if(buffer2[array[1]]==0){               // vol dir que es tercer bloc no existeix
                                
                                bfisico = -1;                       // sortim
                                
                            }else{                                       // si existeix es tercer bloc
                                
                                pos = buffer2[array[1]];            // refrescam amb sa direccio des tercer bloc
                                
                                bread(pos, &buffer3);               // llegim es tercer bloc
                                
                                if(buffer3[array[2]]==0){           // vol dir que es bloc de dades no existeix
                                    
                                    buffer3[array[2]] = reservar_bloque();  // reservam es bloc de dades
                                    
                                    bwrite(pos,&buffer3);           // escrivim es bloc 3 amb
                                    
                                }
                                
                                bfisico = &buffer3[array[2]];       // asignam es bloc fisic
                                
                            }
                            
                            bfisico = &buffer2[array[1]];           // asignam es punter de dades
                            
                        }
                    }
                    
                    break;
                    
                default:
                    
                    break;
            }
            
        }else{                                                      // vol dir que es un punter directe
            
            if(I.bloquesDirectos[array[0]]==0){                    // si no existeix
                
                bfisico = -1;
                
            }else{
                
                bfisico = &I.bloquesDirectos[array[0]];                 // asignam
                
            }
            
        }
        
    }
    
     return bfisico;
     if(bfisic == -1){                               // quiere decir que el blogico no existe
     
     printf("El bloque logico no existe!!\n");
     
     }
}

int liberar_bloques_inode(unsigned int ninode, unsigned int blogico){
	
    struct mifs_superbloque SB;
	bread(posSB,&SB);
    
    struct mifs_inode Inode = leer_inode(ninode);
    
    int i;
    int max = Inode.tam/BLOCKSIZE ;                 //Esta caquita no sabemos que es
                                                    //y siempre da cero ya que Inode.tam és cero.
    for (i=blogico; i<=max; i++) {
        liberar_bloque(i);
    }
	
}

int liberar_inode(unsigned int ninode){
	
    struct mifs_superbloque SB;
    bread(posSB,&SB);
    
    struct mifs_inode Inode = leer_inode(ninode);
    
    Inode.type = 'l';
    
    int posAI = SB.posPrimerBloqueAI;
    int nbloque = ninode /(BLOCKSIZE/T_INODE);
    int blogico = posAI + nbloque;
    
    SB.posPrimerInodeLibre = blogico;
    
    liberar_bloques_inode(ninode, blogico);
    
    SB.cantInodesLibres = SB.cantInodesLibres + 1;
    
    escribir_inode(Inode, ninode);
    bwrite(posSB, &SB);
    
    return blogico;
}