#include <stdio.h>
#include <stdlib.h>






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
	int *vector;
	int N;
	int total=0;
	int j;

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

	vector=(int*)malloc(sizeof(int)*N);


		for(j=0;j<N;j++){
			if(j % 50 == 0){
				vector[j]=5;
			}else vector[j]=j;
		}

	timetick = dwalltime();

		for(j=0;j<N;j++){
			total+=vector[j];
		}

	printf("la busqueda del vector de %dposiciones. Tiempo en segundos %f\n",N ,dwalltime() - timetick);

	printf("el promedio de los numeros almacenados en el vector es %d\n",(total/N));

	free(vector);
	return 0;

}	