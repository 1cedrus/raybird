#include <stdlib.h>

#include "include/raylib.h"
#include "include/sewer.h"

Sewer* randomSewer() {
    Sewer* sewer = (Sewer*) malloc(sizeof(Sewer));
    
    int upHeight = GetRandomValue(0, 300);
    int downHeight = 600 - upHeight - 150;

    Vector2* upSewer = (Vector2*) malloc(sizeof(Vector2)); 
    upSewer->x = DEFAULT_SEWER_WIDTH;
    upSewer->y = upHeight;

    Vector2* downSewer = (Vector2*) malloc(sizeof(Vector2));
    downSewer->x = DEFAULT_SEWER_WIDTH;
    downSewer->y = downHeight;

    sewer->upSewer = upSewer;
    sewer->downSewer = downSewer;
    sewer->xPosition = 800 + DEFAULT_SEWER_WIDTH;
    
    return sewer;
}

