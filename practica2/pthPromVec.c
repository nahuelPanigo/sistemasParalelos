#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define CANT_THREADS 2

//globales
		int *vector;
		int N;
		int total=0;
		int num;
		pthread_mutex_t mutex;


//funcion que va a ejecutar los hilos
void * mult_matr(void *ident){
	int *p =(int *) ident;
	int id= *p;
	int desde,hasta;
	long int cant=0;
	desde=id*N/CANT_THREADS;
	hasta=(id+1)*N/CANT_THREADS;
			for (int i= desde; i < hasta; i++){
					cant+=vector[i];
			}
	pthread_mutex_lock(&mutex);
	total+=cant;
	pthread_mutex_unlock(&mutex);
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
	int j;
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

	pthread_mutex_init(&mutex, NULL); 
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

	printf("la busqueda del vector de %dposiciones. Tiempo en segundos %f\n",N ,dwalltime() - timetick);

	printf("el promedio de los numeros almacenados en el vector es %d\n",(total/N));

	free(vector);
	return 0;

}	