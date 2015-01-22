#include "matriz.h"
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define MAESTRO	0
#define	TAG	1
#define	TAREA	3
#define	PARAR	-6
#define SIZE	N2

int main(argc,argv)
int argc;
char *argv[];  {

	int numtasks, rank, dest, i, j, x, y, tareas, resto, tarea;
	double startwtime, endwtime;

	MPI_Status stat;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if(numtasks>1){
		
		double matR_temp[M1][N2];
		for(i=0;i<M1;i++) 
			for(j=0;j<N2;j++) 
				matR_temp[i][j]=0;
		long elementos = M1 * N2;
		tareas = elementos / numtasks;
		resto = elementos % numtasks; 
		tarea = rank * tareas;
		if(resto>0)
			if(rank<resto){
				tareas++;
				if(rank!=0)
					tarea++;
			} else
				tarea+=resto;
		generarMatrices();
		printf("Soy %d de %d, y empezamos ya\n",rank,numtasks);
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank==MAESTRO)
			startwtime = MPI_Wtime();
		for(i=0;i<tareas;i++,tarea++){
			x = tarea / N2;
			y = tarea % N2;
			for(j=0;j<N1;j++)
				matR_temp[x][y]+=mat1[x][j]*mat2[j][y];		
		}
		MPI_Reduce(&matR_temp,&matR,elementos,MPI_DOUBLE,MPI_SUM,MAESTRO,MPI_COMM_WORLD);

		if(rank==MAESTRO){
			endwtime = MPI_Wtime();
			printf("Ha tardado %.4f segundos\n",endwtime-startwtime);
		}
		printf("Soy %d de %d, y ya hemos terminado\n",rank,numtasks);
	} else
		printf("Debe de indicar al menos 2 procesos\n");
	if(rank==MAESTRO)
		escribirResultado(startwtime,endwtime,"resulEstatico.txt");
	MPI_Finalize();
}
