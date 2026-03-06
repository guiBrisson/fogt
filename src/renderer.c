#include "renderer.h"

#include "../include/raylib.h"
#include "element.h"
#include "presentation.h"
#include "asset_registry.h"

static void renderSlideBackground(Slide* slide) {
    ClearBackground(slide->backgroundColor);
}

static void renderElement(Element* element, AssetRegistry* registry) {
    switch (element->type) {
        case ELEMENT_TEXT: {
            TextData data = element->data.text;

            if (data.fontId == -1) {
                DrawText(
                    data.text, 
                    element->rect.x, 
                    element->rect.y, 
                    data.fontSize, 
                    data.color
                );
            } else {
                Font* font = RegistryFontById(registry, data.fontId);
                DrawTextEx(
                    *font,
                    data.text,
                    (Vector2){ .x = element->rect.x, .y = element->rect.y},
                    data.fontSize,
                    2,
                    data.color
                );
            }
            break;
        }
        case ELEMENT_SHAPE: {
            ShapeData shape = element->data.shape;
            switch (shape.shapeType) {
            case SHAPE_RECT: {
                DrawRectangle(
                    element->rect.x,
                    element->rect.y,
                    element->rect.width,
                    element->rect.height,
                    shape.color
                );
                break;
            }
            case SHAPE_CIRCLE: {
                int cx = (int)(element->rect.x + element->rect.width / 2.0f);
                int cy = (int)(element->rect.y + element->rect.height / 2.0f);
                float radius = (float)(element->rect.width / 2);
                DrawCircle(cx, cy, radius, shape.color);
            }
            break;
        }
        case ELEMENT_IMAGE:
            break;
        }
    }
}

static void renderSlideElements(Slide* slide, AssetRegistry* registry) {
    if (slide->elementSize == 0) return;
    for (size_t i = 0; i < slide->elementSize; i++) {
        Element* element = SlideGetElementAt(slide, i);
        renderElement(element, registry);
    }
}

void renderSlide(Slide* slide, AssetRegistry* registry) {
    renderSlideBackground(slide); // This is also important to clear the background
    renderSlideElements(slide, registry);
}
