#include <stdio.h>
#include <string.h>

#include "../include/lauxlib.h"
#include "../include/lua.h"
#include "../include/lualib.h"

#include "application.h"
#include "asset_registry.h"
#include "presentation.h"
#include "element.h"


int HelloFromLua() {
    const char *tmpstr;

    lua_State *L = luaL_newstate();
    int err;
    luaopen_string(L);
    luaL_openlibs(L);

    const char *sample = "print(\"Hello World From Lua\")";

    err = luaL_loadbuffer(L, sample, strlen(sample), "mysample");
    if (err) {
        printf("Error initializing lua with hello world script: %i", err);
        tmpstr = lua_tostring(L, -1);
        if (tmpstr != NULL) {
        printf("%s\n", tmpstr); /* the detailed error string */
        } else {
        /* could print more information or do some detailed checks */
        }
        lua_close(L);
        return (0);
    }

    err = lua_pcall(L, 0, 0, 0);
    if (err) {
        printf("Error running lua hello world script: %i", err);
        tmpstr = lua_tostring(L, -1);
        if (tmpstr != NULL) {
        printf("%s\n", tmpstr); /* the detailed error string */
        } else {
        /* could print more information or do some detailed checks */
        }
        lua_close(L);
        return (0);
    }

    printf("Success running hello world script\n");
    lua_close(L);
    return (0);
}

int main() {
    // Steps
    // 1. Init window
	// 2. Execute the lua file
	//  2.1. Load assets (AssetRegistry) - Do not forget to free all assets
	// 3. layout calculation
	// 4. layout generates elements for each slide
    // 5. main loop
    Application* app = AppNew("Fogt", 1248, 702);
    AppRunWindow(app);
    int font = RegistryLoadFont(&app->registry, "resources/fonts/font.ttf", 24);
    int catImg = RegistryLoadTexture(&app->registry, "resources/images/cat.jpg");

    Slide* slide1 = PresAddSlide(&app->presentation);
    if (slide1 != NULL) {
        slide1->title = "First Slide";
        Element* rect = SlideAddElement(slide1);
        *rect = makeRectangle((Rectangle) {100, 100, 100, 100}, RED);

        Element* text = SlideAddElement(slide1);
		*text = makeText(
			(Rectangle) { 10, 10, 0, 0 },
			slide1->title,
			font,
			24,
			(Color) { 0xff, 0xff, 0xff, 0xff }
		);
    }

    Slide* slide2 = PresAddSlide(&app->presentation);
    if (slide2 != NULL) {
        slide2->title = "Second Slide";
        Element* text = SlideAddElement(slide2);
		*text = makeText(
			(Rectangle) { 10, 10, 0, 0 },
			slide2->title,
			-1,
			26,
			(Color) { 0xff, 0xff, 0xff, 0xff }
		);

        Element* image = SlideAddElement(slide2);
        *image = makeImage(catImg, (Rectangle) {100, 100, 400, 200}, false, WHITE);
    }

    AppLoop(app);

    HelloFromLua();
    AppFree(app);
    return 0;
}
