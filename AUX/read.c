#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define MAESTRO	0
#define	MAX	2.345
float **mat1, **mat2, **mat3, **mat4;
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

void writeMatrixToFile(int rows, int cols,float **m, char *file){
    FILE *fd;
	int i,j;

	printf("-------WRITING matrix in '%s'\n",file);
	fd=fopen(file,"w");
		for (i=0; i<rows; i++) 
		    for (j=0; j<cols; j++) 
		            fwrite(&m[i][j], sizeof(float), 1, fd);
	fclose(fd);
	printf("----------------DONE WRITING matrix\n");
}

void readMatrixFromFile (int rows, int cols, float **m, char *file){
	int i;
	MPI_Status status;
	MPI_File fh;
	printf("-------READING matrix from '%s'\n",file);
	MPI_File_open( MPI_COMM_SELF, file, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
	for(i = 0; i < rows; i++)
		MPI_File_read(fh, m[i], cols, MPI_FLOAT, &status);	
	MPI_File_close(&fh);	
	printf("----------------DONE READING matrix\n");
}

int main(int argc,char *argv[]){

	char *nameFileM1 = argv[1];
	char *nameFileM2 = argv[2];
	int M1 = atoi(argv[3]);
	int N1 = atoi(argv[4]);
	int M2 = atoi(argv[5]);
	int N2 = atoi(argv[6]);
	printf("F1: %s F2: %s\n",nameFileM1, nameFileM2);
	printf("M1: %d N1: %d M2: %d N2: %d\n",M1,N1,M2,N2);

	int numtasks, rank, i,j;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	if(rank==MAESTRO){
		printf("MATRIX GENERATION****************************\n");
			mat1=allocMat(M1,N1);
			for(i=0; i<M1; i++)
				for (j=0; j<N1; j++)
				  	mat1[i][j] = ((float)rand()/(float)RAND_MAX)*MAX;
			printf("mat1-------\n");
			//printMatrix(M1,N1,mat1);
			printf("%2.2f %2.2f %2.2f\n",mat1[0][0],mat1[M1/2][N1/2],mat1[M1-1][N1-1]);

			mat2=allocMat(M2,N2);
			for(i=0; i<M2; i++)
				for (j=0; j<N2; j++)
				  	mat2[i][j] = ((float)rand()/(float)RAND_MAX)*MAX;
			printf("mat2-------\n");
			//printMatrix(M2,N2,mat2);
			printf("%2.2f %2.2f %2.2f\n",mat2[0][0],mat2[M2/2][N2/2],mat2[M2-1][N2-1]);
		printf("WRITE PHASE****************************\n");
			writeMatrixToFile(M1,N1,mat1,nameFileM1);
			writeMatrixToFile(M2,N2,mat2,nameFileM2);
			free(mat1);free(mat2);
		printf("READ PHASE****************************\n");
			mat3=allocMat(M1,N1);
			mat4=allocMat(M2,N2);

			readMatrixFromFile(M1,N1,mat3,nameFileM1);
			printf("mat3-------\n");
		//	printMatrix(M1,N1,mat3);
			printf("%2.2f %2.2f %2.2f\n",mat3[0][0],mat3[M1/2][N1/2],mat3[M1-1][N1-1]);

			readMatrixFromFile(M2,N2,mat4,nameFileM2);
			printf("mat4-------\n");
		//	printMatrix(M2,N2,mat4);
			printf("%2.2f %2.2f %2.2f\n",mat4[0][0],mat4[M2/2][N2/2],mat4[M2-1][N2-1]);

			free(mat3);free(mat4);
	} //rank==maestro
	MPI_Finalize();

}
