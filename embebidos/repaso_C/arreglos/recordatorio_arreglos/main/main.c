//librerias basicas de C 
#include<stdio.h>

//librerias ed freeRTOS 
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>

//manejador de errores
#include<esp_log.h>



/*
que es lo que pide el ejercicos? 

la aplicacion ordena de froma ascendente los numeros de un arreglo de W numeros enteros de 8 bits positivos. 

y despues remueve los duplicados 

-los numreo deben seguir ordenados despues de remover los duplicados 


-> entonces el algoritmos 

1 -> ordena de menor -> mayor

2 -> elimina los duplicados y recorrer lo numeros sin eliminar al inciio y los numeros finales rellenarlos con -1 

3- los elementos deben de estar ordenados despues de elininar los duplicados 

*/

//el ingreso lo haremos medainte 2 maneras, solo por practicar 
//mediante UART - arreglo estatico 

//1 medinate un metodo estatico 

#define N 10 //cantidad que tiene el arreglo 


//variables 

const static char *TAG="ordenamiento";

//funcion que recibe el arreglo para arreglarlo 

void sort_array(uint8_t *vector,uint8_t tam);
//funcion que elimina los datos repetidos
void delete_rep(uint8_t *vector, uint8_t tam);

//tareas 

void init_task(void *params);


void app_main(void){


	xTaskCreate(init_task, "init_task", 2048, NULL, 10,NULL);



}


void init_task(void *params){

	uint8_t arreglo[N]={4,1,6,4,8,9,5,6,3,7};

	sort_array(arreglo, N);

	ESP_LOGI(TAG,"elemtnos ya arreglados");
    for(int i =0; i<N; i++){
        ESP_LOGI(TAG, "%d", arreglo[i]);
    }



	vTaskDelete(NULL);
}	


void sort_array(uint8_t *vector,uint8_t tam){

    //lo mas sencillo seria con quicksort 

    uint8_t i;
	uint8_t j;       
	for(i=0;i<N-1;i++){

		for(j=i+1;j<N;j++){
			
			uint8_t actual = vector[i];
			uint8_t cmp = vector[j]; 

			if(actual > cmp){
			    vector[i]= cmp;            
		    	vector[j]= actual;
			}   
			else if(actual == cmp){                                               
				continue;
            
			}

		}
	}
}



void delete_rep(uint8_t *vector, uint8_t tam){



	//ahora esta funcion va a recibir el vector ya ordenado 




}


