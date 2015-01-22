#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define MAESTRO	0
#define	MAX	37.56

float **mat2, **matR, **matSum;
float **allocMat (int rows, int cols){
	int i;
    float **matrix = (float **) malloc (rows * sizeof(float *));
    for (i = 0; i < rows; i++)
        matrix[i] = (float *)malloc(rows * cols * sizeof(float) );;
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

void readMatrixFromFile (int rows, int cols, float **m, char *file){
	int i;
	MPI_Status status;
	MPI_File fh;
	MPI_File_open( MPI_COMM_WORLD, file, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
	for(i = 0; i < rows; i++)
		MPI_File_read(fh, m[i], cols, MPI_FLOAT, &status);	
	MPI_File_close(&fh);	
}

void writeTime(double start, double end,double read, char * name,int M1,int N1, int M2, int N2, int np){
	FILE *f;
        f=fopen(name,"a");
        if(f!=NULL)
        	fprintf(f,"---M1=%d*%d M2=%d*%d np=%d : TOTAL->%5.6f READ->%5.6f CALC->%5.6f\n",M1,N1,M2,N2,np,end-start,read-start,end-read);
	fclose(f);
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
		if((N1 == M2) && ((M1 % numtasks) == 0)) {
			if(rank==MAESTRO)
				printf("F1: %s M1: %d N1: %d\nF2: %s M2: %d N2: %d\n",nameFileM1,M1,N1,nameFileM2,M2,N2);
//se definen las variables locales del metodo a utilizar
			int i, j, x, y, sizeOfFloat, inicio;
			double startwtime,readwtime, endwtime;
			MPI_Datatype mtype;
			MPI_Status status;
			MPI_File fh;
			MPI_Request request;

//se reparten las tareas a realizar
			int misFilas = M1 / numtasks;
			int miInicio = rank * misFilas;
		MPI_Barrier(MPI_COMM_WORLD);
			printf("Soy %d de %d, hago %d de %d desde %d y empezamos ya\n",rank,numtasks,misFilas,M1,miInicio);
//se reserva espacio y se crean las matrices
			mat2=allocMat(M2,N2);
			matR=allocMat(misFilas,N2);
			for(i=0; i<misFilas; i++)
				  	for (j=0; j<N2; j++)
						matR[i][j]=0;
			if(rank==MAESTRO)
				matSum=allocMat(M1,N2);
//se empieza
		MPI_Barrier(MPI_COMM_WORLD);
			if(rank==MAESTRO)
				startwtime = MPI_Wtime();
//fase de lectura de las matrices
//			printf("READ PHASE****************************\n");
//leemos 1º matriz parcial, segun toque
			MPI_Type_size(MPI_FLOAT,&sizeOfFloat);
			MPI_Type_vector(misFilas,N1,N1,MPI_FLOAT,&mtype);
			MPI_Type_commit(&mtype);
			float mat1[misFilas][N1];
			MPI_File_open( MPI_COMM_WORLD, nameFileM1, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
			inicio=rank*misFilas*sizeOfFloat*N1;
			MPI_File_set_view(fh,inicio, MPI_FLOAT,mtype, "native", MPI_INFO_NULL);
			MPI_File_read_all(fh, mat1, 1, mtype, &status);	
			MPI_File_close(&fh);
			MPI_Type_free(&mtype);
//leemos 2º matriz
			MPI_Barrier(MPI_COMM_WORLD);
			readMatrixFromFile(M2,N2,mat2,nameFileM2);
//			printf("READ PHASE DONE****************************\n");	
		MPI_Barrier(MPI_COMM_WORLD);
			if(rank==MAESTRO)
				readwtime = MPI_Wtime();		
//calculamos la matriz resultado		
			for(i=miInicio,x=0;i<miInicio+misFilas;i++,x++)
				for(y=0;y<N2;y++)
					for(j=0;j<N1;j++)
						matR[x][y]+=mat1[x][j]*mat2[j][y];	//varia contador
			free(mat2);
//el maestro añade sus resultados a la matriz suma
			if(rank==MAESTRO)
				for(i=0; i<misFilas; i++)
				  	for (j=0; j<N2; j++)
						matSum[i][j]=matR[i][j];
//comienza el envio y recoleccion de los datos 
		MPI_Barrier(MPI_COMM_WORLD);
			if(rank==MAESTRO){
				for(x=0;x<(numtasks-1)*misFilas;x++){
					MPI_Recv(matR[0],N2,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
					y = status.MPI_TAG; //el inicio del proceso que manda
				  	for (j=0; j<N2; j++)
						matSum[y][j]=matR[0][j];
				}						
			} else { //otro proceso
				for(i=0;i<misFilas;i++)	{		
					MPI_Isend(matR[i],N2,MPI_FLOAT,MAESTRO,miInicio+i,MPI_COMM_WORLD,&request);
					MPI_Wait(&request,&status);
				}
			}
//finalizada la recoleccion el maestro muestra la matrizSuma
		MPI_Barrier(MPI_COMM_WORLD);
			if(rank==MAESTRO){
				endwtime = MPI_Wtime();
				//printf("MATRIZ PRODUCTO\n");
				//printMatrix(M1,N2,matSum);
				printf("Ha tardado %.4f segundos\n",endwtime-startwtime);
				writeTime(startwtime,endwtime,readwtime,"resulEstaticoV2.txt",M1,N1,M2,N2,numtasks);
			}
//liberamos las matrices que tenemos con datos
			free(matR);			
			if(rank==MAESTRO)
				free(matSum);
		} else //si procesos no multiplo de M1
			if(rank==MAESTRO)			
				printf("El numero de procesos debe ser multiplo de M1\n");
	} else //si procesos <2
		printf("El numero de procesos debe ser mayor que 1\n");
	MPI_Finalize();

}
