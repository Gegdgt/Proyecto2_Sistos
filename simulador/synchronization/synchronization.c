#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "guiUtils.h"
#include "synchronization.h"

#define VENTANA_ANCHO 800
#define VENTANA_ALTO 600
#define MAX_PROCESOS 100
#define MAX_RECURSOS 20
#define MAX_ACCIONES 500
#define MAX_CYCLE 1000

// Definición de Checkbox compatible con scheduling.c
typedef struct {
    SDL_Rect box;
    bool activo;
    char nombre[32];
} Checkbox;

// Prototipos de funciones de checkbox (implementadas en scheduling.c)
void dibujarCheckbox(SDL_Renderer* renderer, TTF_Font* font, Checkbox* cb);
bool clickEnCheckbox(int x, int y, Checkbox* cb);

// Estructuras de datos
typedef struct {
    char pid[32];
    int burst, arrival, priority;
} Process;

typedef struct {
    char name[32];
    int capacity;
    int available;
} Resource;

typedef enum { NEW, WAITING, ACCESSING, DONE } ActionState;

typedef struct {
    char pid[32];
    char type[16];
    char resource[32];
    int cycle;
    ActionState state;
    int startCycle;
} Action;

// Datos globales
static Process procesos[MAX_PROCESOS];
static int numProcesos = 0;
static Resource recursos[MAX_RECURSOS];
static int numRecursos = 0;
static Action acciones[MAX_ACCIONES];
static int numAcciones = 0;
static bool isMutex = true;

// Auxiliares de simulación
static Action* accediendoActual[MAX_ACCIONES];
static int accessingCount = 0;
static Action* waitingQueues[MAX_RECURSOS][MAX_ACCIONES];
static int waitingCount[MAX_RECURSOS];
static Action* cycleActions[MAX_CYCLE+1][MAX_ACCIONES];
static int cycleCount[MAX_CYCLE+1];

// Prototipos internos
static void clearData(void);
static void cargarDatos(void);
static void runSimulation(void);
static int findResourceIndex(const char* name);

// Función principal expuesta
void lanzarSimuladorSynchronization(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Error inicializando SDL2: %s\n", SDL_GetError());
        return;
    }
    SDL_Window* window = SDL_CreateWindow("Synchronization Simulator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          VENTANA_ANCHO, VENTANA_ALTO, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("assets/fuente.ttf", 16);
    if (!font) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "No se pudo cargar la fuente.", window);
        return;
    }

    // Crear botones y checkboxes
    Boton btnUpload = crearBoton(renderer, font, "Upload Data", 50, 50, 150, 40);
    Boton btnRun    = crearBoton(renderer, font, "Run Simulation", 50, 110, 150, 40);
    Boton btnClear  = crearBoton(renderer, font, "Clear", 50, 170, 150, 40);
    Checkbox checks[2];
    checks[0] = (Checkbox){ .box = {250, 50, 20, 20}, .activo = true,  .nombre = "Mutex" };
    checks[1] = (Checkbox){ .box = {250, 80, 20, 20}, .activo = false, .nombre = "Semáforo" };

    bool running = true;
    SDL_Event e;
    while (running) {
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        SDL_RenderClear(renderer);

        // Dibujar UI
        dibujarBoton(renderer, btnUpload);
        dibujarBoton(renderer, btnRun);
        dibujarBoton(renderer, btnClear);
        for (int i = 0; i < 2; i++) dibujarCheckbox(renderer, font, &checks[i]);
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = false; break; }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (estaSobreBoton(x,y,btnUpload)) {
                    clearData();
                    cargarDatos();
                } else if (estaSobreBoton(x,y,btnClear)) {
                    clearData();
                    printf("[CLEAR] Datos eliminados.\n");
                } else if (estaSobreBoton(x,y,btnRun)) {
                    isMutex = checks[0].activo;
                    runSimulation();
                }
                // Toggle modo (solo uno activo)
                for (int i = 0; i < 2; i++) {
                    if (clickEnCheckbox(x,y,&checks[i])) {
                        checks[i].activo = true;
                        checks[1-i].activo = false;
                    }
                }
            }
        }
    }

    // Limpiar recursos SDL
    liberarBoton(btnUpload);
    liberarBoton(btnRun);
    liberarBoton(btnClear);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit(); SDL_Quit();
}

static void clearData(void) {
    numProcesos = numRecursos = numAcciones = 0;
    accessingCount = 0;
    memset(waitingCount, 0, sizeof(waitingCount));
}

static void cargarDatos(void) {
    char line[256];
    FILE* f;
    // Procesos
    f = fopen("procesos.txt","r");
    if (!f) { printf("Error abriendo procesos.txt\n"); return; }
    while (fgets(line, sizeof(line), f) && numProcesos < MAX_PROCESOS) {
        char* tok = strtok(line, ",\n"); if (!tok) continue;
        strncpy(procesos[numProcesos].pid, tok, 31);
        tok = strtok(NULL, ",\n"); procesos[numProcesos].burst   = tok?atoi(tok):0;
        tok = strtok(NULL, ",\n"); procesos[numProcesos].arrival = tok?atoi(tok):0;
        tok = strtok(NULL, ",\n"); procesos[numProcesos].priority= tok?atoi(tok):0;
        numProcesos++;
    }
    fclose(f);
    printf("[UPLOAD] %d procesos cargados\n", numProcesos);

    // Recursos
    f = fopen("recursos.txt","r");
    if (!f) { printf("Error abriendo recursos.txt\n"); return; }
    while (fgets(line, sizeof(line), f) && numRecursos < MAX_RECURSOS) {
        char name[32]; int cnt;
        char* tok = strtok(line, ",\n"); if (!tok) continue;
        strncpy(name, tok, 31);
        tok = strtok(NULL, ",\n"); cnt = tok?atoi(tok):0;
        recursos[numRecursos].capacity  = isMutex?1:cnt;
        recursos[numRecursos].available = recursos[numRecursos].capacity;
        strncpy(recursos[numRecursos].name, name, 31);
        numRecursos++;
    }
    fclose(f);
    printf("[UPLOAD] %d recursos cargados (modo %s)\n", numRecursos, isMutex?"Mutex":"Semáforo");

    // Acciones
    f = fopen("acciones.txt","r");
    if (!f) { printf("Error abriendo acciones.txt\n"); return; }
    while (fgets(line, sizeof(line), f) && numAcciones < MAX_ACCIONES) {
        char* tok = strtok(line, ",\n"); if (!tok) continue;
        strncpy(acciones[numAcciones].pid, tok, 31);
        tok = strtok(NULL, ",\n"); strncpy(acciones[numAcciones].type, tok?tok:"",15);
        tok = strtok(NULL, ",\n"); strncpy(acciones[numAcciones].resource, tok?tok:"",31);
        tok = strtok(NULL, ",\n"); acciones[numAcciones].cycle = tok?atoi(tok):0;
        acciones[numAcciones].state = NEW;
        numAcciones++;
    }
    fclose(f);
    printf("[UPLOAD] %d acciones cargadas\n", numAcciones);
}

static int findResourceIndex(const char* name) {
    for (int i = 0; i < numRecursos; i++)
        if (strcmp(recursos[i].name, name) == 0)
            return i;
    return -1;
}

static void runSimulation(void) {
    // Organizar acciones por ciclo
    int maxCycle = 0;
    for (int i = 0; i < numAcciones; i++) {
        int c = acciones[i].cycle;
        if (c > maxCycle) maxCycle = c;
    }
    for (int c = 0; c <= maxCycle; c++) cycleCount[c] = 0;
    for (int i = 0; i < numAcciones; i++) {
        Action* a = &acciones[i];
        a->state = NEW;
        cycleActions[a->cycle][ cycleCount[a->cycle]++ ] = a;
    }

    // Inicializar colas de espera y accesos actuales
    accessingCount = 0;
    memset(waitingCount, 0, sizeof(waitingCount));

    // Simulación ciclo a ciclo
    for (int c = 0; c <= maxCycle; c++) {
        printf("---- CICLO %d ----\n", c);
        // 1) Liberar recursos usados en ciclo anterior
        for (int i = 0; i < accessingCount; i++) {
            Action* a = accediendoActual[i];
            int ridx = findResourceIndex(a->resource);
            recursos[ridx].available++;
            a->state = DONE;
        }
        accessingCount = 0;
        // 2) Reintentar procesos en espera
        for (int r = 0; r < numRecursos; r++) {
            Resource* res = &recursos[r];
            while (res->available > 0 && waitingCount[r] > 0) {
                Action* a = waitingQueues[r][0];
                // Desencolar
                for (int k = 1; k < waitingCount[r]; k++)
                    waitingQueues[r][k-1] = waitingQueues[r][k];
                waitingCount[r]--;
                res->available--;
                a->state = ACCESSING;
                a->startCycle = c;
                accediendoActual[accessingCount++] = a;
            }
        }
        // 3) Procesar nuevas acciones de este ciclo
        for (int i = 0; i < cycleCount[c]; i++) {
            Action* a = cycleActions[c][i];
            int ridx = findResourceIndex(a->resource);
            Resource* res = &recursos[ridx];
            if (res->available > 0) {
                res->available--;
                a->state = ACCESSING;
                a->startCycle = c;
                accediendoActual[accessingCount++] = a;
            } else {
                a->state = WAITING;
                waitingQueues[ridx][ waitingCount[ridx]++ ] = a;
            }
        }
        // 4) Imprimir accesos concedidos
        bool anyAccess = false;
        for (int i = 0; i < accessingCount; i++) {
            if (accediendoActual[i]->startCycle == c) { anyAccess = true; break; }
        }
        if (anyAccess) {
            printf(" > ACCESOS concedidos:\n");
            for (int i = 0; i < accessingCount; i++) {
                Action* a = accediendoActual[i];
                if (a->startCycle == c)
                    printf("    - Proceso %s (Acción: %s) tomó Recurso %s\n",
                           a->pid, a->type, a->resource);
            }
        } else {
            printf(" > Ningún acceso concedido este ciclo.\n");
        }
        // 5) Imprimir espera
        bool anyWait = false;
        for (int r = 0; r < numRecursos; r++) if (waitingCount[r] > 0) { anyWait = true; break; }
        if (anyWait) {
            printf(" > Procesos en espera:\n");
            for (int r = 0; r < numRecursos; r++) {
                for (int i = 0; i < waitingCount[r]; i++) {
                    Action* a = waitingQueues[r][i];
                    printf("    - Proceso %s en recurso %s\n", a->pid, a->resource);
                }
            }
        } else {
            printf(" > No hay procesos en espera.\n");
        }
        printf("\n");
    }
    printf("=== Fin de la simulación (ciclo %d) ===\n", maxCycle);
}
