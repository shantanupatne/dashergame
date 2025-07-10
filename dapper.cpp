#include <iostream>
#include "raylib.h"

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};
const int win_h{380}, win_w{512}, offset{64};

bool IsOnGround(AnimData data, int win_h) {
    return data.pos.y >= win_h + offset - 16 - data.rec.height;
}

AnimData ResetScarfy(Texture2D scarfy) {
    AnimData data = { 
        {0., 0., (float)(scarfy.width / 6.0), (float)scarfy.height}, // Rectangle
        {(float)(win_w / 2. - scarfy.width/ 6. / 2.), (float)(win_h + offset - 16 - scarfy.height)}, // Position
        0,          // frame
        1./12.,     // updateTime
        0.          // runningTime
    };
    return data;
}
void ResetNebulae(AnimData *nebulae, int numNebula, Texture2D nebula) {
    for (int i = 0; i<numNebula; i++) {
        nebulae[i].rec = {0., 0., (float)(nebula.width / 8), (float)(nebula.height / 8)};
        nebulae[i].pos = {(float)(win_w + i*400), (float)(win_h + offset - 16 - (nebula.height / 8))};
        nebulae[i].frame = 0.;
        nebulae[i].updateTime = 1./12.;
        nebulae[i].runningTime = 0.;
    }
}   

AnimData UpdateAnimation(AnimData data, float dT, int maxFrame) {
    data.runningTime += dT;
    if (data.runningTime >= data.updateTime) {
        data.rec.x = data.frame * data.rec.width;
        if (data.pos.x <= -data.rec.width) {
            data.pos.x = win_w + 600;
        }
        data.frame++;
        data.runningTime = 0.;
        data.frame = data.frame % maxFrame;
    }

    return data;
}

float ScrollBackground(Texture2D texture, float scale, float posX, float velX, float dT) {
    posX -= velX * dT;
    if (posX <= -(texture.width * scale)) {
        posX = 0;
    }
    Vector2 bg1Pos {posX, offset - 16};
    Vector2 bg2Pos {posX + scale * texture.width, offset - 16};
    Vector2 bg3Pos {posX + 2 * scale * texture.width, offset - 16};
    DrawTextureEx(texture, bg1Pos, 0., scale, WHITE);
    DrawTextureEx(texture, bg2Pos, 0., scale, WHITE);
    DrawTextureEx(texture, bg3Pos, 0., scale, WHITE);

    return posX;
}

int main() {
    
    // Initalize
    InitWindow(2*offset + win_w, 2*offset + win_h, "Dapper Dash");
    SetTargetFPS(60);

    const int gravity{1900}, jumpVel{-900};
    bool inAir {false};
    
    // Scarfy Sprite
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData {ResetScarfy(scarfy)};
    
    // Nebule Sprite
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    int numNebula = 3;
    AnimData nebulae[numNebula]{};
    ResetNebulae(nebulae, numNebula, nebula);
    
    // Animation velocity
    int velocity{}, nebulaVel{-300};

    // Background Textures
    Texture2D bground = LoadTexture("textures/far-buildings.png");
    Texture2D mground = LoadTexture("textures/back-buildings.png");
    Texture2D fground = LoadTexture("textures/foreground.png");
    float bgX{}, mgX{}, fgX{};
    int score{};
    bool collision{};
    int state = 1;

    while (!WindowShouldClose()){
        float dT { GetFrameTime() };
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleLinesEx({0., offset - 24, win_w + 2*offset, win_h + 20}, 8, WHITE);
        // draw bg
        bgX = ScrollBackground(bground, 2., bgX, 20, dT);
        mgX = ScrollBackground(mground, 2., mgX, 40, dT);
        fgX = ScrollBackground(fground, 2., fgX, 80, dT);

        // Velocity Calculation
        inAir = !IsOnGround(scarfyData, win_h);
        velocity = inAir ? velocity + gravity * dT : 0;

        if (IsKeyPressed(KEY_ENTER) && state == 0) {
            state = 1;
            collision = false;
            score = 0;
            scarfyData = ResetScarfy(scarfy);
            ResetNebulae(nebulae, numNebula, nebula);
        }

        // Jump
        if (IsKeyPressed(KEY_SPACE) && !inAir) {
            velocity += jumpVel;
        }

        // Position Update
        scarfyData.pos.y += velocity * dT;
        
        // Animate scarfy
        if (!inAir) {
            scarfyData = UpdateAnimation(scarfyData, dT, 6);
        }

        if (state == 1) {
            for (AnimData nebula: nebulae) {
                int pad{30};
                Rectangle nebRec {
                    nebula.pos.x + pad,
                    nebula.pos.y + pad,
                    nebula.rec.width - 2*pad,
                    nebula.rec.height - 2*pad
                };
                Rectangle scarfyRec {
                    scarfyData.pos.x,
                    scarfyData.pos.y,
                    scarfyData.rec.width,
                    scarfyData.rec.height,
                };

                collision = CheckCollisionRecs(nebRec, scarfyRec);
                if (collision) break;
            }
        }
            
        if (collision) {
            state = 0;
            DrawText("GAME OVER.", 15, win_h + offset - 70, 30, WHITE);
            DrawText("Press [Enter] to restart..", 15, win_h + offset - 40, 20, WHITE);
        } else {
            // Animate Nebulae
            for (int i = 0; i<numNebula; i++) {
                nebulae[i].pos.x += nebulaVel * dT;
                nebulae[i] = UpdateAnimation(nebulae[i], dT, 8);
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            // Draw textures
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
            score++;
        }

        DrawText(TextFormat("%i", score), 10, 10, 20, WHITE);
        DrawText("Dapper Dash", win_w / 2 - 120, win_h + offset - 4, 60, WHITE);
        DrawText("Press [Space] to jump", win_w / 2 + 150, 10, 20, WHITE);

        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(bground);
    UnloadTexture(mground);
    UnloadTexture(fground);
    UnloadTexture(nebula);
    CloseWindow();
    return 0;
}