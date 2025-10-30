#ifndef WIFI_LIB_H
#define WIFI_LIB_H

//HTTP_GET el cleinte PIDE datos al servidor ESP -> USER 

//HTTO_POST el cliente ENVIA datos al servidor ESP <- USER



/**
 *@brief  esta funcion permite guardar en memoria flash los archivos que serviran como la presetnacion web del sistema guarda, el HTML/CSS/JS asi como imgaenes esto para que todo sea mucho mas rapido  
 * 
 * 
*/
void init_spiffs(void);

/**
 * 
 * @brief el proposito es convertir el ESP32 en un router WIFI, en esta funcion se configura el SSID <nombre de la red>, password, la cantiad mazima de estaciones <conexiones> que permite. configura los aspectos basicos de la red mas no inicaliza el rpoceso de la conversion a un ACCESS POINT 
 * 
 * 
 * 
*/
void wifi_init_softap(void);


#endif