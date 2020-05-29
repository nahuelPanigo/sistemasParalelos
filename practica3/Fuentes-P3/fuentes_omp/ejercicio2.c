//Ejercicio 2
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<math.h>


//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc,char*argv[]){
 double x,scale,*v;
 int i;
 int numThreads = atoi(argv[2]);
 int N=atoi(argv[1]);
 omp_set_num_threads(numThreads);
 scale=2.78;
 x=0.0;
 v=(double*)malloc(sizeof(double)*N);


double timetick = dwalltime();


 /*#pragma omp for nowait
 for(i=1;i<=N;i++){
 	v[i]=sqrt(i*scale);
 }*/
 #pragma omp parallel for ordered
 for(i=1;i<=N;i++){
 	#pragma omp ordered
 	{x+= sqrt(i*scale) + 2*x;}
 }

printf("la busqueda del vector de %dposiciones. Tiempo en segundos %f\n",N ,dwalltime() - timetick);

printf("\n Resultado: %f \n",x);

 return(0);
}

