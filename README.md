# mpi
MPI programs that calculate the multiplication of 2 matrices
For standar MPI, there are 4 programs that calculate the multiplication of 2 matrices
  -secuencial.c It's a secuential program for the multiplication
  -celda.c It's a program that calculate the result of each entry i,j of the matrix in each proccess
  -fila.c It's a program that calculate the result of all entries of a row from the final matrix in each proccess.
  -secuencial.c It's a program that calculate an equal number of entries in each proccess.
  -secuenacil_v2.c Improved version od secuencial.c
For MPI-IO, there are 2 static programs
  -mpiio_v1.c It's a program that read the matrices from a file in each proccess
  -mpiio_v2.c It's a program that only read it's part of the data matrix in each proccess

The program were tested in matrices of:
  -60x80.000 * 80.000x60
  -600x8.000 * 8.000x600
  -600x400 * 400x8.000
  -6.000x100 * 100x6.000

In the AUX folder you have the file "read.c" that generate a random matrix
