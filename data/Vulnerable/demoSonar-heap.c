#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIALSIZE 20

struct equipo{
    int ID;
    int tEnt;
    char nombre[20];
		
}; 

struct heap{
    int size;
    int last;
    struct equipo * arrayEquipos;
}; 

void swap(struct equipo * eq1, struct equipo * eq2){
    struct equipo tmpEq;
    tmpEq = *eq1;
    *eq1 = *eq2;
    *eq2 = tmpEq;
}
    


void initializeHeap( struct heap * heapSt){
    heapSt->size = INITIALSIZE;
    heapSt->last = 0;
    heapSt->arrayEquipos = malloc (INITIALSIZE * sizeof(struct equipo));

}

void growHeap( struct heap * heapSt){
    int i;
    int newSize = heapSt->size + (int)(heapSt->size * 0.5);
    heapSt->size = newSize;
    heapSt->arrayEquipos = realloc (heapSt->arrayEquipos, (heapSt->size) * sizeof(struct equipo));

}
	

void insertarEquipo( struct equipo * equipoSt, struct heap * heapSt){
    int i, p;
    if (heapSt->last == heapSt->size){
	growHeap(heapSt);
	printf("INFO: se está redimencionando la cola del CDE\n");
    }
   (heapSt->last)++;
   heapSt->arrayEquipos[heapSt->last] = *equipoSt;
   for(i=heapSt->last; i > 1 && heapSt->arrayEquipos[p=(i/2)].tEnt > heapSt->arrayEquipos[i].tEnt ; i = p){
   	swap(&heapSt->arrayEquipos[p], &heapSt->arrayEquipos[i]);
   }
   
   
   
 
} 

int removerEquipo( struct equipo * equipoSt, struct heap * heapSt){
    int i, c;
    if(heapSt->last == 0){
	printf("ERROR: el heap está vacío\n");
	return 0; 
    }
    else{
        *equipoSt = heapSt->arrayEquipos[1];
	heapSt->arrayEquipos[1] = heapSt->arrayEquipos[(heapSt->last)--];
	for(i=1; (c=2*i) <= heapSt->last; i = c){
	    if(c+1 <= heapSt->last && heapSt->arrayEquipos[c+1].tEnt < heapSt->arrayEquipos[c].tEnt)
		c++;
	    if(heapSt->arrayEquipos[i].tEnt <= heapSt->arrayEquipos[c].tEnt)
		break;
	    swap(&heapSt->arrayEquipos[c], &heapSt->arrayEquipos[i]);
		    
	}
	return 1; //OK
    }
    
}

void automatedTest(struct heap * heapSt, int times){
    struct equipo eq;
    int i;
    for (i=0;i<times;i++){
	
	eq.ID = (int) (rand() % (times * 10));
	eq.tEnt = (int) (1 + rand() % 360);
	snprintf(eq.nombre, 20, "Equipo %d\0", eq.ID);
	insertarEquipo(&eq, heapSt);
    }
}


int main(int argc, char * argv[]){
    struct heap colaCDE;
    struct equipo eq;
    int IDEquipo=1, tiempoAEntrenar=0;
    char nombreEquipo[20];
    initializeHeap(&colaCDE);

    if (argc >= 2){
	    printf("Inicializando Tests Automatizado\n");
	    automatedTest(&colaCDE, atoi(argv[2]));
    }else{
    	for(;;){
		printf("Ingrese el nombre del equipo %d:\n", IDEquipo);
		scanf("%s", nombreEquipo);
		printf("Ingrese el tiempo a Entrenar para el equipo %d\n", IDEquipo);
		scanf("%d", &tiempoAEntrenar);
		if (tiempoAEntrenar < 0)
	    		break;
		eq.ID = IDEquipo;
		strcpy(eq.nombre, nombreEquipo);
		eq.tEnt = tiempoAEntrenar;
		insertarEquipo( &eq, &colaCDE); 
		IDEquipo++;
    	}
    }
	
    
    printf("-----------------------------------\n");
    while ( removerEquipo( &eq, &colaCDE) ){
	printf("ID: %d, Nombre: %s, tiempo: %d\n", eq.ID, eq.nombre, eq.tEnt);
	
    }
    
	
	

    
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 110,8;110,12

