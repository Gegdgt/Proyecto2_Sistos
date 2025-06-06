#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MAX_PROCESOS 100
#define TAM_BLOQUE 30

typedef struct {
    char pid[32];
    int  at;
    int  bt;
    int  prioridad;
    int  start;
    int  end;
    int  turnaround_time;
    int  waiting_time;
} Proceso;

// Carga procesos desde archivo, retorna cantidad
int cargarProcesos(const char* filename, Proceso procesos[]);

// Dibuja el diagrama de Gantt en la posición vertical especificada
void dibujarDiagrama(SDL_Renderer* renderer, TTF_Font* font, Proceso procesos[], int n, int y_offset);

#endif
