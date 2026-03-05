#include "element.h"

Element makeText(Rectangle rect, const char* text, int fontId, int fontSize, Color color) {
    TextData data = {
        .text = text,
        .fontId = fontId,
        .fontSize = fontSize,
        .color = color,
    };
    Element element = {
        .type = ELEMENT_TEXT,
        .rect = rect,
        .data.text = data,
    };
    return element;
}

Element makeRectangle(Rectangle rect, Color color) {
    ShapeData data = {
        .shapeType = SHAPE_RECT,
        .color = color,
    };
    Element element = {
        .type = ELEMENT_SHAPE,
        .rect = rect,
        .data.shape = data,
    };
    return element;
}

Element makeCircle(int centerX, int centerY, float radius, Color color) {
    ShapeData data = {
        .shapeType = SHAPE_CIRCLE,
        .color = color,
    };
    Rectangle rect = {
        .x = centerX - (int)radius,
        .y = centerY - (int)radius,
        .width = (int)(radius * 2),
        .height = (int)(radius * 2),
    };
    Element element = {
        .type = ELEMENT_SHAPE,
        .rect = rect,
        .data.shape = data,
    };
    return element;
}
