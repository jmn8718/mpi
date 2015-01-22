#include "matriz.h"
#include <stdio.h>
#include <mpi.h>

#define MAESTRO	0
#define	TAG	1
#define	TAREA	3
#define	PARAR	-6
#define SIZE	3

int main(argc,argv)
int argc;
char *argv[];  {

	int numtasks, rank, i, j, k, x, y;
	double startwtime, endwtime;
	double buffer[SIZE] = {0}; //se almacenan 3 valores, "0" resultado, "1" fila, "2" columna
	MPI_Status stat;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if(numtasks>1){
		generarMatrices();
		printf("Soy %d de %d, y empezamos ya\n",rank,numtasks);
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank==MAESTRO){  //TRABAJO DEL MASTER
			startwtime = MPI_Wtime();
			for (i=0; i<M1; i++)
				for (j=0; j<N2; j++){
					MPI_Recv(&buffer,SIZE,MPI_DOUBLE,MPI_ANY_SOURCE,TAG,MPI_COMM_WORLD,&stat);
					matR[(int)buffer[1]][(int)buffer[2]]+=buffer[0];
					buffer[1]=i;buffer[2]=j;
					MPI_Send(&buffer,SIZE,MPI_DOUBLE,stat.MPI_SOURCE,TAREA,MPI_COMM_WORLD);
				}
			i=0;
			while(i<numtasks-1){
				MPI_Recv(&buffer,SIZE,MPI_DOUBLE,MPI_ANY_SOURCE,TAG,MPI_COMM_WORLD,&stat);
                matR[(int)buffer[1]][(int)buffer[2]]+=buffer[0];
                buffer[1]=PARAR;
				MPI_Send(&buffer,SIZE,MPI_DOUBLE,stat.MPI_SOURCE,TAREA,MPI_COMM_WORLD);
				i++;
			}
		} else { //TRABAJO DEL WORKER
			buffer[0] = 0.0;
			int parar=0;
			while(!parar){
				MPI_Send(&buffer,SIZE,MPI_DOUBLE,MAESTRO,TAG,MPI_COMM_WORLD);
				MPI_Recv(&buffer,SIZE,MPI_DOUBLE,MAESTRO,TAREA,MPI_COMM_WORLD,&stat);
				i=(int)buffer[1];
				j=(int)buffer[2];
				if(i<0){
					parar=1;
				} else {		
					buffer[0] = 0.0;
					for(k=0;k<N1;k++)
						buffer[0]+=mat1[i][k]*mat2[k][j];
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
		escribirResultado(startwtime,endwtime,"resulCelda.txt");
	MPI_Finalize();
	return(0);
}
