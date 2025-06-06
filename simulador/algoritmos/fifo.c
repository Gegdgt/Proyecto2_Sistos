#include <stdio.h>
#include "fifo.h"

// Comparador para Arrival Time
int compararAT(const void* a, const void* b) {
    Proceso* p1 = (Proceso*)a;
    Proceso* p2 = (Proceso*)b;
    return p1->at - p2->at;
}

void ejecutarFIFO(Proceso procesos[], int n) {
    qsort(procesos, n, sizeof(Proceso), compararAT);
    int tiempo = 0;

    for (int i = 0; i < n; i++) {
        if (procesos[i].at > tiempo) {
            tiempo = procesos[i].at;
        }

        procesos[i].start = tiempo;
        procesos[i].end = tiempo + procesos[i].bt;
        tiempo = procesos[i].end;

        printf("%s -> Start: %d, End: %d\n", procesos[i].pid, procesos[i].start, procesos[i].end);
    }
}
