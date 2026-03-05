#pragma once

#include <stdio.h>

#define MAX_SLIDES 256

typedef struct {
    const char* title;
} Slide;

typedef struct {
    Slide slides[MAX_SLIDES];
    size_t slide_size;
} Presentation;

Slide* PresAddSlide(Presentation* pres);
Slide* PresGetSlideAt(Presentation* pres, size_t index);
