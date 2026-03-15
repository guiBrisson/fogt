#include "api.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../include/lauxlib.h"

#include "asset_registry.h"
#include "element.h"
#include "layout.h"

#define LUA_GOBAL_LAYOUT_CTX "__layout_ctx"

int luaopen_fogt(lua_State* L);

void apiRegisterApi(lua_State* L, LayoutContext* ctx) {
    lua_pushlightuserdata(L, ctx);
    lua_setglobal(L, LUA_GOBAL_LAYOUT_CTX);

    luaL_requiref(L, "fogt", luaopen_fogt, 1);
}

static LayoutContext* getLayoutContext(lua_State* L) {
    lua_getglobal(L, LUA_GOBAL_LAYOUT_CTX);
    LayoutContext* ctx = lua_touserdata(L, -1);
    lua_pop(L, 1);
    assert(ctx != NULL && "Layout context not set");
    return ctx;
}

static void luaAssert(lua_State* L, bool condition, const char* message) {
    if (!condition) luaL_error(L, message);
}

static int luaFogtColor(lua_State* L) {
    luaAssert(L, lua_isnumber(L, 1), "color: expected number for r");
    luaAssert(L, lua_isnumber(L, 2), "color: expected number for g");
    luaAssert(L, lua_isnumber(L, 3), "color: expected number for b");
    luaAssert(L, lua_isnumber(L, 4), "color: expected number for a");

    unsigned char r = (unsigned char)lua_tointeger(L, 1);
    unsigned char g = (unsigned char)lua_tointeger(L, 2);
    unsigned char b = (unsigned char)lua_tointeger(L, 3);
    unsigned char a = (unsigned char)lua_tointeger(L, 4);

    // return as a table { r, g, b, a }
    lua_newtable(L);
    
    lua_pushinteger(L, r);
    lua_setfield(L, -2, "r");

    lua_pushinteger(L, g);
    lua_setfield(L, -2, "g");
    
    lua_pushinteger(L, b);
    lua_setfield(L, -2, "b");
    
    lua_pushinteger(L, a);
    lua_setfield(L, -2, "a");

    return 1;
}

static Color luaReadColor(lua_State* L, int tableIndex) {
    luaAssert(L, lua_istable(L, tableIndex), "expected a color table");

    Color c = {0};

    lua_getfield(L, tableIndex, "r");
    lua_getfield(L, tableIndex, "g");
    lua_getfield(L, tableIndex, "b");
    lua_getfield(L, tableIndex, "a");

    luaAssert(L, lua_isnumber(L, -4), "color: r is not a number");
    luaAssert(L, lua_isnumber(L, -3), "color: g is not a number");
    luaAssert(L, lua_isnumber(L, -2), "color: b is not a number");
    luaAssert(L, lua_isnumber(L, -1), "color: a is not a number");

    c.r = (unsigned char)lua_tointeger(L, -4);
    c.g = (unsigned char)lua_tointeger(L, -3);
    c.b = (unsigned char)lua_tointeger(L, -2);
    c.a = (unsigned char)lua_tointeger(L, -1);

    lua_pop(L, 4);

    return c;
}

static int luaFogtLoadFont(lua_State* L) {
    luaAssert(L, lua_isstring(L, 1), "load_font: expected string for path");
    luaAssert(L, lua_isstring(L, 2), "load_font: expected number for size");

    LayoutContext* ctx = getLayoutContext(L);

    const char* path = lua_tostring(L, 1);
    int size = lua_tointeger(L, 2);

    int id = RegistryLoadFont(ctx->registry, path, size);
    luaAssert(L, id != -1, "load_font: registry is full");

    lua_pushinteger(L, id);
    return 1;
}

static int luaFogtLoadImage(lua_State* L) {
    luaAssert(L, lua_isstring(L, 1), "load_image: expected string for path");

    LayoutContext* ctx = getLayoutContext(L);

    const char* path = lua_tostring(L, 1);

    int id = RegistryLoadTexture(ctx->registry, path);
    luaAssert(L, id != -1, "load_image: registry if full");

    lua_pushinteger(L, id);
    return 1;
}

static int luaFogtSlide(lua_State* L) {
    luaAssert(L, lua_istable(L, 1), "slide: expected table for properties");
    luaAssert(L, lua_isfunction(L, 2), "slide: expected function for body");

    LayoutContext* ctx = getLayoutContext(L);

    lua_getfield(L, 1, "background");
    Color backgroundColor = { 0x17, 0x17, 0x17, 0xFF };
    if (lua_istable(L, -1)) backgroundColor = luaReadColor(L, -1);
    lua_pop(L, 1);

    layoutBeginSlide(ctx, backgroundColor);

    // call the user's body function
    // this is where row/column/text etc get called
    lua_pushvalue(L, 2); // push the function onto the stack
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        luaL_error(L, "slide: error in body: %s", error);
    }

    layoutEndSlide(ctx);

    return 0;
}


static int luaFogtContainerImpl(lua_State* L, LayoutDirection direction, const char* name) {
    luaAssert(L, lua_istable(L, 1), "expected table for properties");
    luaAssert(L, lua_isfunction(L, 2), "expected function for body");

    LayoutContext* ctx = getLayoutContext(L);

    float gap = 0;
    float padding = 0;
    LayoutAlign align = LAYOUT_ALIGN_START;

    lua_getfield(L, 1, "gap");
    if (lua_isnumber(L, -1)) gap = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "padding");
    if (lua_isnumber(L, -1)) padding = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "align");
    if (lua_isstring(L, -1))
    {
        const char* align_str = lua_tostring(L, -1);
        if (strcmp(align_str, "center") == 0)      align = LAYOUT_ALIGN_CENTER;
        else if (strcmp(align_str, "end") == 0)    align = LAYOUT_ALIGN_END;
        else if (strcmp(align_str, "start") == 0)  align = LAYOUT_ALIGN_START;
        else luaL_error(L, "%s: unknown align value '%s'", name, align_str);
    }
    lua_pop(L, 1);

    layoutBeginContainer(ctx, direction, align, gap, padding);

    lua_pushvalue(L, 2);
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        const char* error = lua_tostring(L, -1);
        luaL_error(L, "%s: error in body: %s", name, error);
    }

    layoutEndContainer(ctx);

    return 0;
}

static int luaFogtRow(lua_State* L) {
    return luaFogtContainerImpl(L, DIRECTION_ROW, "row");
}

static int luaFogtColumn(lua_State* L) {
    return luaFogtContainerImpl(L, DIRECTION_COLUMN, "column");
}

static int luaFogtText(lua_State* L) {
    luaAssert(L, lua_istable(L, 1), "text: expected table for properties");

    LayoutContext* ctx = getLayoutContext(L);

    // text is required
    lua_getfield(L, 1, "content");
    luaAssert(L, lua_isstring(L, -1), "text: expected string for text property");
    const char* content = lua_tostring(L, -1);
    lua_pop(L, 1);

    // font is required
    lua_getfield(L, 1, "font");
    luaAssert(L, lua_isnumber(L, -1), "text: expected font handle for font");
    int fontId = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);

    // size is required
    lua_getfield(L, 1, "size");
    luaAssert(L, lua_isnumber(L, -1), "text: expected number for size");
    float fontSize = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    // color is optional
    Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
    lua_getfield(L, 1, "color");
    if (lua_istable(L, -1)) color = luaReadColor(L, -1);
    lua_pop(L, 1);

    // width and height are optional
    // if not provided, layout will calculate from font metrics
    float width = 0;
    float height = 0;

    lua_getfield(L, 1, "width");
    if (lua_isnumber(L, -1)) width = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "height");
    if (lua_isnumber(L, -1)) height = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    // Element e = {0};
    // e.type = ELEMENT_TEXT;
    // e.rect.width = width;
    // e.rect.height = height;

    // // copies the content text, from lua, to the element text
    // e.data.text.text = NULL;
    // size_t n = strlen(content) + 1; // + null terminator
    // e.data.text.text = malloc(n); // TODO: free the string whenever the element is freed
    // luaAssert(L, e.data.text.text != NULL, "text: out of memory");
    // memcpy(e.data.text.text, content, n);

    // e.data.text.fontId = fontId;
    // e.data.text.fontSize = fontSize;
    // e.data.text.color = color;

    Element element = makeText(
        (Rectangle){.x = 0, .y = 0, .width = width, .height = height}, 
        content, 
        fontId, 
        fontSize, 
        color
    );

    layoutAddElement(ctx, element);

    return 0;
}

static int luaFogtImage(lua_State* L) {
    luaAssert(L, lua_istable(L, 1), "image: expected table for properties");

    LayoutContext* ctx = getLayoutContext(L);

    // src is required
    // expects an asset id from load_image
    lua_getfield(L, 1, "src");
    luaAssert(L, lua_isnumber(L, -1), "image: expected image handle for src");
    int assetId = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);

    // tint is optional, defaults to white
    Color tint = { 0xFF, 0xFF, 0xFF, 0xFF };
    lua_getfield(L, 1, "tint");
    if (lua_istable(L, -1)) tint = luaReadColor(L, -1);
    lua_pop(L, 1);

    // width and height are optional
    // if not provided, layout will calculate from font metrics
    float width = 0;
    float height = 0;

    lua_getfield(L, 1, "width");
    if (lua_isnumber(L, -1)) width = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "height");
    if (lua_isnumber(L, -1)) height = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    Element element = makeImage(
        assetId, 
        (Rectangle){ .x = 0, .y = 0, .width = width, .height = height }, 
        true, 
        tint
    );

    layoutAddElement(ctx, element);

    return 0;
}

static int luaFogtShape(lua_State* L) {
    luaAssert(L, lua_istable(L, 1), "shape: expected table for properties");

    LayoutContext* ctx = getLayoutContext(L);

    // type is required
    lua_getfield(L, 1, "type");
    luaAssert(L, lua_isstring(L, -1), "shape: expected string for type");
    const char* shapeTypeSty = lua_tostring(L, -1);
    lua_pop(L, 1);

    ShapeType shapeType = SHAPE_RECT;
    if(strcmp(shapeTypeSty, "rect") == 0) shapeType = SHAPE_RECT;
    else if (strcmp(shapeTypeSty, "circle") == 0) shapeType = SHAPE_CIRCLE;
    else luaL_error(L, "shape: unknown shape type '%s'", shapeTypeSty);

    // color is optional
    Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
    lua_getfield(L, 1, "color");
    if (lua_istable(L, -1)) color = luaReadColor(L, -1);
    lua_pop(L, 1);

    // width and height are relevant for rectangle
    float width = 0;
    float height = 0;

    lua_getfield(L, 1, "width");
    if (lua_isnumber(L, -1)) width = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "height");
    if (lua_isnumber(L, -1)) height = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    // radius is relevant for circle
    float radius = 0;
    lua_getfield(L, 1, "radius");
    if (lua_isnumber(L, -1)) radius = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    Element element = {0};
    switch (shapeType) {
        case SHAPE_RECT: {
            luaAssert(L, width > 0,  "shape rect: width must be provided and greater than zero");
            luaAssert(L, height > 0, "shape rect: height must be provided and greater than zero");
            element = makeRectangle((Rectangle){ .x = 0, .y = 0, .width = width, .height = height } , color);
        } break;
        case SHAPE_CIRCLE: {
            luaAssert(L, width > 0, "shape circle: radius must be provided and greater than zero");
            element = makeCircle(0, 0, radius, color);
        } break;
    }

    layoutAddElement(ctx, element);

    return 0;
}

static const luaL_Reg lib[] = {
    { "color",      luaFogtColor        },
    { "load_font",  luaFogtLoadFont     },
    { "load_image", luaFogtLoadImage    },
    { "slide",      luaFogtSlide        },
    { "row",        luaFogtRow          },
    { "column",     luaFogtColumn       },
    { "text",       luaFogtText         },
    { "image",      luaFogtImage        },
    { "shape",      luaFogtShape        },
    { NULL,         NULL                }
};

int luaopen_fogt(lua_State* L) {
    luaL_newlib(L, lib);
    return 1;
}
