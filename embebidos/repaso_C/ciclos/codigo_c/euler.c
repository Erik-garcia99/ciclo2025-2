#include<stdio.h>
#include<string.h>

//este es para escalar el numero, enter mas grande mas resolucion
#define SCALED 100000


int factorial(int numer_fac){

    int suma =1;
    for(int i=1; i<=numer_fac; i++){

        suma *=i;
    }

    return suma;

}

int calcular_e(int factorial){


    int suma = 0; 

    //for(int i=1; i<=recorrido; i++){

    int term = SCALED/factorial;

    suma += term;
        
    //}
    return (suma);
}


int main(void){

    int fac = 0;
    int i;
    int j;
    float euler = 0;
    for(i=1; i<10; i++){

        for(j=1; j<10 && j<=i; j++){
            printf("1/%d!  ", j);
        }

        fac = factorial(i);
        printf("%d!:", i);
        printf("%d", fac);
        euler = calcular_e(fac);
        printf("\t|||EULER : %f", euler);
        printf("\n");
    }




    return 0;
}


