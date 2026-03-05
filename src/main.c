#include <stdio.h>
#include <string.h>

#include "../include/lauxlib.h"
#include "../include/lua.h"
#include "../include/lualib.h"
#include "../include/raylib.h"

void CreateWindow() {
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "fogt");
  SetTargetFPS(60);
}

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
  HelloFromLua();
  CreateWindow();
  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawText("Hello, World!", 10, 10, 20, LIGHTGRAY);
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
