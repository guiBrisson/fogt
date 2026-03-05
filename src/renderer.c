#include "renderer.h"

#include "../include/raylib.h"
#include "presentation.h"

static void renderSlideBackground(Slide* slide) {
    ClearBackground(slide->backgroundColor);
}

void renderSlide(Slide* slide) {
    // This is also important to clear the background
    renderSlideBackground(slide);
    DrawText(slide->title, 10, 10, 26, WHITE);
}
