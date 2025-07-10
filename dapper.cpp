#include <iostream>
#include "raylib.h"

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool IsOnGround(AnimData data, int win_h) {
    return data.pos.y >= win_h - data.rec.height;
}

AnimData UpdateAnimation(AnimData data, float dT, int maxFrame) {
    data.runningTime += dT;
    if (data.runningTime >= data.updateTime) {
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        data.runningTime = 0.;
        data.frame = data.frame % maxFrame;
    }

    return data;
}

float ScrollBackground(Texture2D texture, float scale, float posX, float velX, float dT) {
    posX -= velX * dT;
    if (posX <= - texture.width * scale) {
        posX = 0.;
    }
    Vector2 bg1Pos {posX, 0.};
    Vector2 bg2Pos {posX + scale * texture.width, 0.};
    DrawTextureEx(texture, bg1Pos, 0., scale, WHITE);
    DrawTextureEx(texture, bg2Pos, 0., scale, WHITE);

    return posX;
}

int main() {
    const int win_h{380}, win_w{512};
    
    // Initalize
    InitWindow(win_w, win_h, "Dapper Dasher");
    SetTargetFPS(60);

    const int gravity{1800}, jumpVel{-800};
    bool inAir {false};
    
    // Scarfy Sprite
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData { 
        {0., 0., (float)(scarfy.width / 6.0), (float)scarfy.height}, // Rectangle
        {(float)(win_w / 2. - scarfy.width/ 6. / 2.), (float)(win_h - scarfy.height)}, // Position
        0,          // frame
        1./12.,     // updateTime
        0.          // runningTime
    };
    
    // Nebule Sprite
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    int numNebula = 6;
    AnimData nebulae[numNebula]{};

    for (int i = 0; i<numNebula; i++) {
        nebulae[i].rec = {0., 0., (float)(nebula.width / 8), (float)(nebula.height / 8)};
        nebulae[i].pos = {(float)(win_w + i*400), (float)(win_h - (nebula.height / 8))};
        nebulae[i].frame = 0.;
        nebulae[i].updateTime = 1./12.;
        nebulae[i].runningTime = 0.;
    }

    
    // Animation velocity
    int velocity{}, nebulaVel{-300};

    // Background Textures
    Texture2D bground = LoadTexture("textures/far-buildings.png");
    Texture2D mground = LoadTexture("textures/back-buildings.png");
    Texture2D fground = LoadTexture("textures/foreground.png");
    float bgX{}, mgX{}, fgX{};


    while (!WindowShouldClose()){
        float dT { GetFrameTime() };
        BeginDrawing();
        ClearBackground(WHITE);

        // draw bg
        bgX = ScrollBackground(bground, 2., bgX, 20, dT);
        mgX = ScrollBackground(mground, 2., mgX, 40, dT);
        fgX = ScrollBackground(fground, 2., fgX, 80, dT);
         
        // Delta time

        // Velocity Calculation
        inAir = !IsOnGround(scarfyData, win_h);
        velocity = inAir ? velocity + gravity * dT : 0;

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
        
        // Animate Nebulae
        for (int i = 0; i<numNebula; i++) {
            nebulae[i].pos.x += nebulaVel * dT;
            nebulae[i] = UpdateAnimation(nebulae[i], dT, 8);
            DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
        }

        // Draw textures
        DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);

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