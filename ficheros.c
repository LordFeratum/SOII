#include <stdio.h>
#include "ficheros_basicos.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
    
    struct mifs_inode I = leer_inode(ninode);
    
    if ((I.mod & 2) == 2) {                     //tienes permisos de write
        
        int primerBloque = offset / BLOCKSIZE;
        int ultimoBloque = (offset + nbytes -1) / BLOCKSIZE;
        
        int escritos = -1;
        
        int bfisico;
        traducir_bloque_inode(ninode, primerBloque, &bfisico, 1);
        
        int despl1 = offset % BLOCKSIZE;
        int despl2 = (offset + nbytes -1) % BLOCKSIZE;
        
        unsigned char buf_bloque[BLOCKSIZE];
        
        
        if (primerBloque == ultimoBloque) {                      //El bloque a escribir cabe en un único bloque.
            
            bread(bfisico, &buf_bloque);
            memcopy(buf_bloque + despl1, buf_original, nbytes);
            bwrite(bfisco, &buf_bloque);
            escritos = nbytes;
            
        }
        else{                                   //El bloque a escribir va a ocupar varios bloques consecutivos.
            
			// GUARDAMOS EL PRIMER BLOQUE
			
            bread(bfisico, &buf_bloque);
            memcopy(buf_bloque + despl1, buf_original, BLOCKSIZE - despl1);
            bwrite(bfisco, &buf_bloque);
            
            escritos = (BLOCKSIZE - despl1);
            
            // END
            // GUARDAMOS BLOQUES INTERMEDIOS
            
            int i = primerBloque + 1;
            
            for(i; i<ultimoBloque; i++){
				
				traducir_bloque_inode(ninode, i, &bfisico, 1);
				void *op = buf_original + (BLOCKSIZE - despl1) + (i - primerBloque - 1) * BLOCKSIZE;
				bwrite(bfisico, &op);
				
				escritos = escritos + BLOCKSIZE;
			
			}
			
			// END
			// GUARDAMOS BLOQUE FINAL
			
			traducir_bloque_inode(ninode, ultimoBloque, &bfisico, 1);
			bread(bfisico, &buf_bloque);
			memcpy(buf_bloque, buf_original + (BLOCKSIZE - despl1) + (ultimoBloque - primerBloque - 1) * BLOCKSIZE, despl2 + 1);
			bwrite(bfisico, &buf_bloque);
			escritos = escritos + (despl2 + 1); 
			
			// END
            
        }
        
        //ACTUALIZAMOS LA MARCA DE TIEMPO DEL INODO
        
        I.atime = time(NULL);
        I.ctime = time(NULL);
        
        if((offset + nbytes) > I.tam){	
			I.tam = (offset + nbytes);
        }
        
        escribir_inode(I, ninode);
        
    }
    else{
			printf("You havent write permissions");
	}
    
    return escritos;
    
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    
	struct mifs_inode I = leer_inode(ninodo);
	
	int primerBloque = offset / BLOCKSIZE;
	int ultimoBloque = (offset + nbytes -1) / BLOCKSIZE;
	
	int bfisico;
	int leidos = -1;
	
    int despl1 = offset % BLOCKSIZE;
    int despl2 = (offset + nbytes -1) % BLOCKSIZE;

	if((I.mod & 4) == 4){
		
		char buf_bloque[BLOCKSIZE];
		
		if(primerBloque == ultimoBloque){
			
			traducir_bloque_inode(ninodo, primerBloque, &bfisico, 1);
			
			if(bfisico != -1){
					bread(bfisico, &buf_bloque);
					memcpy(buf_original, buf_bloque + despl1, nbytes);
					leidos = nbytes;
			}
	
			
		}
		else{
			
				traducir_bloque_inode(ninodo, primerBloque, &bfisico, 1);
		    	
		    	if(bfisico != -1){
					
					bread(bfisico, &buf_bloque);
					memcpy(buf_original, buf_bloque + despl1, nbytes);
					leidos = BLOCKSIZE - despl1;
					
					unsigned int 
					
				}
		    	
		}
		
	}

    
}
