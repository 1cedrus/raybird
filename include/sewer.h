#ifndef SEWER_H
#define SEWER_H

#include "raylib.h"

#define DEFAULT_SEWER_WIDTH 100

typedef struct Sewer {
    Vector2* upSewer;    
    Vector2* downSewer; 
    float xPosition;
} Sewer;

Sewer* randomSewer();

#endif
