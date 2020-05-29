#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define CANT_THREADS 4

//globales
		int *conj1, *conj2,*conj3;
		int N;
		int total=0;
		int num;
		pthread_mutex_t mutex;



//funcion que va a ejecutar los hilos
void * mult_matr(void *ident){
	int *p =(int *) ident;
	int *conj,cant,id= *p;
	int desde,hasta,ok,longitud=0;

	desde=id*N/CANT_THREADS;
	hasta=(id+1)*N/CANT_THREADS;
	cant=hasta-desde;
	conj=(int*)malloc(sizeof(int)*cant);
			for (int i= desde; i < hasta; i++){
				for(int j= 0;j<N;j++){
					if(conj2[i]==conj1[j]){
						conj[longitud]=conj2[j];
						longitud++;
						j=N;
					}
				}
			}
	pthread_mutex_lock(&mutex);
		for (int j = 0; j < longitud; j++)
		{
				ok=1;
				for (int i= 0; i < total; i++)
				{
					
					if( conj3[i] == conj[j] ){
						ok=0;
						i=total;
					}
					
				}
				if(ok){
					conj3[total]=conj[j];
					total++;
				}

		}
	pthread_mutex_unlock(&mutex);
	free(conj);
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
	conj1=(int*)malloc(sizeof(int)*N);
	conj2=(int*)malloc(sizeof(int)*N);
	conj3=(int*)malloc(sizeof(int)*N);

		for(j=0;j<N;j++){
			if(j % 4 == 0){
				conj1[j]=4;
				conj2[j]=4;
			}else{
				conj2[j]=j;
				conj1[j]=j+1;
			} 
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
/*
	for (int i = 0; i < total; i++)
	{
		printf("[%d] \n",conj3[i]);
	}
*/
	free(conj1);
	free(conj2);
	free(conj3);
	return 0;

}	