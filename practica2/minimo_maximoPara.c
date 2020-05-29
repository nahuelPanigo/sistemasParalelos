#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#define CANT_THREADS 2

//globales
		int *vector;
		int N;
		int minimo=99999999;
		int maximo=-9999999;
		int num;
		sem_t sem;


//funcion que va a ejecutar los hilos
void * mult_matr(void *ident){
	int *p =(int *) ident;
	int id= *p;
	long int desde,hasta;
	int min,max;
	min=99999999;
	max=-9999999;
	desde=id*(N/CANT_THREADS);
	hasta=(id+1)*(N/CANT_THREADS);
			for (int i= desde; i < hasta; i++){
					if(min > vector[i]){
						min=vector[i];
					}
					if(max < vector[i]){
						max=vector[i];
					}
			}
	sem_wait(&sem);
	if(min < minimo){
		minimo=min;
	}
	if(max > maximo){
		maximo=max;
	}
	sem_post(&sem);
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
	int j,aux;
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
	aux=sem_init (&sem,NULL,1);
	vector=(int*)malloc(sizeof(int)*N);


		for(j=0;j<N;j++){
			if(j % 50 == 0){
				vector[j]=5;
			}else vector[j]=j;
		}


	pthread_attr_init(&attr);

	timetick = dwalltime();
	for ( j = 0; j < CANT_THREADS; j++)	{
		id[j]=j;
		pthread_create(&threads[j], &attr,mult_matr,&id[j]);
	}


	for (j = 0; j < CANT_THREADS; j++){
	pthread_join(threads[j],NULL);
	}

	printf("la busqueda del minimo y maximo de un vector de %dposiciones. Tiempo en segundos %f\n",N ,dwalltime() - timetick);

	printf("el minimo es: %d  y el maximo: %d \n",minimo,maximo);

	free(vector);
	return 0;

}	