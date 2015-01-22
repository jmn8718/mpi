#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define	MAX	37.56
#define MAESTRO	0

float **mat1, **mat2, **matR, **matSum;

float **allocMat (int rows, int cols){
	int i,j;
    float  *data   = (float  *) malloc (rows * cols * sizeof(float));
    float **matrix = (float **) malloc (rows * sizeof(float *));
    for (i = 0; i < rows; i++)
        matrix[i] = & (data[i * cols]);
	for(i=0; i<rows; i++)
	  	for (j=0; j<cols; j++)
	   		matrix[i][j] = 0;
    return matrix;
}

void printMatrix (int rows, int cols,float **m){
	int i, j;
    for (i = 0; i < rows; i++) {
        printf ("%6d: ", i);
        for (j = 0; j < cols; j++)
            printf ("%3.3f ", m[i][j]);
        printf ("\n");
    }
	printf("----------------------\n");
}

int main(int argc,char *argv[]){
	int numtasks, rank;	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if(numtasks>1){
		char *nameFileM1 = argv[1];
		char *nameFileM2 = argv[2];
		int M1 = atoi(argv[3]);
		int N1 = atoi(argv[4]);
		int M2 = atoi(argv[5]);
		int N2 = atoi(argv[6]);
		if((M1 % numtasks) == 0) {
			if(rank==MAESTRO)
				printf("F1: %s M1: %d N1: %d\nF2: %s M2: %d N2: %d\n",nameFileM1,M1,N1,nameFileM2,M2,N2);
			int i, j, x, y;
			double startwtime, endwtime;

			MPI_Status status;
			MPI_Request request;
		
			mat1=allocMat(M1,N1);
			mat2=allocMat(M2,N2);
			for(i=0; i<M1; i++) 
				for (j=0; j<N1; j++) 
				  	mat1[i][j] = ((float)rand()/(float)RAND_MAX)*MAX;
			if(rank==MAESTRO) printMatrix(M1,N1,mat1);
			for(i=0; i<M2; i++)
				for (j=0; j<N2; j++)
					mat2[i][j] = ((float)rand()/(float)RAND_MAX)*MAX;
			printf("MATRICES GENERADAS %d\n",rank);
		//se reparten las tareas a realizar
			int misFilas = M1 / numtasks;
			int miInicio = rank * misFilas;
			//se empieza la ejecución
	MPI_Barrier(MPI_COMM_WORLD);
			printf("Soy %d de %d, hago %d de %d desde %d y empezamos ya\n",rank,numtasks,misFilas,M1,miInicio);
	MPI_Barrier(MPI_COMM_WORLD);
			if(rank==MAESTRO)
				startwtime = MPI_Wtime();

			matR=allocMat(misFilas,N2);
			for(i=0; i<misFilas; i++)
				  	for (j=0; j<N2; j++)
						matR[i][j]=0;
			for(i=miInicio,x=0;i<miInicio+misFilas;i++,x++)
				for(y=0;y<N2;y++)
					for(j=0;j<N1;j++)
						matR[x][y]+=mat1[i][j]*mat2[j][y];
			free(mat1);
			free(mat2);
			if(rank==MAESTRO){
				matSum=allocMat(M1,N2);
				for(i=0; i<misFilas; i++)
				  	for (j=0; j<N2; j++)
						matSum[i][j]=matR[i][j];
			}
	MPI_Barrier(MPI_COMM_WORLD);
			if(rank==MAESTRO){
				for(x=0;x<(numtasks-1)*misFilas;x++){
					MPI_Recv(matR[0],N2,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
					y = status.MPI_TAG; //el inicio del proceso que manda
					//printf("recibo de %d en x=%d para fila %d\n",status.MPI_SOURCE,x,y);
				  	for (j=0; j<N2; j++)
						matSum[y][j]=matR[0][j];
				}						
			} else { //otro proceso
				for(i=0;i<misFilas;i++)	{		
					MPI_Isend(matR[i],N2,MPI_FLOAT,MAESTRO,miInicio+i,MPI_COMM_WORLD,&request);
					MPI_Wait(&request,&status);
				}
			}
	MPI_Barrier(MPI_COMM_WORLD);
			if(rank==MAESTRO){
				endwtime = MPI_Wtime();
				printf("MATRICES FIN S 0\n");
				for(i=0; i<M1; i++){
				  	for (j=0; j<N2; j++)
						printf("%5.3f ",matSum[i][j]);
					printf("\n");
				}
				printf("Ha tardado %.4f segundos\n",endwtime-startwtime);
			}
		
	//		MPI_Barrier(MPI_COMM_WORLD);
			free(matR);
			if(rank==MAESTRO)
				free(matSum);
		} else
			if(rank==MAESTRO)			
				printf("El numero de procesos debe ser multiplo de M1\n");
	} else
		printf("El numero de procesos debe ser mayor que 1\n");
	MPI_Finalize();

}
