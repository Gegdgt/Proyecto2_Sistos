#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MAX_PROCESOS 100
#define TAM_BLOQUE 30

// Estructura para representar un proceso
typedef struct {
    char pid[10];
    int bt;         // Burst Time
    int at;         // Arrival Time
    int priority;
    int start;      // Tiempo de inicio
    int end;        // Tiempo de finalización
} Proceso;

// Carga procesos desde archivo, retorna cantidad
int cargarProcesos(const char* filename, Proceso procesos[]);

// Dibuja el diagrama de Gantt en la posición vertical especificada
void dibujarDiagrama(SDL_Renderer* renderer, TTF_Font* font, Proceso procesos[], int n, int y_offset);

#endif
