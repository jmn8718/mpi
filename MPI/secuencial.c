#include "matriz.h"
#include <stdio.h>
#include <mpi.h>

#define MAESTRO	0

int main(argc,argv)
int argc;
char *argv[];  {
	int i,j,k,rank, numtasks;
    double startwtime, endwtime;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if(numtasks==1){
		generarMatrices();
		printf("Soy el proceso %d y voy a empezar.\n",rank);
		startwtime = MPI_Wtime();
		for (i=0; i<M1; i++)
			for (j=0; j<N2; j++)
				for (k=0; k<N1; k++)
		        		matR[i][j] += mat1[i][k] * mat2[k][j];
       		endwtime   = MPI_Wtime();
       		printf("Ha tardado %.4f segundos\n",endwtime-startwtime);
		printf("soy el proceso %d y ya he terminado\n",rank);
		escribirResultado(startwtime,endwtime,"resulSecuencial.txt");
	} else
		if(rank==MAESTRO)
			printf("Solo se necesita 1 proceso.\n",rank);
	MPI_Finalize();
	return(0);
}
