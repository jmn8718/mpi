#include "matriz.h"
#include <stdio.h>
#include <mpi.h>

#define MAESTRO	0
#define	TAG	1
#define	TAREA	3
#define	PARAR	-6
#define SIZE	N2

int main(argc,argv)
int argc;
char *argv[];  {

	int numtasks, rank, i, j, k, x, y;
	double startwtime, endwtime;
	double buffer[SIZE] = {0};
	MPI_Status stat;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if(numtasks>1){
		generarMatrices();
		printf("Soy %d de %d, y empezamos ya\n",rank,numtasks);
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank==MAESTRO){  //TRABAJO DEL MASTER
	        printf("Soy %d de %d, y empiezo a repartir fila\n",rank,numtasks);
			startwtime = MPI_Wtime();
			for (i=0; i<M1; i++){
				MPI_Recv(&buffer,SIZE,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
				x = stat.MPI_TAG-1;
				for(k=0;k<SIZE;k++)
					matR[x][k]+=buffer[k];
				MPI_Send(&i,1,MPI_INT,stat.MPI_SOURCE,TAREA,MPI_COMM_WORLD);
			}
			i=0;
			while(i<numtasks-1){
				MPI_Recv(&buffer,SIZE,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&stat);
				x = stat.MPI_TAG-1;
				for(k=0;k<SIZE;k++)
					matR[x][k]+=buffer[k];
                                j=PARAR;
				MPI_Send(&j,1,MPI_INT,stat.MPI_SOURCE,TAREA,MPI_COMM_WORLD);
				i++;
			}
		} else { //TRABAJO DEL WORKER
			int tarea=0; double resul;
			while(tarea>=0){
				MPI_Send(&buffer,SIZE,MPI_DOUBLE,MAESTRO,tarea+1,MPI_COMM_WORLD);
				MPI_Recv(&tarea,1,MPI_INT,MAESTRO,TAREA,MPI_COMM_WORLD,&stat);
				if(tarea>=0){
					for(j=0;j<SIZE;j++){
						resul = 0.0;
						for(k=0;k<N1;k++)
							resul+=mat1[tarea][k]*mat2[k][j];
						buffer[j]=resul;
					}
				}
			}
		} //end if TRABAJOS

		MPI_Barrier(MPI_COMM_WORLD);
		if(rank==MAESTRO){
			endwtime = MPI_Wtime();
			printf("Ha tardado %.4f segundos\n",endwtime-startwtime);
		}
		printf("Soy %d de %d, y ya hemos terminado\n",rank,numtasks);
	} else
		printf("Debe de indicar al menos 2 procesos\n");
	if(rank==MAESTRO)
		escribirResultado(startwtime,endwtime,"resulFila.txt");
	MPI_Finalize();
	return(0);
}
