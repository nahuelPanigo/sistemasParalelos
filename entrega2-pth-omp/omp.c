#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <float.h>
#define MINIMOVALOR -DBL_MAX
#define MAXIMOVALOR  DBL_MAX 


//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}



int main(int argc,char* argv[]){

 double *A,*B,*C,*D,*AB,d,avgA,avgB,avgC,maxA,maxB,maxC,minA,minB,minC;
 int i,j,k;
 int check = 1;
 double timetick;
 int N=100;
 int disp1,disp2,disp3;
//en la posicion 1 se encuentra el maximo, en la posicion 2 el minimo y en la posicion 3 la suma de elementos
 double matA[3]; 
 double matB[3];
 double matC[3];
 int numThreads;


 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((numThreads = atoi(argv[2])) <= 0) ) {
    printf("la cantidad de parametros ingresados no es correcata o no estan bien debe ser nombre del programa N(representara a la matriz NxN) y cant threads\n");
    exit(1);
  }

 //inicializamos los maximos con un valor mu bajo
 matA[0]=MINIMOVALOR;matB[0]=MINIMOVALOR;matC[0]=MINIMOVALOR;
 //inicializamos los minimos con un valor muy alto
 matA[1]=MAXIMOVALOR;matB[1]=MAXIMOVALOR;matC[1]=MAXIMOVALOR;
 //inicializamos la suma con 0
 matA[2]=0;matB[2]=0;matC[2]=0;

 avgA=0;
 avgB=0;
 avgC=0;
 maxA=MINIMOVALOR;
 maxB=MINIMOVALOR;
 maxC=MINIMOVALOR;
 minA=MAXIMOVALOR;
 minB=MAXIMOVALOR;
 minC=MAXIMOVALOR;

 
 //Aloca memoria para las matrices
 A=(double*)malloc(sizeof(double)*N*N);
 B=(double*)malloc(sizeof(double)*N*N);
 C=(double*)malloc(sizeof(double)*N*N);
 D=(double*)malloc(sizeof(double)*N*N);
 AB=(double*)malloc(sizeof(double)*N*N);


 //Inicializa las matrices
 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
      A[i*N+j]=i+j;
      B[j*N+i]=i+j;
      C[j*N+i]=i+j;
 }
}
 
 //Realiza la multiplicacion 
 timetick = dwalltime();
  omp_set_num_threads(numThreads); 

double aux;
#pragma omp parallel default(none)private(i,j,k,matA,matB,matC,aux,disp1,disp2,disp3) shared(A,B,C,D,AB,N,d,maxA,maxB,maxC,minA,minB,minC,avgA,avgB,avgC)
 {

#pragma omp for schedule(static)
 for(i=0;i<N;i++){
  disp1=i*N;
  for(j=0;j<N;j++){
    disp2 = disp1 + j;
    //comparamos los maximos y minimos y los guardamos en los vectores que tiene cada thread
    if(matA[0] < A[disp2])matA[0]=A[disp2];
    if(matB[0] < B[disp2])matB[0]=B[disp2];
    if(matA[1] > A[disp2])matA[1]=A[disp2];
    if(matB[1] > B[disp2])matB[1]=B[disp2];
    //guardamos la suma de elementos que tiene cada matriz por cada thread
    matA[2]+=A[disp2];
    matB[2]+=B[disp2];
    aux=0;
    disp3 = j*N;
    for(k=0;k<N;k++){
    aux+= A[disp1+k]*B[k+disp3];
   }
   AB[disp2]=aux;
  }
}


#pragma omp for schedule(static)
 for(i=0;i<N;i++){
    disp1 = i*N;
    for(j=0;j<N;j++){
      disp2 = disp1 + j;      
      matC[2]+=C[disp2];
      if(matC[0] < C[disp2])matC[0]=C[disp2];
      if(matC[1] > C[disp2])matC[1]=C[disp2];
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

 #pragma omp barrier


 #pragma omp single
{
//en esta seccion un unico hilo se encargara de calcular el resultado de la expresion 
  avgA=avgA/(N*N);
  avgB=avgB/(N*N);
  avgC=avgC/(N*N);
  d=(maxA*maxB*maxC - minA*minC*minB)/(avgA*avgB*avgC);
}

#pragma omp for schedule(static)
 for(i=0;i<N;i++){
  disp1 = i*N;   
  for(j=0;j<N;j++){
     D[disp1+j]= D[disp1+j] * d;
  }
 }

}
 printf("Tiempo en segundos %f\n", dwalltime() - timetick);


 free(A);
 free(AB);
 free(B);
 free(C);
 free(D);

 return(0);
}
