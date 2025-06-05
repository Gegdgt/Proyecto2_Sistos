#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include "guiUtils.h"

Boton crearBoton(SDL_Renderer* renderer, TTF_Font* font, const char* texto, int x, int y, int w, int h) {
    Boton boton;
    boton.rect.x = x;
    boton.rect.y = y;
    boton.rect.w = w;
    boton.rect.h = h;

    boton.color = (SDL_Color){ 200, 200, 200, 255 };  // gris claro
    boton.textColor = (SDL_Color){ 0, 0, 0, 255 };    // negro
    strncpy(boton.texto, texto, sizeof(boton.texto) - 1);

    // Crear textura de texto
    SDL_Surface* surface = TTF_RenderText_Blended(font, boton.texto, boton.textColor);
    boton.textura = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return boton;
}

void dibujarBoton(SDL_Renderer* renderer, Boton boton) {
    // Dibujar fondo del botón
    SDL_SetRenderDrawColor(renderer, boton.color.r, boton.color.g, boton.color.b, boton.color.a);
    SDL_RenderFillRect(renderer, &boton.rect);

    // Dibujar borde
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &boton.rect);

    // Dibujar texto centrado
    int texW = 0, texH = 0;
    SDL_QueryTexture(boton.textura, NULL, NULL, &texW, &texH);

    SDL_Rect textoRect = {
        boton.rect.x + (boton.rect.w - texW) / 2,
        boton.rect.y + (boton.rect.h - texH) / 2,
        texW,
        texH
    };
    SDL_RenderCopy(renderer, boton.textura, NULL, &textoRect);
}

bool estaSobreBoton(int x, int y, Boton boton) {
    return x >= boton.rect.x && x <= boton.rect.x + boton.rect.w &&
           y >= boton.rect.y && y <= boton.rect.y + boton.rect.h;
}

void liberarBoton(Boton boton) {
    if (boton.textura != NULL) {
        SDL_DestroyTexture(boton.textura);
    }
}
