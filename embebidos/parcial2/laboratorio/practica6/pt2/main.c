#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "rom/ets_sys.h"

#define LCD_20X04   1

//configuracion para la comunicacion I2C
#define I2C_ACK_CHECK_EN      1
#define I2C_ADDRESS_LCD       0x27
#define I2C_SCL_LCD           21
#define I2C_SDA_LCD           22

/* Comandos de la LCD */
//estos comandos son definidos por hitachi en el controlador del HD44780

#define CLEAR_DISPLAY               0x01
#define RETURN_HOME_UNSHIFT         0x02
#define CURSOR_RIGHT_NO_SHIFT       0x04 //el cursor se mueve a la drecha despues de escribir 
#define CURSOR_RIGHT_SHIFT          0x05 //el cursos dercha + display se desplaza
#define CURSOR_RIGHT_NO_SHIFT_LEFT  0x06
#define CURSOR_RIGHT_SHIFT_LEFT     0x07
#define DISPLAY_OFF                 0x08
#define DISPLAY_ON_CURSOR_OFF       0x0C
#define DISPLAY_ON_CURSOR_ON_STEADY 0x0E
#define DISPLAY_ON_CURSOR_ON_BLINK  0x0F
#define SHIFT_CURSOR_LEFT           0x10 //mueve el cursor 1 posicion a la izquierda (sin escribir)
#define SHIFT_CURSOR_RIGHT          0x14 //mueve el cursosr a la derecha
#define SHIFT_DISPLAY_LEFT          0x18 //mueve TODO el contenido del display a la izquierda
#define SHIFT_DISPLAY_RIGHT         0x1C//mueve todo a la drecha 
#define SET_4BIT_MODE               0x28
#define RETURN_HOME                 0x80

/* PCF8574 */
//pines de expansor I2C 
#define PCF8574_RS    0
#define PCF8574_RW    1
#define PCF8574_EN    2
#define PCF8574_BL    3

//macros que usaremos para indciar si lo que se le manda al LCD sera una instrucciones o sera algo que queremos que escriba 
#define LCD_RS_CMD    0
#define LCD_RS_DATA   1

//agrupa toda la configuracion del LCD en una estrucutra
typedef struct {
  uint8_t i2c_address;
  uint8_t i2c_port;
  uint8_t screen_size;
  uint8_t screen_backlight;
} lcd_i2c_device_t;

void i2c_init(void);
void lcd_init(lcd_i2c_device_t * lcd);
void lcd_i2c_write_byte(lcd_i2c_device_t * lcd, uint8_t data);
void lcd_i2c_write_command(lcd_i2c_device_t * lcd, uint8_t register_select, uint8_t cmd);
void lcd_set_cursor(lcd_i2c_device_t * lcd, uint8_t column, uint8_t row);
void lcd_i2c_write_custom_char(lcd_i2c_device_t * lcd, uint8_t char_address, const uint8_t * pixels);

void i2c_init(void) 
{
  i2c_config_t i2c_config = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_SDA_LCD,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = I2C_SCL_LCD,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = 400000,
  };

  esp_err_t error = i2c_param_config(I2C_NUM_1, &i2c_config);
  //en el caso de que no se puedan configurar los parametros del LCD este se queda en un bucle infinito impidiendo que se pueda seguir con el flujo del programa 

  if (error != ESP_OK) {
        while(1);
    }
  i2c_driver_install(I2C_NUM_1, I2C_MODE_MASTER, 0, 0, 0);
}

/**
 *secuencia de inicio  para el LCD 
 * 
 * 
 *  
 * 
*/
void lcd_init(lcd_i2c_device_t * lcd)
{
  // 1 - inica el cursor al inicio de la pantalla 
  lcd_i2c_write_command(lcd, LCD_RS_CMD, RETURN_HOME_UNSHIFT);
  // 2- configura el modo a 4 bits (el modo 4 bits significa que divide cada byte en dos envios)
  //  en este modo primero se envio el nibble mas alto y despues el mas bajo, ese se hace para
  //ahorrar pines de conexion 
  lcd_i2c_write_command(lcd, LCD_RS_CMD, SET_4BIT_MODE);
  //limpia la pantalla
  lcd_i2c_write_command(lcd, LCD_RS_CMD, CLEAR_DISPLAY);
  //enciente el display sin cursor
  lcd_i2c_write_command(lcd, LCD_RS_CMD, DISPLAY_ON_CURSOR_OFF);
  // modo auto-incremento del cursor 
  //este despues de escribir un caracter lo mueve automaticamente a la sigueinte posicion 
  lcd_i2c_write_command(lcd, LCD_RS_CMD, CURSOR_RIGHT_NO_SHIFT_LEFT); 
  vTaskDelay(20 / portTICK_PERIOD_MS);
}



void lcd_i2c_write_byte(lcd_i2c_device_t * lcd, uint8_t data)
{
  /**
   * 
   * este es el proceo con el cual se establece la comunicacion, el MASTER (ESP) se va a comunicar con el esclavo (LCD)
   * 
   * el proceso es que primero debe de crear un link, uin enlace 
   * 
   */
  i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
  i2c_master_start(cmd_handle); //empieza la comunicacion entre el MASTER y el SLAVE
  /**
   * le indicamcmos en que periferico queremos escribir, en este caso recorremos la direccion 1 bit a la izquierdam esto, porque la direccion de nustro periferico es de 
   * 7 bits, asi vez hacemos una operacion OR indicando que queremos escribir, queremos enviar algo del controlador al LCD, un comando o un mensaje que queremos que muestr
   * debemos de termina con el ACK, este es el acuerdo ester el maestro y el escalvo que indica que se ha recibico el byte. el ACK es importante porque es el que 
   * le va a indicar al controlador si es que ese controlador esta disposnible, esta conectado, se activa cunado en este caso enviamso la direccion primero debe de verificar 
   * que dicho periferico se encuentre dentro de su bus de datos. 
  */
  i2c_master_write_byte(cmd_handle, (lcd->i2c_address << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN); 
  /**
   * enviamos los datos, se puede enviar n bytes los necesarios
   */
  i2c_master_write_byte(cmd_handle, data, 1);
  //terminasmo la comunicacion y dejamos el bus de datos libre para que otro disposivo pueda usarla, si no relaizamos esta operacion el bus permanece ocupado y puede 
  //ocacionar un error fatal 
  i2c_master_stop(cmd_handle);
  /**
   * envia todos los comandos en cola en el bus I2C, la tarea que bloquea hasta que se hayan enviados todos los comandos. 
   */
  i2c_master_cmd_begin(lcd->i2c_port, cmd_handle, 100 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd_handle);//liberamos la memoria 
}


/**
 * 
 * esta funcion es la mas improtante porque es la que maneja el procolo de comunicacion con el LCD en modo de 4 bits 
 * 
 */
void lcd_i2c_write_command(lcd_i2c_device_t * lcd, uint8_t register_select, uint8_t cmd)
{
  /**
   * en esta primera linea lo que se esta preguntadno es que si lo que se va a escribir en el bus va ser un comando o sera datos que se enivaram 
   * 1 << PCF8574 se hace para activar el bit RS loq ue se enivaraia en caso que el register_Select sera 1 lo que resultaria en infromacion 
   */
  uint8_t config = (register_select)? (1 << PCF8574_RS) : 0;
  //el backlight indica si esta prendido o apago esto para poder escribir algo y que se note que se muestre, 
  config |= (lcd->screen_backlight)? (1 << PCF8574_BL) : 0;

  //este es el proceso en enviar el nibble alto
  
  //se estan tomando los bits 4-7, pero queremos mantener el nibble mas bajo sin afectaciones, por lo que aplicamos un OR y una mascara de 0x0F para que no se afectque
  //ningun bit que este en alto ni cambien los que esten en bajo 
  config |= (config & 0x0F) | (0xF0 & cmd);  
  //estamos activando el pin EN 
  config |= (1 << PCF8574_EN);
  //ahora enviamos la configruacion actual el cual es el comando a actual
  lcd_i2c_write_byte(lcd, config);
  ets_delay_us(10);
  config &= ~(1 << PCF8574_EN); //lo que se hace es sellar la transferencia esto para poder mandar el nibble menos significativo 
  lcd_i2c_write_byte(lcd, config); //ahora envia los dato con EN desactivado, esto completa el pulso en flanco de bajada 
  ets_delay_us(50);

  //nibble bajo 
  //aun queremos manter la configuracion actual del reister select, RW, EN y BL entonces del comando desplazamos 4 espacios para obtener los bits menos significativo 
  config = (config & 0x0F) | (cmd << 4);
  //acticamos EN para que se pueda relziar la comunicacion entre el uC y el LCD por medio de I2C 
  config |= (1 << PCF8574_EN);
  //enviamos el segundo byte
  lcd_i2c_write_byte(lcd, config);
  ets_delay_us(10);
  //terminamos con el enviio de datos 
  config &= ~(1 << PCF8574_EN);
  //enviasmos este valor para poder cerrar el envio de datos 
  lcd_i2c_write_byte(lcd, config);
  ets_delay_us(50);

  if (cmd == CLEAR_DISPLAY)
  {
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

/**
 * 
 * la funcion recibe un apuntador a la estrucutra que configura el LCD que contiene su direccion asi como en que columna y fila va a querer que se posicione el cursor 
 */
void lcd_set_cursor(lcd_i2c_device_t * lcd, uint8_t column, uint8_t row)
{
  /**
   * un LCD 20x4 tiene 20 columnas x 4 filas entonces cada fila tiene una direccion empezando por 0x80 que es la primera fila del LCD y 0xD4 es la ultiuma fial del
   * LCD, entnces lo que esta pasando con esta funcion
   * estas direcciones estan dadas en el datasheet del HD44780
   * 
   */
  switch (row) {
    case 0:
      //aqui lo que se esta haciendo es que queremos un comando el comando, el cual pondria nuestro cursor en X posicion de la pantalla en el caso del primer case 
      //hacemos un incrementeo hacia la derecha que son las columnas, como si fuera un gotoxy(). 
      lcd_i2c_write_command(lcd, LCD_RS_CMD, 0x80 + column);
      break;
      //cada linea del LCD tiene su direccion "inical" porque es con la que empieza la fila y nos desplazamos sobre las columnas en cada linea 
    case 1:
      lcd_i2c_write_command(lcd, LCD_RS_CMD, 0xC0 + column);
      break;
    case 2:
      lcd_i2c_write_command(lcd, LCD_RS_CMD, 0x94 + column);
      break;
    case 3:
      lcd_i2c_write_command(lcd, LCD_RS_CMD, 0xD4 + column);
      break;
    default:
      break;
  }
}
/**
 * 
 *esta funcion crea caracteres personalziados, en la memoria del LCD,  este LCD puede almacenar hasta 8 caracteres personalizados. 
 * 
*/

void lcd_i2c_write_custom_char(lcd_i2c_device_t * lcd, uint8_t address, const uint8_t * pixels)
{
  /*
  el 0x40 indca que el comando para acceder a la memoria CGRAM que es donde se almacenan los caracteres especiale. 
  el porque estamos desplzando 3 posicion la direccion es  porque cada caracter ocupa 8 bytes consecutivos en CGRAM
  si tenemos que nuestro caracter tiene la direccion 5 -> 5 * 8 = 40 = 0x28 -> 0x40 | 0x28 = 0x68 -> lo que indica que se posicione el punteo de escritura en la direccion 
  0x28 de la memoria CGRAM que es doinde empieza el caracter peronalizado #5
  */
  lcd_i2c_write_command(lcd, LCD_RS_CMD, 0x40 | (address << 3)); 

  //cada caracter tiene 8 filas de pixeles, 
  for (uint8_t i = 0; i < 8; i++)
  {
    //estamos envinado datos a la memoria no datos 
    lcd_i2c_write_command(lcd, LCD_RS_DATA, pixels[i]);
  }

  //esto es importante, esto se asegura que el puntero regresa a la DDRAM por lo que lo que se escriba despues va a la pantallas y no a CGRAM y consuma la memoria

  lcd_i2c_write_command(lcd, LCD_RS_CMD, RETURN_HOME);
}


//esta funcion solo se encarga de imprimir el mensaje enviado hasta que encuentre el final de la cadena 
void lcd_i2c_print_msg(lcd_i2c_device_t * lcd, char * msg)
{
  uint8_t i = 0;

  while (msg[i] != '\0')
  {
    lcd_i2c_write_command(lcd, LCD_RS_DATA, msg[i++]);
  }
}

//definismos la direccion que tendran los 2 caracteres especiales que indicamos 
#define SMILE        5
#define DROP         2


void app_main()
{
  i2c_init();

  lcd_i2c_device_t my_lcd = {
    .i2c_port = I2C_NUM_1,
    .i2c_address = I2C_ADDRESS_LCD,
    .screen_size = LCD_20X04,
    .screen_backlight = 1,
  };

  vTaskDelay(20 / portTICK_PERIOD_MS);
  lcd_init(&my_lcd);

  /**
   *cada caractere expecial es una matriz de 5x8 pixeles, cada byte representa una fula del caracter. 
   lo que se hace es que cada indice de nuestro arreglo represetna una fila del caracter, por lo que acrivamos con 1 las celdas en donde queremos que
   se muestre para formar nuestro caractere especial.  
  */
  uint8_t smile[8] = {0x00, 0x0A, 0x0A, 0x00, 0x11, 0x0E, 0x00, 0x00}; 
  uint8_t drop[8] = {0x04, 0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E};

  //mandamos a guardar nuestro caracteres espciales dentro de la memoria especializada para eso 
  lcd_i2c_write_custom_char(&my_lcd, SMILE, smile); 
  lcd_i2c_write_custom_char(&my_lcd, DROP, drop);

  bool print_msg = true; //controla si mostramos el mensje o espacios 
  uint8_t conteo_msg = 0; //contador de fila 

  //el mensaje mandado hacia el LCD 
  char message[] = {'H', 'o', 'l', 'a', ' ', 'M', 'u', 'n', 'd', 'o', DROP, SMILE, 0};
  char spaces_char[] = "            ";

  while (1) {

    //posiciona el cursor en la columnas 2 y la fula es inicado con conteo_msg el cual va cambiando de 0 - 1 - 2 - 3 lo que hara es un efecto de mostrar de arriba 
    //hacia abajo el msanejs 
    lcd_set_cursor(&my_lcd, 2, conteo_msg);
    //estara alternando entre 2 valoers mientras sea true mostrara el mensaje cunado este el false borrara el msaneje
    if (print_msg) 
    {
      lcd_i2c_print_msg(&my_lcd, message);
    }
    else 
    {
      lcd_i2c_print_msg(&my_lcd, spaces_char);
    }

    //este llevara el control de cunado mostrar y cunado borrar, primero llenara el LCD con el msanje cunado pase de 3 se reinciara y negara la vairbale print_msg en el
    //caso de de que fuera true -> false borrara los mensajes y de false -> true volvera a poner los mensaes
    conteo_msg++;
    if (conteo_msg > 3) 
    {
      conteo_msg = 0;
      print_msg = !print_msg;
    }

    //tendra un delay de 250 mS
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}

