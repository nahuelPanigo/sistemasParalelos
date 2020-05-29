
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <float.h>

#define MINIMOVALOR -DBL_MAX
#define MAXIMOVALOR DBL_MAX
#define COORDINATOR 0

int main(int argc, char* argv[]){
	int i, j, k, numProcs, rank, N, stripSize, check=1,disp1,disp2,disp3,disp4,despla,err;
	double * A, * B, *C, *D,*AB,aux,d;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime, totalTime;
    double matA[3],matB[3],matC[3],maxA,minA,maxB,minB,maxC,minC,avgA,avgB,avgC;

       //inicializamos los maximos con un valor mu bajo
       matA[0]=MINIMOVALOR;matB[0]=MINIMOVALOR;matC[0]=MINIMOVALOR;
       //inicializamos los minimos con un valor muy alto
       matA[1]=MAXIMOVALOR;matB[1]=MAXIMOVALOR;matC[1]=MAXIMOVALOR;
       //inicializamos la suma con 0
       matA[2]=0;matB[2]=0;matC[2]=0;

       minA=MAXIMOVALOR;minB=MAXIMOVALOR;minC=MAXIMOVALOR;
       maxA=MINIMOVALOR;maxB=MINIMOVALOR;maxC=MINIMOVALOR;
       avgA=0;avgB=0;avgC=0;

	/* Lee parámetros de la línea de comando */
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
	    printf("\nUsar: %s size \n  size: Dimension de la matriz y el vector\n", argv[0]);
		exit(1);
	}
  

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if (N % numProcs != 0) {
		printf("El tamaño de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

	// calcular porcion de cada worker
	stripSize = N / numProcs;

	// Reservar memoria
	if (rank == COORDINATOR) {
		A = (double*) malloc(sizeof(double)*N*N);
		D = (double*) malloc(sizeof(double)*N*N);
        AB =(double*) malloc(sizeof(double)*N*N);
	}else{
		A = (double*) malloc(sizeof(double)*N*stripSize);
		D =  (double*) malloc(sizeof(double)*N*stripSize);
	    AB =(double*) malloc(sizeof(double)*N*stripSize);
      }
	B = (double*) malloc(sizeof(double)*N*N);
	C = (double*) malloc(sizeof(double)*N*N);

	// inicializar datos
	if (rank == COORDINATOR) {
		for (i=0; i<N ; i++)
			for (j=0; j<N ; j++){
				A[i*N+j] =i+j;
				B[j*N+i] =i+j;
				C[j*N+i] =i+j;
			}
           /*double num;
           for (i=0; i<N ; i++){
                for (j=0; j<N ; j++){
                    scanf("%lf",&num);
                    A[i*N+j] = num;
                    B[j*N+i] = num;
                    C[j*N+i] = num;
                 }
		   }*/
      }
      
	MPI_Barrier(MPI_COMM_WORLD); //Los procesos esperan a que el COORDINATOR inicialice las matrices

	commTimes[0] = MPI_Wtime();
    /* Con el scatter el COORDINATOR reparte una porcion de la matriz A a cada proceso (incluyendo al COORDINATOR)*/
    err=MPI_Scatter(A,stripSize*N,MPI_DOUBLE,A,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    /* con el Bcast el COORDINATOR envia el mismo mensaje (en este caso la matriz B) a todos los procesos*/
    err=MPI_Bcast(B,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
	commTimes[1] = MPI_Wtime();

	/* multiplicacion de A * B y calculo de minimos, maximos y suma de valores de A y B */
	for(i=0;i<stripSize;i++){
	      disp1 = i*N;
	      despla = (stripSize * rank * N);
            for(j=0;j<N;j++){
	              disp2 = disp1 + j;	  
		      disp4 = despla + disp2;
	              if(matB[0] < B[disp4])matB[0]=B[disp4];
	              if(matA[0] < A[disp2])matA[0]=A[disp2];
	              if(matA[1] > A[disp2])matA[1]=A[disp2];
	              if(matB[1] > B[disp4])matB[1]=B[disp4];
	              matA[2]+=A[disp2];
	              matB[2]+=B[disp4];
	              aux = 0;
	              disp3 = j*N;
	              for(k=0;k<N;k++){
	                  aux+=A[disp1+k]*B[k+disp3];
	              }
	              AB[disp2] = aux;
            } 
    }
    /* Con este Bcast el COORDINADOR le envia la matrix C a todos los procesos del mismo comunicador*/
   err=MPI_Bcast(C,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    /*Con este Scatter el COORDINATOR envia partes iguales la matriz D a todos los proceoss incluyendo al COORDINATOR */
   err=MPI_Scatter (D,stripSize*N,MPI_DOUBLE,D,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
   for(i=0;i<stripSize;i++){
    disp1 = i*N;
    despla = (stripSize * rank * N);
    for(j=0;j<N;j++){
      disp2 = disp1 + j;
      disp4 = disp2 + despla;	      
      if(matC[0] < C[disp4])matC[0]=C[disp4];
      if(matC[1] > C[disp4])matC[1]=C[disp4];
      matC[2]+=C[disp4];
      aux = 0;
      disp3 = j*N;
      for(k=0;k<N;k++){
    	aux+= AB[disp1+k]*C[k+disp3];
      }
      D[disp2]= aux;
    }
 }
 //en esta parte se actualizan los minimos en las variables locales del cordinador y la suma para hacer el avg luego
err=MPI_Reduce(&matA[0], &maxA,1, MPI_DOUBLE, MPI_MAX,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matB[0], &maxB,1, MPI_DOUBLE, MPI_MAX,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matC[0], &maxC,1, MPI_DOUBLE, MPI_MAX,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matA[1], &minA,1, MPI_DOUBLE, MPI_MIN,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matB[1], &minB,1, MPI_DOUBLE, MPI_MIN,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matC[1], &minC,1, MPI_DOUBLE, MPI_MIN,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matA[2], &avgA,1, MPI_DOUBLE, MPI_SUM,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matB[2], &avgB,1, MPI_DOUBLE, MPI_SUM,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&matC[2], &avgC,1, MPI_DOUBLE, MPI_SUM,COORDINATOR, MPI_COMM_WORLD);

	commTimes[2] = MPI_Wtime();

//el coordinador calcula el resultado de la funcion d
if(rank == COORDINATOR){
  avgA/=(N*N);
  avgB/=(N*N);
  avgC/=(N*N);
  d=(maxA*maxB*maxC - minA*minC*minB)/(avgA*avgB*avgC);
 }

//el coordinador envia el resultado de la funcion d a los otros procesos, hay barrera implicita   
MPI_Bcast(&d,1,MPI_DOUBLE,COORDINATOR, MPI_COMM_WORLD); 
 
//cada proceso calcula su parte de la matriz d    
 for(i=0;i<stripSize;i++){
    disp1 = i*N;	 
    for(j=0;j<N;j++){
      D[disp1+j]= D[disp1+j] * d;
    }
 }

    //se juntan los resultados de todas las partez de matriz d en el coordinador
    MPI_Gather(D,N*stripSize,MPI_DOUBLE, D, N*stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    //se juntan los tiempos  en el coordinador de todos los procesos
    commTimes[3] = MPI_Wtime();
    MPI_Reduce(commTimes, minCommTimes, 4, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(commTimes, maxCommTimes, 4, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);


	MPI_Finalize();

	if (rank==COORDINATOR) {

		// print results
        /*for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				printf("[%lf]",D[i*N+j]);
              }
              printf("\n");
            }*/
		
		totalTime = maxCommTimes[3] - minCommTimes[0];
		commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]);		
		printf("Multiplicacion de matrices (N=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n",N,totalTime,commTime);
	}
	
	free(A);
	free(B);
	free(C);
    free(AB);
	free(D);
	return 0;
}
