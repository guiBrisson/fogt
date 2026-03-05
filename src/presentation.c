#include "presentation.h"

#include "../include/raylib.h"

Slide* PresAddSlide(Presentation* pres) {
    if (pres->slide_size >= MAX_SLIDES) return NULL;
    Slide* slide = &pres->slides[pres->slide_size];
    slide->backgroundColor = (Color) { 0x17, 0x17, 0x17, 0xff};
    pres->slide_size++;
    return slide;
}

Slide* PresGetSlideAt(Presentation* pres, size_t index) {
    if (index >= pres->slide_size) return NULL;
    return &pres->slides[index];
}
