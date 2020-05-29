#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <float.h>

#define MINIMOVALOR -DBL_MAX
#define MAXIMOVALOR DBL_MAX
#define COORDINATOR 0
#define THREAD_REQUIRED 2

int main(int argc, char* argv[]){
	int i, j, k, numProcs, rank, N, stripSize, check=1,disp1,disp2,disp3,disp4,despla,err,cantThreads;
	double * A, * B, *C, *D,*AB,aux,d;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime, totalTime;
    double matA[3],matB[3],matC[3],maxA,minA,maxB,minB,maxC,minC,avgA,avgB,avgC;
    double MAXA,MAXB,MAXC, MINA, MINB, MINC, AVGA, AVGB, AVGC;
       //inicializamos los maximos con un valor mu bajo
       matA[0]=MINIMOVALOR;matB[0]=MINIMOVALOR;matC[0]=MINIMOVALOR;
       //inicializamos los minimos con un valor muy alto
       matA[1]=MAXIMOVALOR;matB[1]=MAXIMOVALOR;matC[1]=MAXIMOVALOR;
       //inicializamos la suma con 0
       matA[2]=0;matB[2]=0;matC[2]=0;

       minA=MAXIMOVALOR;minB=MAXIMOVALOR;minC=MAXIMOVALOR;
       maxA=MINIMOVALOR;maxB=MINIMOVALOR;maxC=MINIMOVALOR;
       avgA=0;avgB=0;avgC=0;
    
       MINA =MAXIMOVALOR;MINB=MAXIMOVALOR;MINC=MAXIMOVALOR;
       MAXA=MINIMOVALOR;MAXB=MINIMOVALOR;MAXC=MINIMOVALOR;
       AVGA = 0; AVGB=0; AVGC=0; 

	/* Lee parÃ¡metros de la lÃ­nea de comando */
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0)) {
	    printf("\nUsar: %s size \n  size: Dimension de la matriz y el vector  cantThreads: cantidad de\n", argv[0]);
		exit(1);
	}
  

	MPI_Init_thread(&argc,&argv,THREAD_REQUIRED,&cantThreads);
	MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if (N % numProcs != 0) {
		printf("El tamaÃ±o de la matriz debe ser multiplo del numero de procesos.\n");
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
    /* Con este Bcast el COORDINADOR le envia la matrix C a todos los procesos del mismo comunicador*/
   err=MPI_Bcast(C,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    /*Con este Scatter el COORDINATOR envia partes iguales la matriz D a todos los proceoss incluyendo al COORDINATOR */
   err=MPI_Scatter (D,stripSize*N,MPI_DOUBLE,D,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    
    
  #pragma omp parallel default(none)private(i,j,k,matA,matB,matC,aux,disp1,disp2,disp3,disp4) shared(A,B,C,D,AB,N,d,maxA,maxB,maxC,minA,minB,minC,avgA,avgB,avgC,stripSize,rank)
    {
    
	/* multiplicacion de A * B y calculo de minimos, maximos y suma de valores de A y B */
	 #pragma omp for schedule (static)
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
       #pragma omp for schedule(static)
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
        
        
       #pragma omp critical_matA
{
 //actualizamos los minimos maximos y avg de matA
    if(matA[0] > maxA)maxA=matA[0];
    if(matA[1] < minA)minA=matA[1];
    avgA+=matA[2];
/*
}
#pragma omp critical_matB
{
*/
     //actualizamos los minimos maximos y avg de matA
    if(matB[1] < minB)minB=matB[1];
    if(matB[0] > maxB)maxB=matB[0];
    avgB+=matB[2];
/*    
}
#pragma omp critical_matC
{
*/
    if(matC[0] > maxC)maxC=matC[0];
    if(matC[1] < minC)minC=matC[1];
    avgC+=matC[2];
}
 
    
 //end omp section   
}   
    
 //en esta parte se actualizan los minimos en las variables locales del cordinador y la suma para hacer el avg luego
err=MPI_Reduce(&maxA,&MAXA,1, MPI_DOUBLE, MPI_MAX,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&maxB,&MAXB,1, MPI_DOUBLE, MPI_MAX,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&maxC,&MAXC,1, MPI_DOUBLE, MPI_MAX,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&minA,&MINA,1, MPI_DOUBLE, MPI_MIN,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&minB,&MINB,1, MPI_DOUBLE, MPI_MIN,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&minC,&MINC,1, MPI_DOUBLE, MPI_MIN,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&avgA,&AVGA,1, MPI_DOUBLE, MPI_SUM,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&avgB,&AVGB,1, MPI_DOUBLE, MPI_SUM,COORDINATOR, MPI_COMM_WORLD);
err=MPI_Reduce(&avgC,&AVGC,1, MPI_DOUBLE, MPI_SUM,COORDINATOR, MPI_COMM_WORLD);

	commTimes[2] = MPI_Wtime();

//el coordinador calcula el resultado de la funcion d
if(rank == COORDINATOR){
  AVGA/=(N*N);
  AVGB/=(N*N);
  AVGC/=(N*N);
  d=(MAXA*MAXB*MAXC - MINA*MINC*MINB)/(AVGA*AVGB*AVGC);
  printf("Maximos = %lf, %lf, %lf\n ", MAXA,MAXB,MAXC);
  printf("Minimos = %lf, %lf, %lf\n", MINA,MINB,MINC);
  printf("Promedios = %lf, %lf, %lf\n", AVGA, AVGB, AVGC);
  printf("ecuacion = %lf\n", d);
 }

//el coordinador envia el resultado de la funcion d a los otros procesos, hay barrera implicita   
MPI_Bcast(&d,1,MPI_DOUBLE,COORDINATOR, MPI_COMM_WORLD); 
 
//cada proceso calcula su parte de la matriz d  
#pragma omp paralell default(none)private(i,j,k,disp1)shared(D,d,N,stripSize)
    {
     #pragma omp for schedule(static)
     for(i=0;i<stripSize;i++){
        disp1 = i*N;	 
        for(j=0;j<N;j++){
          D[disp1+j]= D[disp1+j] * d;
        }
     }
//termina la seccion omp
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

