#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define CANT_THREADS 4

//globales
		double *A;
		double *B;
		double *C;
		int N;


//funcion que va a ejecutar los hilos
void * mult_matr(void *ident){
	int *p =(int *) ident;
	int id= *p;
	int desde,hasta;
	desde=id*N/CANT_THREADS;
	hasta=(id+1)*N/CANT_THREADS;
	for (int i = 0; i < N ; i++){
		for (int j = desde; j < hasta; j++){
			C[i*N+j]=0;
			for (int k = 0; k < N; k++){
				C[i*N+j]+=A[i*N+k]*B[j*N+k];
			}
		}
	}
}

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}


int main(int argc,char* argv[]){
	double timetick;

	int id[CANT_THREADS];
	int i,j,k;
	int check=1;
	pthread_attr_t attr;
	pthread_t threads[CANT_THREADS];

	//checkeamos cantidad de argumentos del programa
	if(argc<2){
		printf("\n faltan argumentos ingrese %s junto con un numero de matriz\n",argv[0]);
		exit (1);
	}

	//checkeamos que n sea mayor a 0
	if((N=atoi(argv[1])) <=  0){
		printf("\nno se puede realizar la matriz con %sdebe ser positivo mayor a 0 \n",argv[1]);
		exit (2);
	}


	A=(double*)malloc(sizeof(double)*N*N);
	B=(double*)malloc(sizeof(double)*N*N);
	C=(double*)malloc(sizeof(double)*N*N);

	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			//A la guardamos por columnas
			A[j*N+i]=1;
			//B la guardamos por fila
			B[i*N+j]=1;
		}
	}

	pthread_attr_init(&attr);

	timetick = dwalltime();
	for ( i = 0; i < CANT_THREADS; i++)	{
		id[i]=i;
		pthread_create(&threads[i], &attr,mult_matr,&id[i]);
	}


	for (i = 0; i < CANT_THREADS; i++){
	pthread_join(threads[i],NULL);
	}

	printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n",N,N, dwalltime() - timetick);

	for(i=0;i<N;i++){
		for (j = 0; j < N; j++)
		{
			//printf("[%f]\n",C[i*N+j] );
			check=(C[i*N+j]==N) && check;
		}
	}

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }

	free(A);
	free(B);
	free(C);
	return 0;


}	