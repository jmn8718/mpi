#include <stdio.h>
#include <stdlib.h>

#define	MAX	1234.567
//caso 3 600x400 * 400x8.000
#define	M1	600
#define	N1	400

#define	M2	400
#define	N2	8000

double mat1[M1][N1], mat2[M2][N2], matR[M1][N2];

void generarMatrices(){
	int i,j;
	//printf("GENERANDO MATRIZ 1 : %d * %d\n",M1,N1);
        for (i=0; i<M1; i++)
        	for (j=0; j<N1; j++)
                	mat1[i][j] = ((float)rand()/(float)RAND_MAX)*MAX;
//	escribirMatrizFichero(M1,N1,mat1,"M1celda.txt");
        //printf("GENERANDO MATRIZ 2 : %d * %d\n",M2,N2);
        for (i=0; i<M2; i++)
        	for (j=0; j<N2; j++)
        	mat2[i][j] = ((float)rand()/(float)RAND_MAX)*MAX;
//	escribirMatrizFichero(M2,N2,mat2,"M2celda.txt");
        //printf("GENERANDO MATRIZ R : %d * %d\n",M1,N2);
        for (i=0; i<M1; i++)
                for (j=0; j<N2; j++)
                matR[i][j] = 0;

//      printf("MATRICES GENERADAS\n");
}
void escribirResultado(double start, double end, char * name){
	FILE *f;
        f=fopen(name,"a");
        if(f!=NULL)
        	fprintf(f,"---M1=%d*%d M2=%d*%d :\n%f\n",M1,N1,M2,N2,end-start);
	fclose(f);
}
