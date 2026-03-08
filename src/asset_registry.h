#pragma once

#include "../include/raylib.h"
#include <stddef.h>

#define MAX_TEXTURES 512
#define MAX_FONTS 32

typedef struct TextureEntry {
    char path[256];
    Texture2D texture;
} TextureEntry;

typedef struct FontEntry{
    char path[256];
    unsigned int fontSize;
    Font font;
} FontEntry;

typedef struct AssetRegistry {
    TextureEntry textures[MAX_TEXTURES];
    size_t textureSize;

    FontEntry fonts[MAX_FONTS];
    size_t fontSize;
} AssetRegistry;

AssetRegistry RegistryNew();
int RegistryLoadTexture(AssetRegistry* registry, const char* path);
int RegistryLoadFont(AssetRegistry* registry, const char* path, unsigned int fontSize);

Texture2D* RegistryTextureById(AssetRegistry* registry, size_t id);
Font* RegistryFontById(AssetRegistry* registry, size_t id);

void RegistryUnload(AssetRegistry* registry);
