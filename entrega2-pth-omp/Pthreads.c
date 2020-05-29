#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#define MINIMOVALOR -DBL_MAX
#define MAXIMOVALOR DBL_MAX


//Dimension por defecto de las matrices
int N=2048;
pthread_barrier_t barrera;
sem_t semA,semB,semC;
double MINA = MAXIMOVALOR;
double MINB = MAXIMOVALOR;
double MINC = MAXIMOVALOR;
double MAXA = MINIMOVALOR;
double MAXB = MINIMOVALOR;
double MAXC = MINIMOVALOR;
double *A,*B,*C,*D,*AB;
double avgA = 0;
double avgB = 0;
double avgC = 0;
double ecuacion;
int cantThreads;

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

//Realiza la multiplicacion
  void *multiplicacion( void *id){
          int i,j,k,disp1,disp2,disp3;
          double aux;
          double minA = MAXIMOVALOR; double minB = MAXIMOVALOR; double minC = MAXIMOVALOR; //inicializo minimos locales
          double maxA = MINIMOVALOR; double maxB = MINIMOVALOR; double maxC = MINIMOVALOR; //inicializo maximos locales
          double sumA = 0; double sumB = 0; double sumC = 0; //inicializo la suma de los elementos locales
          int ID = *((int*)id);
          int desde = (N/cantThreads) * ID;
          int hasta = desde + (N/cantThreads);
          for( i=desde;i<hasta;i++){ //este for calcula los valores min, max, suma de elementos(de cada matriz) y la matriz AB
            disp1 = i*N;
	    for( j=0;j<N;j++){
		disp2 = disp1 + j; //i*N+j    
                aux = 0;
                if(maxA < A[disp2])maxA=A[disp2];
                if(maxB < B[disp2])maxB=B[disp2];
                if(minA > A[disp2])minA=A[disp2];
                if(minB > B[disp2])minB=B[disp2];
                sumA+=A[disp2];
                sumB+=B[disp2];
		disp3 = j*N;
                for( k=0;k<N;k++){
                    aux += A[disp1+k]*B[k+disp3];
                }
                AB[disp2] = aux;
        }
    }
    for( i=desde;i<hasta;i++){ //este for calcula los valores min, max, suma de elementos(de cada matriz) y la matriz AB
        disp1 = i*N;
        for( j=0;j<N;j++){
	   disp2 = disp1 + j; //i*N+j    
           aux = 0;
	   if(maxC < C[disp2])maxC=C[disp2];
	   if(minC > C[disp2])minC=C[disp2];
           sumC+=C[disp2];
	   disp3 = j*N;
	   for( k=0;k<N;k++){
                    aux += AB[disp1+k]*C[k+disp3];
                }
                D[disp2] = aux;
	}
    }	
    sem_wait(&semA);
    if(MAXA<maxA)MAXA=maxA;
    if(MINA>minA)MINA=minA;
    avgA+=sumA;
    sem_post(&semA);
    sem_wait(&semB);
    if(MAXB<maxB)MAXB=maxB;
    if(MINB>minB)MINB=minB;
    avgB+=sumB;
    sem_post(&semB);
    sem_wait(&semC);	    
    if(MAXC<maxC)MAXC=maxC;
    if(MINC>minC)MINC=minC;
    avgC+=sumC;
    sem_post(&semC);
    pthread_barrier_wait(&barrera); //espero a que terminen su calculo todos los hilos 
     if(ID == 0){ //la ecuacion la calcula solamente un hilo
	    avgA/=(N*N);
	    avgB/=(N*N);
	    avgC/=(N*N); 
            ecuacion = ((MAXA*MAXB*MAXC) - (MINA*MINB*MINC)) / (avgA*avgB*avgC) ;
            printf("ecuacion: %f\n",ecuacion);
    }
    pthread_barrier_wait(&barrera); //espero a que se calcule la ecuacion.
    for( i=desde;i<hasta;i++){
	 disp1 = i*N;
           for( j=0;j<N;j++){
            D[disp1+j] = D[disp1+j] * ecuacion;
            }
        }
        pthread_exit(0);
  }

int main(int argc,char*argv[]){
 int t;
 double timetick;
 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((cantThreads = atoi(argv[2])) <= 0) ) {
    printf("la cantidad de parametros ingresados no es correcata o no estan bien debe ser nombre del programa N(representara a la matriz NxN) y cant threads\n");
    exit(1);
  }
 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  AB=(double*)malloc(sizeof(double)*N*N);


 //Inicializo las matrices.
  for(int i=0;i<N;i++){
   for(int  j=0;j<N;j++){
      A[i*N+j]=i+j;
      B[j*N+i]=i+j;
      C[j*N+i]=i+j;
   }
  }   

 int ids[cantThreads];
 pthread_attr_t attr;
 pthread_t threads [cantThreads];
 pthread_attr_init(&attr);
 pthread_barrierattr_t barrierattr;
 pthread_barrier_init(&barrera,&barrierattr,cantThreads);
 sem_init(&semA,0,1);
 sem_init(&semB,0,1);
 sem_init(&semC,0,1);
 timetick = dwalltime();
 for (t=0;t < cantThreads; t++){
	ids[t] = t;
 	pthread_create(&threads[t],&attr,multiplicacion ,&ids[t]);
 }
 for (t=0;t < cantThreads; t++){
	 pthread_join(threads[t],NULL);
 }
 printf("TamaÃ±o de matriz: %dx%d. Tiempo en segundos %f\n",N,N, dwalltime() - timetick);
 sem_destroy(&semA);
 sem_destroy(&semB);
 sem_destroy(&semC);
 pthread_barrier_destroy(&barrera);
 free(A);
 free(B);
 free(C);
 free(AB);
 free(D);
 return(0);
}
