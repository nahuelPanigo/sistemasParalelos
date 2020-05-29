//Ejercicio 1
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>



//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc,char*argv[]){
 int check = 1; 
double *A;
 int i,j;
 int numThreads = atoi(argv[2]);
 int N=atoi(argv[1]);
 omp_set_num_threads(numThreads);

 //Aloca memoria para la matriz
  A=(double*)malloc(sizeof(double)*N*N);
 
 //Inicializa la matriz. Cada posicion debe quedar con el valor I*J
 // I => fila J=> columna. 

double timetick = dwalltime();
 #pragma omp parallel for default(shared) private(j,i)
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
		A[i*N+j]=i*j;
   }
  }   

printf("la busqueda del vector de %dposiciones. Tiempo en segundos %f\n",N ,dwalltime() - timetick);

 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(A[i*N+j]==i*j);
   }
  }   

  if(check){
   printf("Resultado correcto\n");
  }else{
   printf("Resultado erroneo \n");
  }

 free(A);

 return(0);
}

