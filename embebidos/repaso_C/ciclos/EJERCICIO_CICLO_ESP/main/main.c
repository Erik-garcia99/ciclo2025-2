#include <stdio.h>
// Escala: multiplicamos todo por 10^9 para mantener precisión
#define SCALE 1000000
/*

ejercicio 

implementar un programa que se apixie al valor de e la cual puede ser expresada como: 

e = 1 + 1/1! + 1/2! + 1+3!+ ...1/n!

desplegar las respuests para n = 1 a 10 

no usar datos de punto flotante 

factorial es la multiplicacion de los numeros enteros e incluso el mismo numero 
*/


//factorual ya esta chilo 
int factorial(int intervalo){

    int suma = 1;
    for(int i=1; i <= intervalo ; i++){

        //un facotural es la suma de todos sus valores anteriores hasta el
        suma *= i;
    }

    return suma;
}

void scale(int numer_scaled){

    int parte_entera = numer_scaled / SCALE;
    int parte_decimal = numer_scaled % SCALE;

    printf("%d.%09d", parte_entera, parte_decimal);

}

void app_main(void)
{
    int euler =0;
    int sumador = 0; 

    int i;
    int j;
    for(i=0 ; i < 10; i++){

        printf("%d -> ", i);
        for(j=0; j< 10 && j<=i;j++){

            // if(j==1){
            //     continue;
            // }
            // else{
            //     int fac = factorial(j);
            //     euler = euler + (SCALE /fac);
            //     printf(" - ");
            //     scale(euler);
            //     printf("%d",euler);
            //     printf(" - ");
            // }
            

            
            // // printf("- %d -", fac);
            // sumador = SCALE/fac;
            // euler +=sumador;
            // printf(" - ");
            // scale(euler);
            // printf("%d",euler);
            // printf(" - ");
        }


        int fac = factorial(j);
        euler += 1 + (SCALE/fac);

        printf(" - ");
        scale(euler);
        printf("%d",euler);
        printf(" - ");
        printf("\n");

    }



}