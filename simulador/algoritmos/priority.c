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

        // Buscar proceso con mayor prioridad (menor número)
        for (int i = 0; i < n; i++) {
            if (!ejecutado[i] && procesos[i].at <= tiempo) {
                if (procesos[i].prioridad < prioridad_min) {
                    prioridad_min = procesos[i].prioridad;
                    idx_min = i;
                }
            }
        }

        // Si no hay proceso disponible aún, avanzar tiempo
        if (idx_min == -1) {
            tiempo++;
            continue;
        }

        procesos[idx_min].start = tiempo;
        procesos[idx_min].end = tiempo + procesos[idx_min].bt;
        tiempo = procesos[idx_min].end;
        ejecutado[idx_min] = 1;
        completados++;

        // Cálculo de métricas para GUI
        procesos[idx_min].turnaround_time = procesos[idx_min].end - procesos[idx_min].at;
        procesos[idx_min].waiting_time = procesos[idx_min].turnaround_time - procesos[idx_min].bt;

        // Aplicar envejecimiento a los procesos restantes
        for (int i = 0; i < n; i++) {
            if (!ejecutado[i] && procesos[i].at <= tiempo) {
                tiempo_espera[i] += procesos[idx_min].bt;
                if (tiempo_espera[i] >= ENVEJECIMIENTO_UMBRAL && procesos[i].prioridad > 0) {
                    procesos[i].prioridad--;
                    tiempo_espera[i] = 0;
                }
            }
        }
    }
}
