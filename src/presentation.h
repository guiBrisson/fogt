#pragma once

#include <stddef.h>
#include <stdio.h>

#include "../include/raylib.h"

#include "element.h"

#define MAX_SLIDES 256
#define MAX_ELEMENTS 512

typedef struct Slide {
    const char* title;
    Color backgroundColor;
    Element elements[MAX_ELEMENTS];
    size_t elementSize;
} Slide;

typedef struct Presentation {
    Slide slides[MAX_SLIDES];
    size_t slideSize;
    int slideWidth, slideHeight;
} Presentation;

Slide* PresAddSlide(Presentation* pres);
Slide* PresGetSlideAt(Presentation* pres, size_t index);

Element* SlideAddElement(Slide* slide);
Element* SlideGetElementAt(Slide* slide, size_t index);
