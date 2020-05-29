#include<stdio.h>
#include<stdlib.h>

int N=100;

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}



int main(int argc,char* argv[]){

 double *A,*B,*C,*D,*AB;
 int i,j,k;
 int check = 1;
 double timetick;

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
 //La matriz A se inicializa todas las columnas en 0 menos la ultima
 //La matriz BT se inicializa triangular inferior con 1
 //Esto permite que el resultado sean todos 1
 //La matriz C se inicializa en 0
 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
      A[i*N+j]=1;
      B[j*N+i]=1;
      C[j*N+i]=1;
 }
}
 //Realiza la multiplicacion 
 timetick = dwalltime();

 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
    AB[i*N+j]=0;
   for(k=0;k<N;k++){
    AB[i*N+j]=AB[i*N+j] + A[i*N+k]*B[k+j*N];
   }
  }
 }

 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
    D[i*N+j]=0;
   for(k=0;k<N;k++){
    D[i*N+j]=D[i*N+j] + AB[i*N+j]*C[k+j*N];
   }
  }
 }


 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 //Verifica el resultado
 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
   check = check && (D[i*N+j]== N*N);
  }
 }

 if(check){
  printf("Multiplicacion de matriz correcta\n");
 }else{
  printf("Multiplicacion de matriz erroneo\n");
 }
 
 free(A);
 free(AB);
 free(B);
 free(C);
 free(D);

 return(0);
}
