# ------------------------------------------------
# Makefile para proyecto C con SDL2 y subcarpetas
# ------------------------------------------------

# 1. Compilador
CC      = gcc

# 2. Obtenemos las flags de compilación de SDL2 y SDL2_ttf
#    - sdl2-config --cflags  da los include paths de SDL2
#    - sdl2-config --libs    da los flags de enlace de SDL2 (-lSDL2 ...)
#    A SDL2_ttf la enlazamos con -lSDL2_ttf
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS   := $(shell sdl2-config --libs) -lSDL2_ttf

# 3. Banderas generales de compilación
#    -Wall activa todos los warnings
#    $(SDL_CFLAGS) agrega los -I/usr/include/SDL2 (u otro path)
#    -I./algoritmos -I./scheduling -I./utils para nuestros headers
CFLAGS  = -Wall $(SDL_CFLAGS) \
          -I. \
          -I./algoritmos \
          -I./scheduling \
          -I./utils

# 4. Carpetas donde hay .c/.h
SRCDIRS := algoritmos scheduling utils

# 5. Lista de todos los .c (incluyendo main.c)
SRC := main.c \
       $(wildcard $(addsuffix /*.c,$(SRCDIRS)))

# 6. Cada .c → .o (manteniendo ruta relativa)
OBJ := $(patsubst %.c,%.o,$(SRC))

# 7. Nombre del ejecutable final
TARGET := programa

# ------------------------------------------------
# 8. Regla por defecto: compilar todo
.PHONY: all
all: $(TARGET)

# 9. Cómo enlazar el ejecutable a partir de los objetos
#    Agregamos $(SDL_LIBS) para que se vincule SDL2 y SDL2_ttf
$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(SDL_LIBS) -o $@

# 10. Regla genérica para compilar cada .c en .o
#     $(dir $@) crea la carpeta si no existe (p. ej. algoritmos/)
%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 11. Limpiar objetos y ejecutable
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
