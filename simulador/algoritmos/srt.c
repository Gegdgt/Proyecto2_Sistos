#include <stdio.h>
#include <limits.h>
#include "srt.h"

void ejecutarSRT(Proceso procesos[], int n) {
    int tiempo = 0;
    int completados = 0;
    int remaining[n];
    int started[n];
    int ejecutado[n];

    for (int i = 0; i < n; i++) {
        remaining[i] = procesos[i].bt;
        started[i] = 0;
        ejecutado[i] = 0;
    }

    int actual = -1;

    while (completados < n) {
        int min_rem = INT_MAX;
        int idx_min = -1;

        for (int i = 0; i < n; i++) {
            if (!ejecutado[i] && procesos[i].at <= tiempo && remaining[i] > 0 && remaining[i] < min_rem) {
                min_rem = remaining[i];
                idx_min = i;
            }
        }

        if (idx_min == -1) {
            tiempo++;
            continue;
        }

        if (!started[idx_min]) {
            procesos[idx_min].start = tiempo;
            started[idx_min] = 1;
        }

        remaining[idx_min]--;
        tiempo++;

        if (remaining[idx_min] == 0) {
            procesos[idx_min].end = tiempo;
            ejecutado[idx_min] = 1;
            completados++;
            printf("%s -> Start: %d, End: %d\n", procesos[idx_min].pid, procesos[idx_min].start, procesos[idx_min].end);
        }
    }
}
