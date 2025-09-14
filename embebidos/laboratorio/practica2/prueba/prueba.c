#include <stdio.h>
#include <unistd.h> // Para sleep() o usa vTaskDelay en ESP32
#include <string.h> // Para memset()

// Define el tamaño de tu área de juego
#define SCREEN_WIDTH 80
#define GAME_HEIGHT  10 // El número de líneas entre los bordes será fijo

// Función para imprimir una línea de borde
void print_border_line() {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        putchar('=');
    }
    putchar('\n');
}

int main() {
    // 1. Define el lienzo virtual
    char canvas[GAME_HEIGHT][SCREEN_WIDTH];
    int car_position_y = GAME_HEIGHT / 2; // Posición vertical del carro
    int car_position_x = 5;              // Posición horizontal del carro

    // Bucle principal del juego
    for (int frame = 0; frame < 20; frame++) {
        // 2. Limpia el lienzo virtual con espacios
        memset(canvas, ' ', sizeof(canvas));

        // 3. "Dibuja" en el lienzo (actualiza el estado del juego)
        // Dibuja el carro en su nueva posición
        if (car_position_x + 3 < SCREEN_WIDTH) { // Asegurarse de no salirse del lienzo
             canvas[car_position_y][car_position_x]     = '<';
             canvas[car_position_y][car_position_x + 1] = '=';
             canvas[car_position_y][car_position_x + 2] = '>';
        }

        // Mueve el carro para el siguiente fotograma
        car_position_x++;

        // 4. Limpia la pantalla real y mueve el cursor
        printf("\033[2J\033[H");

        // 5. Imprime el marco completo
        print_border_line();

        // Imprime el contenido del lienzo
        for (int i = 0; i < GAME_HEIGHT; i++) {
            // Imprime una fila completa del lienzo
            // El '%.*s' es para asegurarse de imprimir exactamente SCREEN_WIDTH caracteres
            printf("%.*s\n", SCREEN_WIDTH, canvas[i]);
        }

        print_border_line();
        printf("Fotograma: %d\n", frame);

        fflush(stdout);
        usleep(100000); // Espera 100ms (en ESP32 usarías vTaskDelay)
    }

    return 0;
}
