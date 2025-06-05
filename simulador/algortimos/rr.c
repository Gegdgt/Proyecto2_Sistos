#include <stdio.h>
#include <string.h>
#include "rr.h"

void ejecutarRR(Proceso procesos[], int n, int quantum) {
    int tiempo = 0;
    int completados = 0;
    int remaining[n];
    int started[n];
    int ejecutado[n];
    int cola[MAX_PROCESOS];
    int frente = 0, fin = 0;

    for (int i = 0; i < n; i++) {
        remaining[i] = procesos[i].bt;
        started[i] = 0;
        ejecutado[i] = 0;
    }

    // Agregar los procesos que llegan al tiempo 0
    for (int i = 0; i < n; i++) {
        if (procesos[i].at == 0)
            cola[fin++] = i;
    }

    while (completados < n) {
        if (frente == fin) {
            // no hay procesos listos, avanzar tiempo
            tiempo++;
            for (int i = 0; i < n; i++) {
                if (procesos[i].at == tiempo)
                    cola[fin++] = i;
            }
            continue;
        }

        int idx = cola[frente++];
        if (!started[idx]) {
            procesos[idx].start = tiempo;
            started[idx] = 1;
        }

        int ejecutar = (remaining[idx] < quantum) ? remaining[idx] : quantum;
        tiempo += ejecutar;
        remaining[idx] -= ejecutar;

        // revisar si llegaron nuevos procesos durante este tiempo
        for (int t = tiempo - ejecutar + 1; t <= tiempo; t++) {
            for (int i = 0; i < n; i++) {
                if (procesos[i].at == t)
                    cola[fin++] = i;
            }
        }

        if (remaining[idx] == 0) {
            procesos[idx].end = tiempo;
            ejecutado[idx] = 1;
            completados++;
            printf("%s -> Start: %d, End: %d\n", procesos[idx].pid, procesos[idx].start, procesos[idx].end);
        } else {
            cola[fin++] = idx; // reencolar el proceso
        }
    }
}
