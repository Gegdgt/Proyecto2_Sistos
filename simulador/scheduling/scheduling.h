// scheduling.h
#ifndef SCHEDULING_H
#define SCHEDULING_H

/**
 * Únicamente exponemos la función principal que arranca la simulación
 * de scheduling. 
 * El tipo Proceso ya no se declara aquí, porque lo
 * dejaremos “privado” dentro de scheduling.c para evitar el choque
 * con utils/ (que también tenía su propia struct Proceso).
 */
void lanzarSimuladorScheduling(void);

#endif // SCHEDULING_H
