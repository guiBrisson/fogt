#include "layout.h"

#include <assert.h>

#include "presentation.h"


static void layoutPassSize(LayoutNode* node) {
    if (node->type == LAYOUT_ELEMENT) return;

    ContainerData* container = &node->data.container;

    // recurse into children first - bottom up
    for (int i = 0; i < container->childCount; i++) {
        layoutPassSize(container->children[i]);
    }

    float totalWidth = 0;
    float totalHeight = 0;
    float gap = container->gap;

    switch (container->direction) {
        case DIRECTION_ROW: {
            // row: width is sum of children widths + gaps
            //      height is the tallest child
            for (int i = 0; i < container->childCount; i++) {
                LayoutNode* child = container->children[i];
                totalWidth += child->width;
                if (child->height > totalHeight) totalHeight = child->height;
            }

            // add gaps between children
            if (container->childCount > 1) {
                totalWidth += gap * (container->childCount - 1);
            }
        } break;
        case DIRECTION_COLUMN: {
            // column: height is sum of children heights + gaps
            //         width is the widest child
            for (int i = 0; i < container->childCount; i++) {
                LayoutNode* child = container->children[i];
                totalHeight += child->height;
                if (child->width > totalWidth) totalWidth = child->width;
            }

            // add gaps
            if (container->childCount > 1) {
                totalHeight += gap * (container->childCount - 1);
            }
        } break;
    }

    // add pagging on all sides
    totalWidth += container->padding * 2;
    totalHeight += container->padding * 2;

    // only set size if not explicitly declared
    // a container with explicit size should not be overridden by its children
    if (node->width == 0) node->width = totalWidth;
    if (node->height == 0) node->height = totalHeight;
}

static void layoutPassPosition(LayoutNode* node, float x, float y) {
    node->x = x;
    node->y = y;

    if (node->type == LAYOUT_ELEMENT) return;

    ContainerData* container = &node->data.container;

    // start position accounts for padding
    float cursor = container->padding;

    for (int i = 0; i < container->childCount; i++) {
        LayoutNode* child = container->children[i];

        float childX = 0;
        float childY = 0;

        switch (container->direction) {
            case DIRECTION_ROW: {
                childX = x + cursor;

                // alignment on the cross axis (vertical for row)
                switch (container->align) {
                    case LAYOUT_ALIGN_START: {
                        childY = y + container->padding;
                    } break;
                    case LAYOUT_ALIGN_CENTER: {
                        childY = y + (node->height / 2) - (child->height / 2);
                    } break;
                    case LAYOUT_ALIGN_END: {
                        childY = y + node->height - child->height - container->padding;
                    } break;
                }

                cursor += child->width + container->gap;
            } break;
            case DIRECTION_COLUMN: {
                childY = y + cursor;

                // alignment on the cross axis (horizontal for column)
                switch (container->align) {
                    case LAYOUT_ALIGN_START: {
                        childX = x + container->padding;
                    } break;
                    case LAYOUT_ALIGN_CENTER: {
                        childX = x + (node->width / 2) - (child->width / 2);
                    } break;
                    case LAYOUT_ALIGN_END: {
                        childX = x +node->width - child->width - container->padding;
                    } break;
                }

                cursor += child->height + container->gap;
            } break;
        }

        layoutPassPosition(child, childX, childY);
    }
}

static void layoutFlatten(LayoutNode* node, Slide* slide) {
    if (node->type == LAYOUT_ELEMENT) {
        Element* e = SlideAddElement(slide);    
        assert(e != NULL && "Slide element limit reached during flatten");
    
        *e = node->data.element.pendingElement;
        e->rect.x = node->x;
        e->rect.y = node->y;
        e->rect.width = node->width;
        e->rect.height = node->height;

        return;
    }

    // container - recurse into children
    ContainerData* container = &node->data.container;
    for (int i = 0; i < container->childCount; i++) {
        layoutFlatten(container->children[i], slide);
    }
}

void layoutBeginSlide(LayoutContext* ctx, Color backgroundColor) {
    assert(!ctx->scopeOpen && "A slide is currently open");

    LayoutSlideScope* scope = &ctx->currentScope;
    *scope = (LayoutSlideScope){0};

    scope->targetSlide = PresAddSlide(ctx->presentation);
    scope->targetSlide->backgroundColor = backgroundColor;

    LayoutNode* root = &scope->root;
    root->type = LAYOUT_CONTAINER;
    root->width = ctx->presentation->slideWidth;
    root->height = ctx->presentation->slideHeight;
    root->x = 0;
    root->y = 0;

    scope->parentStack[0] = root;
    scope->stackDepth = 1;

    ctx->scopeOpen = true;
}

void layoutEndSlide(LayoutContext* ctx) {
    assert(ctx->scopeOpen && "No slide scope is open");
    assert(ctx->currentScope.stackDepth == 1 && "Unclosed container in slide");
    
    LayoutSlideScope* scope = &ctx->currentScope;

    layoutPassSize(&scope->root);
    layoutPassPosition(&scope->root, 0, 0);
    layoutFlatten(&scope->root, scope->targetSlide);

    ctx->scopeOpen = false;
}

static void addAsChildOfCurrentParent(LayoutSlideScope* scope, LayoutNode* node) {
    LayoutNode* parent = scope->parentStack[scope->stackDepth - 1];
    assert(parent->data.container.childCount < MAX_CHILDREN && "Too many children");
    parent->data.container.children[parent->data.container.childCount++] = node;
}

void layoutBeginContainer(
    LayoutContext* ctx,
    LayoutDirection direciton,
    LayoutAlign align,
    float gap,
    float padding
) {
    assert(ctx->scopeOpen && "No slide scope is open");

    LayoutSlideScope* scope = &ctx->currentScope;

    assert(scope->stackDepth < MAX_DEPTH && "Layout stack overflow");
    assert(scope->nodePoolCount < MAX_LAYOUT_NODES && "Layout node pool exhausted");

    LayoutNode* node = &scope->nodePool[scope->nodePoolCount++];
    *node = (LayoutNode){0};

    node->type = LAYOUT_CONTAINER;
    node->data.container = (ContainerData) {
        .direction = direciton,
        .align = align,
        .gap = gap,
        .padding = padding,
    };

    addAsChildOfCurrentParent(scope, node);

    scope->parentStack[scope->stackDepth++] = node;
}

void layoutEndContainer(LayoutContext* ctx) {
    assert(ctx->scopeOpen && "No slide scope is open");
    assert(ctx->currentScope.stackDepth > 1 && "No container to close");
    
    ctx->currentScope.stackDepth--;
}

void layoutAddElement(LayoutContext* ctx, Element element) {
    assert(ctx->scopeOpen && "No slide scope is open");

    LayoutSlideScope* scope = &ctx->currentScope;

    assert(scope->nodePoolCount < MAX_LAYOUT_NODES && "Layout node pool exhausted");

    LayoutNode* node = &scope->nodePool[scope->nodePoolCount++];
    *node = (LayoutNode){0};

    node->type = LAYOUT_ELEMENT;
    node->data.element = (ElementData) {
        .pendingElement = element
    };

    node->width = element.rect.width;
    node->height = element.rect.height;

    addAsChildOfCurrentParent(scope, node);
}
