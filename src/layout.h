#pragma once

#include <stdbool.h>

#include "asset_registry.h"
#include "element.h"
#include "presentation.h"

#define MAX_CHILDREN 512
#define MAX_LAYOUT_NODES 48
#define MAX_DEPTH 16

typedef enum LayoutNodeType { 
    LAYOUT_CONTAINER,
    LAYOUT_ELEMENT,
} LayoutNodeType;

typedef enum LayoutDirection {
    DIRECTION_ROW,
    DIRECTION_COLUMN,
} LayoutDirection;

typedef enum LayoutAlign {
    LAYOUT_ALIGN_START,
    LAYOUT_ALIGN_CENTER,
    LAYOUT_ALIGN_END,
} LayoutAlign;

typedef struct LayoutNode LayoutNode;

typedef struct ContainerData {
    LayoutDirection direction;
    LayoutAlign align;
    float gap;
    float padding;
    LayoutNode* children[MAX_CHILDREN];
    int childCount;
} ContainerData;

typedef struct ElementData {
    // stores the element data before it gets written to the slide
    Element pendingElement;
} ElementData;

struct LayoutNode {
    float x, y;
    float width, height;
    
    LayoutNodeType type;
    union {
        ContainerData container;
        ElementData element;
    } data;
};

typedef struct LayoutSlideScope {
    LayoutNode root;

    LayoutNode nodePool[MAX_LAYOUT_NODES];
    int nodePoolCount;

    LayoutNode* parentStack[MAX_DEPTH];
    int stackDepth;

    // which slide in the presentation we're building into
    Slide* targetSlide;
} LayoutSlideScope;

typedef struct LayoutContext {
    Presentation* presentation;
    AssetRegistry* registry;
    LayoutSlideScope currentScope;
    // tracks if the slide scope is currently open
    bool scopeOpen;
} LayoutContext;

void layoutBeginSlide(LayoutContext* ctx, Color backgroundColor);
void layoutEndSlide(LayoutContext* ctx);

void layoutBeginContainer(
    LayoutContext* ctx,
    LayoutDirection direciton,
    LayoutAlign align,
    float gap,
    float padding
);
void layoutEndContainer(LayoutContext* ctx);

void layoutAddElement(LayoutContext* ctx, Element element);
