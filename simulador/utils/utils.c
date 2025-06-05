// utils.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "utils.h"

int cargarProcesos(const char* filename, Proceso procesos[]) {
    FILE* archivo = fopen(filename, "r");
    if (!archivo) return 0;

    char linea[100];
    int count = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        if (count >= MAX_PROCESOS) break;

        char pid[10];
        int bt, at, prio;

        if (sscanf(linea, "%[^,], %d, %d, %d", pid, &bt, &at, &prio) == 4) {
            strcpy(procesos[count].pid, pid);
            procesos[count].bt = bt;
            procesos[count].at = at;
            procesos[count].priority = prio;
            count++;
        }
    }

    fclose(archivo);
    return count;
}

void dibujarDiagrama(SDL_Renderer* renderer, TTF_Font* font, Proceso procesos[], int n, int y_offset) {
    for (int i = 0; i < n; i++) {
        int x = procesos[i].start * TAM_BLOQUE;
        int y = y_offset;
        int w = (procesos[i].end - procesos[i].start) * TAM_BLOQUE;

        SDL_Rect rect = {x, y, w, TAM_BLOQUE};
        SDL_SetRenderDrawColor(renderer, (100 + i*40) % 255, 100, 200, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        char texto[20];
        sprintf(texto, "%s", procesos[i].pid);
        SDL_Surface* surface = TTF_RenderText_Solid(font, texto, (SDL_Color){0, 0, 0});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = {x + 5, y + 5, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}
