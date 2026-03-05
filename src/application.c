#include <stdlib.h>

#include "application.h"
#include "../include/raylib.h"

Application* AppNew(const char* title, int width, int height) {
    Application* app = malloc(sizeof(Application));
    app->window = (Window){
        .title = title,
        .targetFPS = 60,
    };
    app->slideWidth = width;
    app->slideHeight = height;

    return app;
}

void AppRunWindow(Application* app) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(app->slideWidth, app->slideHeight, app->window.title);
    SetWindowMinSize(320, 240);
    SetTargetFPS(app->window.targetFPS);
    app->target = LoadRenderTexture(app->slideWidth, app->slideHeight);
}

// static void AppNextSlide(Application* app) {
    
// }

// static void AppPreviousSlide(Application* app) {

// }

// static void AppUpdateCurrentSlide(Application* app) {

// }

static void AppProcessInputs(Application* app) {
    app->input = (InputEvent){
        .nextSlide = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_RIGHT),
        .previousSlide = IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_LEFT),
    };

    // if (app->input.nextSlide) AppNextSlide(app);
    // if (app->input.previousSlide) AppPreviousSlide(app);
}

// static void AppUpdate(Application* app) {
    // ApplicationUpdateCurrentSlide(app);
// }

static void AppDraw(Application* app) {
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float scale = MIN(screenWidth / app->slideWidth, screenHeight / app->slideHeight);

    BeginTextureMode(app->target);
    // TODO: call the renderer to draw the current slide here
    EndTextureMode();

    BeginDrawing();
    {
        ClearBackground(BLACK);
        Rectangle source = {
            .x = 0.0, .y = 0.0,
            .width = (float)app->target.texture.width,
            .height = (float)-app->target.texture.height
        };

        Rectangle destination = {
            .x = (screenWidth - ((float)app->slideWidth * scale)) * 0.5,
            .y = (screenHeight - ((float)app->slideHeight * scale)) * 0.5,
            .width = (float)app->slideWidth * scale,
            .height = (float)app->slideHeight * scale,
        };

        // Draw render texture to screen, properly scaled
        DrawTexturePro(
            app->target.texture, 
            source, 
            destination, 
            (Vector2){0, 0}, 
            0.0, 
            WHITE
        );
    }
    EndDrawing();
}

void AppLoop(Application* app) {
    while (!WindowShouldClose()) {
        AppProcessInputs(app);
        // AppUpdate(app);
        AppDraw(app);
    }
}

void AppFree(Application* app) {
    UnloadRenderTexture(app->target);
    CloseWindow();
    free(app);
    app = NULL;
}

