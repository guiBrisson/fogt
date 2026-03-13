#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "../include/raylib.h"

#include "presentation.h"
#include "asset_registry.h"

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

typedef struct Window {
    const char* title;
    int targetFPS;
} Window;

typedef struct InputEvent {
    bool nextSlide;
    bool previousSlide;
} InputEvent;

typedef struct Application {
    Window window;
    InputEvent input;
    Presentation presentation;
    RenderTexture2D target;
    AssetRegistry registry;
    size_t currentSlideIndex;
} Application;

Application* AppNew(const char* title, int width, int height);
void AppRunWindow(Application* app);
void AppLoop(Application* app);
void AppFree(Application* app);
