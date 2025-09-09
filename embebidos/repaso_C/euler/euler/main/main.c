#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * 
 * imprementar una aplicacion que se aproxime al valor de 
 * euler el cual puede ser representado como 
 * 
 * e= 1 + 1/1! + 1/2! + 1/3! + ... 1/n!
 * 
 * 
 */



void app_main(void)
{

    int i,j;

    int euler=1;
    for(i=0;i<10;i++){
        for(j=1;j<=i;j++){

            //euler = 1/j; -> la divicion dentro de un uC es algo medio complicado y algo que no se recomienda 
            euler %=j;
            if(j==0){
                printf("1 + ");
            }
            else{
                printf("1/%d! + ",j);
            }

        }
        printf("= %d", euler);
        printf("\n");
        euler=0;
    }



}

// se deberia de imprimir en fomra consecutira, es decir

/*
 1 = e
 1+1/1! = e

 1+ 1/1! +1/2! = e

 ... hasta el 10
*/

//se me ocurren varias formas pero para un numero como de 10 estarai bien pero si fueran 100,1000 o mas ya no lo seria tanto.

//se me ocurre una solucion recursiva, pero la recursividad es algo que si le afecta a los uC. 

/*
se puede imprimir al mismo momento que se esta realizardo la opreacion, 

pero recoradno que el factorial es la sumatoria de todos los numeros anteriores.


al parecer el proceso a lo mucho seria 2 ciclos anidados 

-> una ciclo prinicpal que recorrera  que establecera hasta donde llegara el ciclo interior
*/


