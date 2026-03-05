#include "presentation.h"

Slide* PresAddSlide(Presentation* pres) {
    if (pres->slide_size >= MAX_SLIDES) return NULL;
    Slide* slide = &pres->slides[pres->slide_size];
    pres->slide_size++;
    return slide;
}

Slide* PresGetSlideAt(Presentation* pres, size_t index) {
    if (index >= pres->slide_size) return NULL;
    return &pres->slides[index];
}
