#include <stdio.h>
#include <limits.h>
#include "sjf.h"

void ejecutarSJF(Proceso procesos[], int n) {
    int tiempo = 0;
    int completados = 0;
    int ejecutado[n]; // marca los procesos ya ejecutados

    for (int i = 0; i < n; i++) {
        ejecutado[i] = 0;
    }

    while (completados < n) {
        int idx_min = -1;
        int bt_min = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!ejecutado[i] && procesos[i].at <= tiempo && procesos[i].bt < bt_min) {
                bt_min = procesos[i].bt;
                idx_min = i;
            }
        }

        // Si no hay procesos disponibles aún, avanzamos el tiempo
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
    }
}
