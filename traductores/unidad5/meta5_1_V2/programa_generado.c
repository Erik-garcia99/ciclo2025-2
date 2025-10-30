#include <stdio.h>

int main() {
    double n;
    double prom;
    double i;
    double cal;
    
    printf("cuantas calificaciones ");
    scanf("%lf", &n);
    prom = 0;
    i = 0;
    while (i<n) {
        printf("da una calificacion ");
        scanf("%lf", &cal);
        prom = prom+cal;
        i = i+1;
    }
    prom = prom/n;
    if (prom>5) {
        printf("aprobado con :  %f\n", prom);
    }
    if (prom==10) {
        printf("excelente\n");
    }
    return 0;
    }
