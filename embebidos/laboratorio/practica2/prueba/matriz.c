#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>


#define WIDTH_SCRE 50
#define HEIGHT_SCRE 10
#define SPACE_OBJ 5  // Espacio mínimo requerido para que pase el vehículo
#define LANE_TOP 2      // Carril superior
#define LANE_CENTER 5   // Carril central  
#define LANE_BOTTOM 8   // Carril inferior

// Matriz de obstáculos (1 = hay obstáculo, 0 = libre)
int obstacle[HEIGHT_SCRE][WIDTH_SCRE];
char canvas[HEIGHT_SCRE][WIDTH_SCRE];

// Función para verificar si hay espacio libre en un carril específico
int check_space_available(int lane, int start_col, int space_needed) {
    // Verificar que no se salga de los límites
    if (start_col + space_needed >= WIDTH_SCRE) {
        return 0; // No hay suficiente espacio hasta el final
    }
    
    // Verificar que no haya obstáculos en el espacio requerido
    for (int col = start_col; col < start_col + space_needed; col++) {
        if (obstacle[lane][col] == 1) {
            return 0; // Hay un obstáculo, no hay espacio
        }
    }
    return 1; // Hay espacio disponible
}

// Función para verificar si se puede colocar un obstáculo sin bloquear completamente el paso
int can_place_obstacle(int lane, int col) {
    // Verificar que el obstáculo esté dentro de los límites
    if (col >= WIDTH_SCRE || lane < 0 || lane >= HEIGHT_SCRE) {
        return 0;
    }
    
    // No colocar si ya hay un obstáculo
    if (obstacle[lane][col] == 1) {
        return 0;
    }
    
    int space_available = 0;
    
    switch(lane) {
        case LANE_TOP: // Carril superior
            // Verificar que haya espacio en carril central O inferior
            if (check_space_available(LANE_CENTER, col, SPACE_OBJ) || 
                check_space_available(LANE_BOTTOM, col, SPACE_OBJ)) {
                space_available = 1;
            }
            break;
            
        case LANE_CENTER: // Carril central
            // Verificar que haya espacio en carril superior O inferior
            if (check_space_available(LANE_TOP, col, SPACE_OBJ) || 
                check_space_available(LANE_BOTTOM, col, SPACE_OBJ)) {
                space_available = 1;
            }
            break;
            
        case LANE_BOTTOM: // Carril inferior
            // Verificar que haya espacio en carril superior O central
            if (check_space_available(LANE_TOP, col, SPACE_OBJ) || 
                check_space_available(LANE_CENTER, col, SPACE_OBJ)) {
                space_available = 1;
            }
            break;
            
        default:
            return 0; // Carril no válido
    }
    
    return space_available;
}

// Función principal para generar obstáculos
void generate_obstacule(void) {
    // Limpiar la pantalla
    memset(canvas, ' ', sizeof(canvas));
    
    // Limpiar matriz de obstáculos
    memset(obstacle, 0, sizeof(obstacle));
    
    // Generar obstáculos cada 6 columnas desde la posición 15 hasta el final
    for (int x = WIDTH_SCRE - 35; x < WIDTH_SCRE; x += 6) {
        if (x < WIDTH_SCRE) {
            // Generar número random para el carril (0, 1, 2)
            int lane_index = rand() % 3;
            int actual_lane;
            
            // Mapear el índice random a los carriles reales
            switch(lane_index) {
                case 0: actual_lane = LANE_TOP; break;
                case 1: actual_lane = LANE_CENTER; break;
                case 2: actual_lane = LANE_BOTTOM; break;
                default: actual_lane = LANE_CENTER; break;
            }
            
            // Verificar si se puede colocar el obstáculo
            if (can_place_obstacle(actual_lane, x)) {
                // Colocar el obstáculo
                obstacle[actual_lane][x] = 1;
                canvas[actual_lane][x] = '#'; // Representar visualmente el obstáculo
                
                printf("Obstáculo colocado en fila %d, columna %d\n", actual_lane, x);
            } else {
                printf("No se pudo colocar obstáculo en fila %d, columna %d - Sin espacio suficiente\n", actual_lane, x);
            }
        }
    }
}

// Función para mostrar el estado actual del juego
void display_game(void) {
    printf("\n=== Estado del Juego ===\n");
    for (int i = 0; i < HEIGHT_SCRE; i++) {
        for (int j = 0; j < WIDTH_SCRE; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("\n");
    }
    printf("========================\n\n");
}

// Función de ejemplo para probar el generador
int main() {
    srand(time(NULL)); // Inicializar generador de números aleatorios
    
    printf("Generando obstáculos...\n");
    generate_obstacule();
    
    display_game();
    
    return 0;
}