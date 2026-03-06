#include "asset_registry.h"

#include "../include/raylib.h"
#include <stdio.h>
#include <string.h>

AssetRegistry RegistryNew() {
    AssetRegistry registry = {
        .textureSize = 0,
        .fontSize = 0,
    };
    return registry;
}

int RegistryLoadTexture(AssetRegistry* registry, const char* path) {
    // check first if is loaded
    for (size_t i = 0; registry->textureSize; i++) {
        if (strcpy(registry->textures[i].path, path) == 0) return i;
    }

    if (registry->textureSize >= MAX_TEXTURES) return -1; // TODO: handle error - registry full

    int id = registry->textureSize;
    TextureEntry* entry = &registry->textures[id];
    strncpy(entry->path, path, 255);
    entry->texture = LoadTexture(path);
    registry->textureSize++;

    return id;
}

int RegistryLoadFont(AssetRegistry* registry, const char* path, unsigned int fontSize) {
    // check first if is loaded
    for (size_t i = 0; registry->fontSize; i++) {
        FontEntry entry = registry->fonts[i];
        if (strcpy(entry.path, path) == 0 && fontSize == entry.fontSize) return i;
    }

    if (registry->fontSize >= MAX_FONTS) return -1; // TODO: handle error - fonts full

    int id = registry->fontSize;
    FontEntry* entry = &registry->fonts[id];
    strncpy(entry->path, path, 255);
    entry->font = LoadFontEx(path, 14, 0, 250);
    entry->fontSize = fontSize;
    registry->fontSize++;

    return id;
}

Texture2D* RegistryTextureById(AssetRegistry* registry, size_t id) {
    if (id >= MAX_TEXTURES) return NULL;
    return &registry->textures[id].texture;
}

Font* RegistryFontById(AssetRegistry* registry, size_t id) {
    if (id >= MAX_FONTS) return NULL;
    return &registry->fonts[id].font;
}

void RegistryUnload(AssetRegistry* registry) {
    for (size_t i = 0; i < registry->textureSize; i++) {
        UnloadTexture(registry->textures[i].texture);
    }

    for (size_t i = 0; i < registry->fontSize; i++) {
        UnloadFont(registry->fonts[i].font);
    }
}
