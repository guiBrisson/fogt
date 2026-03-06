#pragma once

#include <stdbool.h>

#include "../include/raylib.h"

typedef enum ElementType {
    ELEMENT_TEXT,
    ELEMENT_IMAGE,
    ELEMENT_SHAPE,
} ElementType;

typedef enum ShapeType {
    SHAPE_RECT,
    SHAPE_CIRCLE,
} ShapeType;

typedef struct TextData {
    const char* text;
    int fontId;
    int fontSize;
    Color color;
} TextData;

typedef struct ImageData {
    int assertId;
    Color tint;
    bool hasTint;
} ImageData;

typedef struct ShapeData {
    ShapeType shapeType;
    Color color;
} ShapeData;

typedef struct Element {
    ElementType type;
    Rectangle rect;
    union {
        TextData text;
        ShapeData shape;
        ImageData iamge;
    } data;
} Element;

Element makeText(Rectangle rect, const char* text, int fontId, int fontSize, Color color);
Element makeRectangle(Rectangle rect, Color color);
Element makeCircle(int centerX, int centerY, float radius, Color color);
