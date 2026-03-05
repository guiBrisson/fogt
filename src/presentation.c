#include "presentation.h"

#include "../include/raylib.h"
#include <stddef.h>

Slide* PresAddSlide(Presentation* pres) {
    if (pres->slideSize >= MAX_SLIDES) return NULL;
    Slide* slide = &pres->slides[pres->slideSize];
    slide->backgroundColor = (Color) { 0x17, 0x17, 0x17, 0xff};
    pres->slideSize++;
    return slide;
}

Slide* PresGetSlideAt(Presentation* pres, size_t index) {
    if (index >= pres->slideSize) return NULL;
    return &pres->slides[index];
}

Element* SlideAddElement(Slide* slide) {
    if (slide->elementSize >= MAX_ELEMENTS) return NULL;
    Element* element = &slide->elements[slide->elementSize];
    slide->elementSize++;
    return element;
}

Element* SlideGetElementAt(Slide* slide, size_t index){
    if (index >= slide->elementSize) return NULL;
    return &slide->elements[index];
}
