#ifndef GUIUTILS_H
#define GUIUTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

// Estructura de un botón simple
typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color textColor;
    char texto[64];
    SDL_Texture* textura;
} Boton;

// Crea un botón con texto, posición y tamaño
Boton crearBoton(SDL_Renderer* renderer, TTF_Font* font, const char* texto, int x, int y, int w, int h);

// Dibuja un botón en pantalla
void dibujarBoton(SDL_Renderer* renderer, Boton boton);

// Verifica si las coordenadas (x, y) están sobre el botón
bool estaSobreBoton(int x, int y, Boton boton);

// Libera recursos de botón
void liberarBoton(Boton boton);

#endif
