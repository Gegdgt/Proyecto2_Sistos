#include <stdio.h>
#include <limits.h>
#include "priority.h"

#define ENVEJECIMIENTO_UMBRAL 5

void ejecutarPriority(Proceso procesos[], int n) {
    int tiempo = 0;
    int completados = 0;
    int ejecutado[n];
    int tiempo_espera[n];

    for (int i = 0; i < n; i++) {
        ejecutado[i] = 0;
        tiempo_espera[i] = 0;
    }

    while (completados < n) {
        int idx_min = -1;
        int prioridad_min = INT_MAX;

        // buscar proceso con mayor prioridad (menor número)
        for (int i = 0; i < n; i++) {
            if (!ejecutado[i] && procesos[i].at <= tiempo) {
                if (procesos[i].priority < prioridad_min) {
                    prioridad_min = procesos[i].priority;
                    idx_min = i;
                }
            }
        }

        // si no hay proceso disponible aún, avanzar tiempo
        if (idx_min == -1) {
            tiempo++;
            continue;
        }

        procesos[idx_min].start = tiempo;
        procesos[idx_min].end = tiempo + procesos[idx_min].bt;
        tiempo = procesos[idx_min].end;
        ejecutado[idx_min] = 1;
        completados++;

        printf("%s -> Start: %d, End: %d\n", procesos[idx_min].pid, procesos[idx_min].start, procesos[idx_min].end);

        // aplicar envejecimiento a los procesos restantes
        for (int i = 0; i < n; i++) {
            if (!ejecutado[i] && procesos[i].at <= tiempo) {
                tiempo_espera[i] += procesos[idx_min].bt;
                if (tiempo_espera[i] >= ENVEJECIMIENTO_UMBRAL) {
                    if (procesos[i].priority > 0) {
                        procesos[i].priority--;
                        printf("↑ Envejecimiento aplicado a %s -> nueva prioridad: %d\n",
                               procesos[i].pid, procesos[i].priority);
                    }
                    tiempo_espera[i] = 0;
                }
            }
        }
    }
}
