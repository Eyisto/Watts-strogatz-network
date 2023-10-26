#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

double ran_expo(double lambda){
	double u;
	u = rand() / (RAND_MAX + 1.0);
	return  -log(1- u) / lambda;
}


int main(int argc, char *argv[]) {
	int **m,*v,contador,**matriz;
	int bin,b,c,a,seed1;
	int size;
	int N, k,p1,d1,s,cont,wsub,steps;
	int i, j,l,left, right,numero, t, nodo,conex,antconex,h,g;
	double r, ran,d, p,seed2;
	_Bool done,salida,reconex,conectado,reverse,noesta;

	FILE *data;
	FILE *matriz1;
	//FILE *matriz2;

	char fileName[100];
	char fileName4[100];
	//char fileName5[100];

	seed1=time(NULL);
	srand48(time(NULL));


	//////valores iniciales///////////
	N=atoi(argv[1]);
	k=atoi(argv[2]);
	p=atof(argv[3]);
	p1=p*100;
	size=5*k;
	///////////////////////////////
	sprintf(fileName,"DataREDWSN%dK%dPI%d.txt",N,k,p1);
	sprintf(fileName4,"matrizconeccionesN%dK%dPI%d.txt",N,k,p1);
	//sprintf(fileName5,"xMat0");


	if((data=fopen(fileName, "w")) == NULL){
		printf("Error: No se pudo abrir el archivo %s\n",fileName);
	}

	if((matriz1=fopen(fileName4, "w")) == NULL){
		printf("Error: No se pudo abrir el archivo %s\n",fileName4);
	}
	fprintf(data,"#seed=%d\n",seed1);


	printf("N=%d ; k=%d ; pi=%.3f \n", N,k,p);
	fprintf(data, "N=%d ; k=%d \n", N,k);


	///////////////////////////////////////////////////////

	if((v = (int*)calloc(N,sizeof(int)))==NULL){
		printf("Error al asignar memoria\n");
	}



	if((m = (int**)calloc(N,sizeof(int*)))==NULL){
		printf("Error al asignar memoria\n");
	}

	for(i=0; i < N; i++){

		if((m[i] = (int*)calloc(5*k,sizeof(int)))==NULL){
			printf("Error al asignar memoria\n");
		}

		for(j=0; j < k*5 ;j++){ //acá lleno la matriz m de -1
			m[i][j] = -1;
		}


	}
	/////////////////////////////////////////////////////////////////////////////

	//Agrego un enlace a los vecinos de la izquierda y la derecha de cada nodo////
	for (i = 0; i < N; ++i){
		b=i;
		//printf("aquiestoy %d\n",i);
		right = k/2;
		for (j = 0; j < right; ++j){

			if ((b == N-1)){
				b=0;
				m[i][j]=b;
			}else{
				//printf("%d\n",b);
				b=b+1;
				m[i][j]=b;
			}
		}
	}//printf("matriz sin recableado\n");

	////////cuento los vecinos antes del recableado///////////////////
	for (i = 0; i < N ;++i){

		contador=0;

		for (j = 0; j < 5*k; ++j){
			if (m[i][j] !=-1)
			{

				contador=contador+1;
			}
			v[i]=contador;
		}

	}

	////////Reescritura de los enlaces///////////////
	//La reescritura se realiza con los nodos a la derecha//


	srand(time(NULL));

	for (i = 0; i < N; ++i){
		for (j = 0; j < k/2; ++j){

			r = drand48();

			if (r < p){


				done=false;
				while ( done == false){

					numero = rand() % (N);//numero aleatorio entre 0 y N

					if ((numero != i) && (numero!=m[i][j])){ // el numero debe ser diferente a el mismo y ser diferente al que ya esta conectado
						conectado=false;
						for (l = 0; l < size; ++l){ //Ac? reviso si ya esta conectado, y lo hago sobre el numero de vecinos v
							if (m[i][l]==numero) {
								conectado=true;
								//printf("el nodo m[%d][%d]=%d esta igual a %d\n",i,l,m[i][l],numero);
							}

						}
						for (l = 0; l < size; ++l){ //Ac? reviso si ya esta conectado, y lo hago sobre el numero de vecinos v
							if (m[numero][l]==i) {
								conectado=true;
								//printf("el nodo m[%d][%d]=%d esta igual a %d\n",i,l,m[i][l],numero);
							}

						}
						if (conectado==false){
							done = true;
						}
					}
				}
				nodo=i; //nodo que se reconectará
				conex=numero; //conexion al nodo recableado
				//no reconectar/////
				antconex=m[i][j]; // el nodo que será desconectado
				m[i][j]=numero;
			}
		}
		//}
	}


	for (i = 0; i < N ;++i)
	{ contador=0;
		//printf("v[%d]\t",i);
		for (j = 0; j < 5*k; ++j)
		{if (m[i][j] !=-1)
			{
				contador=contador+1;
			}
			v[i]=contador;
		}

	}


	double **peso;
	int Simetricovecino;

	if((peso = (double**)calloc(N,sizeof(double*)))==NULL){
		printf("Error al asignar memoria\n");
	}

	for(i=0; i < N; i++){

		if((peso[i] = (double*)calloc(N,sizeof(double)))==NULL){
			printf("Error al asignar memoria\n");
		}

		for(j=0; j < v[i]; j++){
			// deberia funcionar
			peso[i][j]=ran_expo(12.5);
		}
	}


	//printf(" SIMETRIZA\n");
	for (i = 0; i < N; ++i){
		for (j = 0; j < v[i]; ++j){

			Simetricovecino=m[i][j];
			cont=0;
			for (l = 0; l < 5*k; ++l)//reviso que ya no este, si esta me salgo del bucle
			{
				if ((m[Simetricovecino][l]==i)){
					cont=cont+1;
					break;
				}
			}
			if (cont==0)//si no esta, lo agrego a la siguiente posición
			{
				m[Simetricovecino][v[Simetricovecino]]=i;
				peso[Simetricovecino][v[Simetricovecino]]=peso[i][j];
				v[Simetricovecino]=v[Simetricovecino]+1; //aca sumo 1 vecino luego de asignarlo porque en C comienzo en cero
			}//else{peso[Simetricovecino][v[Simetricovecino]]=0;}
		}
	}

			////////////////////NetworkMetrics//////////////////////////
			double NumfEdges=0;
			double sumawij=0;

			for (i = 0; i < N; i++) {
				for (j = 0; j < v[i]; j++) {
					fprintf(matriz1,"%d %d\n",i,m[i][j]);
					sumawij=sumawij+peso[i][j];
				}
				NumfEdges = NumfEdges + j;
				//printf("%d %d %lf \n",i,j,NumfEdges);
			}
			printf("Num Edges = %lf ; Average Degree k = %lf\n",NumfEdges,NumfEdges/(double)N);
			printf("suma wij = %lf ; Average wij = %lf\n",sumawij,sumawij/NumfEdges);
			fprintf(data, "Average Degree %lf\n",NumfEdges/(double)N);
			fprintf(data, "Average wij = %lf\n",sumawij/NumfEdges);
		}
