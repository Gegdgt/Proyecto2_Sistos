// scheduling.c
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "guiUtils.h"
#include "scheduling.h"
#include "utils.h"
#include "fifo.h"
#include "sjf.h"
#include "srt.h"
#include "rr.h"
#include "priority.h"

#define VENTANA_ANCHO 1000
#define VENTANA_ALTO 700
#define CHECKBOX_TAM 20
#define MAX_PROCESOS 100
#define QUANTUM_RR 2

Proceso procesos_global[MAX_PROCESOS];
int num_procesos_global = 0;

typedef struct {
    SDL_Rect box;
    bool activo;
    char nombre[32];
} Checkbox;

void mostrarMetricas(SDL_Renderer* renderer, TTF_Font* font, Proceso procesos[], int n, int y, const char* algoritmo) {
    float avg_tat = 0, avg_wt = 0;
    for (int i = 0; i < n; i++) {
        avg_tat += procesos[i].turnaround_time;
        avg_wt += procesos[i].waiting_time;
    }
    avg_tat /= n;
    avg_wt /= n;

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%s -> Avg TAT: %.2f, Avg WT: %.2f", algoritmo, avg_tat, avg_wt);

    SDL_Surface* surface = TTF_RenderText_Solid(font, buffer, (SDL_Color){0, 0, 0});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {20, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void dibujarCheckbox(SDL_Renderer* renderer, TTF_Font* font, Checkbox* cb) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &cb->box);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &cb->box);
    if (cb->activo) {
        SDL_RenderDrawLine(renderer, cb->box.x, cb->box.y, cb->box.x + CHECKBOX_TAM, cb->box.y + CHECKBOX_TAM);
        SDL_RenderDrawLine(renderer, cb->box.x + CHECKBOX_TAM, cb->box.y, cb->box.x, cb->box.y + CHECKBOX_TAM);
    }
    SDL_Surface* label = TTF_RenderText_Solid(font, cb->nombre, (SDL_Color){0, 0, 0});
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, label);
    SDL_Rect label_rect = {cb->box.x + CHECKBOX_TAM + 5, cb->box.y, label->w, label->h};
    SDL_RenderCopy(renderer, tex, NULL, &label_rect);
    SDL_FreeSurface(label);
    SDL_DestroyTexture(tex);
}

bool clickEnCheckbox(int x, int y, Checkbox* cb) {
    return x >= cb->box.x && x <= cb->box.x + cb->box.w &&
           y >= cb->box.y && y <= cb->box.y + cb->box.h;
}

void lanzarSimuladorScheduling() {
    SDL_Window* ventana = SDL_CreateWindow("Scheduling Simulator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          VENTANA_ANCHO, VENTANA_ALTO, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("assets/fuente.ttf", 16);

    if (!font) {
        printf("No se pudo cargar la fuente.\n");
        return;
    }

    Boton boton_upload = crearBoton(renderer, font, "Upload Processes", 20, 20, 160, 30);
    Boton boton_info = crearBoton(renderer, font, "Show Info", 200, 20, 120, 30);
    Boton boton_clear = crearBoton(renderer, font, "Clear", 340, 20, 100, 30);
    Boton boton_run = crearBoton(renderer, font, "Run Simulation", 460, 20, 160, 30);

    Checkbox checks[5];
    const char* nombres_algoritmos[] = {"FIFO", "SJF", "SRT", "RR", "Priority"};
    for (int i = 0; i < 5; i++) {
        checks[i].box.x = 20 + i * 110;
        checks[i].box.y = 70;
        checks[i].box.w = CHECKBOX_TAM;
        checks[i].box.h = CHECKBOX_TAM;
        checks[i].activo = true;
        strcpy(checks[i].nombre, nombres_algoritmos[i]);
    }

    SDL_Event e;
    bool corriendo = true;

    while (corriendo) {
        SDL_SetRenderDrawColor(renderer, 200, 225, 255, 255);
        SDL_RenderClear(renderer);

        dibujarBoton(renderer, boton_upload);
        dibujarBoton(renderer, boton_info);
        dibujarBoton(renderer, boton_clear);
        dibujarBoton(renderer, boton_run);

        for (int i = 0; i < 5; i++) {
            dibujarCheckbox(renderer, font, &checks[i]);
        }

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) corriendo = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                if (estaSobreBoton(x, y, boton_upload)) {
                    num_procesos_global = cargarProcesos("procesos.txt", procesos_global);
                    if (num_procesos_global > 0) {
                        printf("[UPLOAD] %d procesos cargados exitosamente.\n", num_procesos_global);
                    } else {
                        printf("[UPLOAD] Error al cargar procesos.\n");
                    }
                } else if (estaSobreBoton(x, y, boton_info)) {
                    printf("[INFO] Mostrar info de procesos...\n");
                    for (int i = 0; i < num_procesos_global; i++) {
                        printf("PID: %s, AT: %d, BT: %d, PRIORITY: %d\n",
                               procesos_global[i].pid,
                               procesos_global[i].at,
                               procesos_global[i].bt,
                               procesos_global[i].prioridad);
                    }
                } else if (estaSobreBoton(x, y, boton_clear)) {
                    printf("[CLEAR] Limpiar pantalla...\n");
                    num_procesos_global = 0;
                } else if (estaSobreBoton(x, y, boton_run)) {
                    printf("[RUN] Ejecutar simulación con algoritmos seleccionados...\n");

                    Proceso copia[MAX_PROCESOS];
                    for (int i = 0; i < 5; i++) {
                        if (!checks[i].activo) continue;
                        memcpy(copia, procesos_global, sizeof(Proceso) * num_procesos_global);
                        switch (i) {
                            case 0: ejecutarFIFO(copia, num_procesos_global); break;
                            case 1: ejecutarSJF(copia, num_procesos_global); break;
                            case 2: ejecutarSRT(copia, num_procesos_global); break;
                            case 3: ejecutarRR(copia, num_procesos_global, QUANTUM_RR); break;
                            case 4: ejecutarPriority(copia, num_procesos_global); break;
                        }
                        dibujarDiagrama(renderer, font, copia, num_procesos_global, 150 + i * 100);
                        mostrarMetricas(renderer, font, copia, num_procesos_global, 150 + i * 100 + 60, nombres_algoritmos[i]);
                    }
                }

                for (int i = 0; i < 5; i++) {
                    if (clickEnCheckbox(x, y, &checks[i])) {
                        checks[i].activo = !checks[i].activo;
                    }
                }
            }
        }
    }

    liberarBoton(boton_upload);
    liberarBoton(boton_info);
    liberarBoton(boton_clear);
    liberarBoton(boton_run);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
}
