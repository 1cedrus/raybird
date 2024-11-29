#include <stdio.h>
#include <stdlib.h>

#include "include/raylib.h"
#include "include/sewer.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 600

int main() {
    InitAudioDevice();
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raybird"); 
    SetTargetFPS(60);     
    Vector2 p = {100, 300};
    Vector2 size = {50, 50};
    float speed = 0;
    float a = -800;
    int point = 0;
    char pointText[10];

    Sound wing = LoadSound("audio/wing.wav");
    Sound hit = LoadSound("audio/point.wav");

    Sewer* pump[10] = {0};
    int pumpSize = 0;

    int isCount = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(BLACK);
        speed -= a * dt;   

        if (IsKeyPressed(KEY_SPACE)) {
            PlaySound(wing);
            speed = -250;
        }

        p.y += speed * dt;
        
        Rectangle rec = {
            p.x, p.y, size.x, size.y
        };

        DrawRectangleRec(rec, RED);
        
        if (!pumpSize || pump[pumpSize - 1]->xPosition < 800 - DEFAULT_SEWER_WIDTH * 2.2) {
            Sewer* sewer = randomSewer();
            pump[pumpSize++] = sewer;
        } 

        if (pumpSize && pump[0]->xPosition <= 0 && !isCount) {
            point += 1;
            PlaySound(hit);

            isCount = 1;
        }

        if (pumpSize && pump[0]->xPosition < -DEFAULT_SEWER_WIDTH) {
            free(pump[0]->upSewer);
            free(pump[0]->downSewer);
            free(pump[0]);

            for (int i = 0; i < pumpSize - 1; i++) {
                pump[i] = pump[i + 1];
            }

            pumpSize -= 1;
            isCount = 0;
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

            if (CheckCollisionRecs(rec, sewerUp) || CheckCollisionRecs(rec, sewerDown)) {
                for (int i = 0; i < pumpSize - 1; i++) {
                    free(pump[i]->upSewer);
                    free(pump[i]->downSewer);
                    free(pump[i]);
                }

                point = 0;
                pumpSize = 0;
            }
        }

        sprintf(pointText, "%d", point);
        DrawText(pointText, 25, 25, 32, WHITE);

        EndDrawing();
    }

    CloseWindow(); 
    return 0;
}



