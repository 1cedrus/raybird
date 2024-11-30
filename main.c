#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "include/raylib.h"
#include "include/sewer.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 600
#define DEATH_POSITION 550

int freeSewer(Sewer* sewer);
int freePump(Sewer** pump, int *size);

int main() {
    InitAudioDevice();
    SetMasterVolume(0.1);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raybird"); 
    SetTargetFPS(60);     

    Sound wingSound = LoadSound("audio/wing.wav");
    Sound pointSound = LoadSound("audio/point.wav");
    Sound dieSound = LoadSound("audio/die.wav");
    Sound hitSewerSound = LoadSound("audio/hit.wav");
    Sound swooshSound = LoadSound("audio/swoosh.wav");

    Vector2 position = {100, 300};
    Vector2 size = {50, 50};

    float speed = 0;
    float acceleration = -800;

    uint16_t point = 0;
    char pointStr[50];

    Sewer* pump[5] = {0};
    int pumpSize = 0;

    bool isStart = false;
    bool alreadyCount = false;
    bool isDie = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(BLACK);

        if (!isStart) {
            if (IsKeyPressed(KEY_SPACE)) {
                isStart = true;
            }

            DrawText("Press space to start!", 75, 300, 32, WHITE);
        } else {
            speed -= acceleration * dt;   

            position.y += speed * dt;
            position.y = fminf(position.y, DEATH_POSITION);

            if (position.y == DEATH_POSITION) {
                isDie = true;
            }

            Rectangle birdRec = {
                position.x, position.y, size.x, size.y
            };

            if (isDie) {
                for (int i = 0; i < pumpSize; i += 1) {
                    Rectangle sewerUp = {
                        pump[i]->xPosition, 0, pump[i]->upSewer->x, pump[i]->upSewer->y
                    };

                    DrawRectangleRec(sewerUp, GREEN);

                    Vector2 positionDown = {
                        pump[i]->xPosition, 600 - pump[i]->downSewer->y
                    };

                    Rectangle sewerDown = {
                        pump[i]->xPosition, 600 - pump[i]->downSewer->y,pump[i]->downSewer->x, pump[i]->downSewer->y
                    };

                    DrawRectangleRec(sewerDown, GREEN);
                }

                if (position.y == DEATH_POSITION) {
                    if (IsKeyPressed(KEY_SPACE)) {
                        PlaySound(swooshSound);

                        freePump(pump, &pumpSize);

                        speed = -250;
                        point = 0;
                        position.y = 300;

                        isDie = false;
                    }

                    DrawText("Game Over!", 75, 200, 64, WHITE);
                    sprintf(pointStr, "Your point is %d", point);
                    DrawText(pointStr, 125, 280, 32, WHITE);
                    DrawText("Press space to start again!", 25, 330, 32, WHITE);
                }
            } else {
                if (IsKeyPressed(KEY_SPACE)) {
                    PlaySound(wingSound);
                    speed = -250;
                }

                if (!pumpSize || pump[pumpSize - 1]->xPosition < 800 - DEFAULT_SEWER_WIDTH * 2.2) {
                    pump[pumpSize++] = randomSewer();
                } 

                if (pumpSize && pump[0]->xPosition <= 50 && !alreadyCount) {
                    PlaySound(pointSound);
                    point += 1;
                    alreadyCount = true;
                }

                if (pumpSize && pump[0]->xPosition < -DEFAULT_SEWER_WIDTH) {
                    freeSewer(pump[0]);

                    // Shift the array to the left
                    for (int i = 0; i < pumpSize - 1; i++) {
                        pump[i] = pump[i + 1];
                    }
                    pumpSize -= 1;

                    alreadyCount = false;
                }

                for (int i = 0; i < pumpSize; i += 1) {
                    Rectangle sewerUp = {
                        pump[i]->xPosition, 0, pump[i]->upSewer->x, pump[i]->upSewer->y
                    };

                    DrawRectangleRec(sewerUp, GREEN);

                    Vector2 positionDown = {
                        pump[i]->xPosition, 600 - pump[i]->downSewer->y
                    };

                    Rectangle sewerDown = {
                        pump[i]->xPosition, 600 - pump[i]->downSewer->y,pump[i]->downSewer->x, pump[i]->downSewer->y
                    };

                    DrawRectangleRec(sewerDown, GREEN);

                    pump[i]->xPosition -= 150 * dt;

                    if (CheckCollisionRecs(birdRec, sewerUp) || CheckCollisionRecs(birdRec, sewerDown)) {
                        isDie = 1;

                        PlaySound(hitSewerSound); 
                        PlaySound(dieSound);
                    }
                }

                sprintf(pointStr, "%d", point);
                DrawText(pointStr, 25, 25, 32, WHITE);
            }

            DrawRectangleRec(birdRec, RED);
        }

        EndDrawing();
    }

    CloseWindow(); 
    return 0;
}

int freeSewer(Sewer* sewer) {
    assert(sewer != NULL);
    free(sewer->upSewer);
    free(sewer->downSewer);
    free(sewer);
    return 1;
}

int freePump(Sewer** pump, int *size) {
    assert(pump != NULL && size != NULL);
    while ((*size) && freeSewer(pump[--(*size)])) {}
    return 1;
}
