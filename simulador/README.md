# Proyecto Simulador de Calendarización de Procesos

**Programadores:**

* Gabriel Garcia 21352
* Nelson Escalante 22046

---

## Descripción

Este simulador está diseñado para representar visualmente el funcionamiento de distintos algoritmos de calendarización de procesos en un entorno de sistemas operativos. La aplicación cuenta con una interfaz gráfica desarrollada en C utilizando SDL2 y SDL\_ttf.

Incluye soporte para los siguientes algoritmos:

* FIFO (First-In First-Out)
* SJF (Shortest Job First)
* SRT (Shortest Remaining Time)
* Round Robin (con quantum = 2)
* Priority Scheduling

El usuario puede seleccionar uno o varios algoritmos a simular, cargar procesos desde archivo, visualizar los resultados en un diagrama de Gantt, y observar las métricas como tiempo promedio de espera (WT) y turnaround (TAT).

---

## Estructura del Proyecto

```
simulador/
├── main.c
├── scheduling.c
├── scheduling.h
├── guiUtils.c
├── guiUtils.h
├── utils.c
├── utils.h
├── fifo.c
├── fifo.h
├── sjf.c
├── sjf.h
├── srt.c
├── srt.h
├── rr.c
├── rr.h
├── priority.c
├── priority.h
├── procesos.txt
├── assets/
│   └── fuente.ttf
├── Makefile
└── README.md
```

---

## Requisitos

* SDL2
* SDL2\_ttf

Para instalar en Debian/Ubuntu:

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

---

## Compilación

Desde la carpeta `simulador/`, ejecutar:

```bash
make
```

Esto generará el ejecutable `simulador`.

---

## Uso

Ejecuta el simulador con:

```bash
./simulador
```

### Interfaz Principal

Al iniciar, verás un menú con dos opciones:

* **Scheduling Simulator:** Accede a la simulación de algoritmos.
* **Synchronization Simulator:** Esta parte está destinada a otro integrante.

### Interfaz de Scheduling

Contiene:

* **Botón "Upload Processes"**: Carga procesos desde `procesos.txt`.
* **Checkboxes**: Selecciona los algoritmos que deseas simular.
* **Botón "Run Simulation"**: Ejecuta los algoritmos seleccionados y muestra los diagramas.
* **Botón "Show Info"**: Imprime en consola la información de los procesos cargados.
* **Botón "Clear"**: Limpia visualmente el área de simulación.

Los diagramas se dibujan en la ventana junto con las métricas de cada algoritmo:

* Tiempo de turnaround promedio
* Tiempo de espera promedio

---

## Formato de `procesos.txt`

Cada línea debe tener el siguiente formato:

```
PID AT BT PRIORITY
```

Por ejemplo:

```
P1 0 5 2
P2 2 3 1
P3 4 1 3
```

---

## Licencia

Este proyecto fue realizado como parte del curso de Sistemas Operativos 2025.
