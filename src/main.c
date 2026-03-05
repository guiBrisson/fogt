#include <stdio.h>
#include <string.h>

#include "../include/lauxlib.h"
#include "../include/lua.h"
#include "../include/lualib.h"

#include "application.h"
#include "presentation.h"


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
    Application* app = AppNew("Fogt", 1248, 702);
    AppRunWindow(app);
    
    Slide* slide1 = PresAddSlide(&app->presentation);
    if (slide1 != NULL) slide1->title = "First Slide";
    
    Slide* slide2 = PresAddSlide(&app->presentation);
    if (slide2 != NULL) slide2->title = "Second Slide";

    AppLoop(app);

    AppFree(app);
    return 0;
}
