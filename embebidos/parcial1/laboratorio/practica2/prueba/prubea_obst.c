#include<stdio.h>

#define LANE_TOP 1      // Carril superior
#define LANE_CENTER 4   // Carril central
#define LANE_BOTTOM 7   // Carril inferior

#define SPAWN_OBJ 5 //el spawn de nuestro vehiculo
#define SPACE_OBJ 5 //el esapcio minimo que debe de haber entre objetos 

#define WIDTH_SCRE 50
#define HEIGTH_SCRE 10

volatile int obstacle[3][WIDTH_SCRE]={0};
//matriz princola 
//volatile int obstacle[HEIGTH_SCRE][3];
//volatile char canvas[WIDTH_SCRE][HEIGTH_SCRE];

volatile char canvas[HEIGTH_SCRE][WIDTH_SCRE]={0};

//en el parametro < ROW > vamos a pasar una de las 3 macros y verificaremos con un switch 
//regresa 1 o 0 
//0 para NO insertarlo 
//1 para SI insertarlo 
int check_space_availabe(int row,int col){

    int space_needed = SPACE_OBJ;
    int cumpled = 0;
    //
    switch(row){

        case LANE_TOP:{
            //porque menor que el ancho de la pantalal?, porque realmente no se en que parte aparecera nuestro, pero aun no se si ponde otra variable para que esta salga o solo un break
            for(int x = col; x < WIDTH_SCRE && space_needed < 6;x++){
                //en la matriz de obstaculo

                if(obstacle[LANE_TOP][x] != 1){
                    space_needed++;
                    if(space_needed < 6){
                        cumpled =1;
                    }
                }
                else{
                    //en alguno de los espacios antes de los  5 salimso del ciclo y del switch
                    cumpled =0;
                    break;
                }
                
            }
        }break;


        case LANE_CENTER:{
            for(int x = col; x < WIDTH_SCRE && space_needed < 6;x++){
                //en la matriz de obstaculo

                if(obstacle[LANE_CENTER][x] != 1){
                    space_needed++;
                    if(space_needed < 6){
                        cumpled =1;
                    }
                }
                else{
                    //en alguno de los espacios antes de los  5 salimso del ciclo y del switch
                    cumpled =0;
                    break;
                }   
            }
        }break;

        case LANE_BOTTOM:{

            for(int x = col; x < WIDTH_SCRE && space_needed < 6;x++){
                //en la matriz de obstaculo

                if(obstacle[LANE_BOTTOM][x] != 1){
                    space_needed++;
                    if(space_needed < 6){
                        cumpled =1;
                    }
                }
                else{
                    //en alguno de los espacios antes de los  5 salimso del ciclo y del switch
                    cumpled =0;
                    break;
                }
            }
        }break; 
    }
    return cumpled;
}





int main(void){


    int cumple= check_space_availabe(1,20);



}