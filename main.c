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
#define DEATH_POSITION(BASE_HEIGHT) WINDOW_HEIGHT - 50 - BASE_HEIGHT 

int freeSewer(Sewer* sewer);
int freePump(Sewer** pump, int *size);

int main() {
    InitAudioDevice();
    SetMasterVolume(0.1);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raybird"); 
    SetTargetFPS(60);     

    Sound wingSound = LoadSound("audio/wing.ogg");
    Sound pointSound = LoadSound("audio/point.ogg");
    Sound dieSound = LoadSound("audio/die.ogg");
    Sound hitSewerSound = LoadSound("audio/hit.ogg");
    Sound swooshSound = LoadSound("audio/swoosh.ogg");

    Texture background = LoadTexture("sprites/background-day.png");
    Texture base = LoadTexture("sprites/base.png");
    Texture sewer = LoadTexture("sprites/pipe-green.png");

    Texture birdUpFlap = LoadTexture("sprites/yellowbird-upflap.png");
    Texture birdMidFlap = LoadTexture("sprites/yellowbird-midflap.png");
    Texture birdDownFlap = LoadTexture("sprites/yellowbird-downflap.png");

    Texture birdFlap[3] = {birdUpFlap, birdMidFlap, birdDownFlap};

    Vector2 position = {100, 300};
    Vector2 size = {51, 36};

    float speed = 0;
    float acceleration = -800;

    uint16_t point = 0;
    char pointStr[50];

    Sewer* pump[5] = {0};
    int pumpSize = 0;

    bool isStart = false;
    bool alreadyCount = false;
    bool isDie = false;

    float scrollingBack = 0.0f;
    float scrollingBase = 0.0f;

    float birdRotation = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(BLACK);
    
        if (!isDie) {
            scrollingBack -= 0.1f;
            scrollingBase -= 150 * dt;
        }

        if (scrollingBack <= -background.width * 0.3f) scrollingBack = 0.0f;
        if (scrollingBase <= -base.width) scrollingBase = 0.0f;

        DrawTextureEx(background, (Vector2){ scrollingBack, 0.0f }, 0.0f, 1.2f, WHITE);
        DrawTextureEx(background, (Vector2){ background.width + scrollingBack, 0.0f }, 0.0f, 1.2f, WHITE);

        if (!isStart) {
            if (IsKeyPressed(KEY_SPACE)) {
                isStart = true;
            }

            DrawText("Press space to start!", 75, 300, 32, WHITE);
        } else {
            speed -= acceleration * dt;   

            position.y += speed * dt;
            position.y = fminf(position.y, DEATH_POSITION(base.height));

            birdRotation = fminf(birdRotation + 90.0f * dt, 90.0f);

            if (position.y == DEATH_POSITION(base.height)) {
                isDie = true;
            }

            Rectangle birdRec = {
                position.x, position.y, size.x, size.y
            };

            if (isDie) {
                for (int i = 0; i < pumpSize; i += 1) {
                    DrawTextureEx(sewer, (Vector2){pump[i]->xPosition + sewer.width * 2, pump[i]->upSewer->y}, 180.0f, 2.0f, WHITE);
                    DrawTextureEx(sewer, (Vector2){pump[i]->xPosition, 600 - pump[i]->downSewer->y}, 0.0f, 2.0f, WHITE);
                }

                if (position.y == DEATH_POSITION(base.height)) {
                    if (IsKeyPressed(KEY_SPACE)) {
                        PlaySound(swooshSound);

                        freePump(pump, &pumpSize);

                        speed = -250;
                        point = 0;
                        position.y = 300;

                        isDie = false;
                        birdRotation = -30.0f;
                        alreadyCount = false;
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
                    birdRotation = -30.0f;
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

                    //DrawRectangleRec(sewerUp, GREEN);
                    DrawTextureEx(sewer, (Vector2){pump[i]->xPosition + sewer.width * 2, pump[i]->upSewer->y}, 180.0f, 2.0f, WHITE);

                    Vector2 positionDown = {
                        pump[i]->xPosition, 600 - pump[i]->downSewer->y
                    };

                    Rectangle sewerDown = {
                        pump[i]->xPosition, 600 - pump[i]->downSewer->y, pump[i]->downSewer->x, pump[i]->downSewer->y - base.height 
                    };

                    //DrawRectangleRec(sewerDown, GREEN);
                    DrawTextureEx(sewer, (Vector2){pump[i]->xPosition, 600 - pump[i]->downSewer->y}, 0.0f, 2.0f, WHITE);

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

            DrawTexturePro(
                birdFlap[(int) (GetTime() * 10) % 3],
                (Rectangle){0, 0, birdMidFlap.width, birdMidFlap.height},
                (Rectangle){position.x + birdRec.width / 2, position.y + birdRec.height / 2, birdRec.width, birdRec.height}, 
                (Vector2) {birdRec.width / 2, birdRec.height / 2},                            
                birdRotation,
                WHITE       
            );
        }

        DrawTextureEx(base, (Vector2){ scrollingBase, 600 - base.height }, 0.0f, 1.6f, WHITE);
        DrawTextureEx(base, (Vector2){ base.width + scrollingBase - 30, 600 - base.height }, 0.0f, 1.6f, WHITE);

        EndDrawing();
    }

    UnloadSound(wingSound);
    UnloadSound(pointSound);
    UnloadSound(dieSound);
    UnloadSound(hitSewerSound);
    UnloadSound(swooshSound);

    UnloadTexture(background);
    UnloadTexture(base);
    UnloadTexture(sewer);   
    UnloadTexture(birdUpFlap);
    UnloadTexture(birdMidFlap);
    UnloadTexture(birdDownFlap);

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
