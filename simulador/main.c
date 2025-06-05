// main.c
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "guiUtils.h"
#include "scheduling.h"
#include "utils.h"
#include "fifo.h"
#include "sjf.h"
#include "srt.h"
#include "rr.h"
#include "priority.h"

#define VENTANA_ANCHO 400
#define VENTANA_ALTO 200

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Process Simulator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          VENTANA_ANCHO, VENTANA_ALTO, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("assets/fuente.ttf", 20);
    if (!font) {
        printf("Error cargando fuente.\n");
        return 1;
    }

    Boton boton_scheduling = crearBoton(renderer, font, "Scheduling Simulator", 100, 50, 200, 40);
    Boton boton_sync = crearBoton(renderer, font, "Synchronization Simulator", 100, 110, 200, 40);

    bool corriendo = true;
    SDL_Event e;

    while (corriendo) {
        SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
        SDL_RenderClear(renderer);

        dibujarBoton(renderer, boton_scheduling);
        dibujarBoton(renderer, boton_sync);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                corriendo = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                if (estaSobreBoton(x, y, boton_scheduling)) {
                    lanzarSimuladorScheduling();
                } else if (estaSobreBoton(x, y, boton_sync)) {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                             "Sincronización",
                                             "Esta parte será implementada por otro integrante.",
                                             window);
                }
            }
        }
    }

    liberarBoton(boton_scheduling);
    liberarBoton(boton_sync);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
