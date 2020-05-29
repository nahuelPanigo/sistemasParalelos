#include<stdio.h>
#include<stdlib.h>
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

 double *A,*B,*C,*D,*AB,d,avgA,avgB,avgC,maxA,maxB,maxC,minA,minB,minC,aux;
 int i,j,k,disp1,disp2,disp3;
 int check = 1;
 double timetick;
 int N=100;

 avgA=0;
 avgB=0;
 avgC=0;
 maxA=MINIMOVALOR;
 maxB=MINIMOVALOR;
 maxC=MINIMOVALOR;
 minA=MAXIMOVALOR;
 minB=MAXIMOVALOR;
 minC=MAXIMOVALOR;

 //Controla los argumentos al programa
 if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }
 
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

 timetick = dwalltime();

 //Realiza la multiplicacion 
	
 for(i=0;i<N;i++){
	disp1 = i*N;
  for(j=0;j<N;j++){
    disp2 = disp1 + j;	  
    if(maxB < B[disp2])maxB=B[disp2];
    if(maxA < A[disp2])maxA=A[disp2];
    if(minA > A[disp2])minA=A[disp2];
    if(minB > B[disp2])minB=B[disp2];
    avgA+=A[disp2];
    avgB+=B[disp2];
    aux = 0;
    disp3 = j*N;
   for(k=0;k<N;k++){
    aux+=A[disp1+k]*B[k+disp3];
   }
   AB[disp2] = aux;
  }
 }

  for(i=0;i<N;i++){
    disp1 = i*N;
    for(j=0;j<N;j++){
      disp2 = disp1 + j;	    
      if(maxC < C[disp2])maxC=C[disp2];
      if(minC > C[disp2])minC=C[disp2];
      avgC+=C[disp2];
      aux = 0;
      disp3 = j*N;
      for(k=0;k<N;k++){
    	aux+= AB[disp1+k]*C[k+disp3];
      }
      D[disp2]= aux;
      }
 }
  avgA/=(N*N);
  avgB/=(N*N);
  avgC/=(N*N);
  d=(maxA*maxB*maxC - minA*minC*minB)/(avgA*avgB*avgC);

 for(i=0;i<N;i++){
  disp1 = i*N;	 
  for(j=0;j<N;j++){
   D[disp1+j]= D[disp1+j] * d;
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
