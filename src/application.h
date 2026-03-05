#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "../include/raylib.h"

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

typedef struct {
    const char* title;
    int targetFPS;
} Window;

typedef struct {
    bool nextSlide;
    bool previousSlide;
} InputEvent;

typedef struct {
    Window window;
    InputEvent input;
    RenderTexture2D target;
    int slideWidth, slideHeight;
    size_t currentSlideIndex;
} Application;

Application* AppNew(const char* title, int width, int height);
void AppRunWindow(Application* app);
void AppLoop(Application* app);
void AppFree(Application* app);
