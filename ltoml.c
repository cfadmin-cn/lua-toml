/*
**  LICENSE: BSD
**  Author: CandyMi[https://github.com/candymi]
*/
#define LUA_LIB

#include <core.h>
#include "toml.h"

static inline void toml_to_lua_array(lua_State *L, toml_array_t* array);
static inline void toml_to_lua_table(lua_State *L, toml_table_t* table);

static inline void toml_to_lua_array(lua_State *L, toml_array_t* array)
{
  lua_newtable(L); toml_datum_t o;
  for (int i = 0; i < toml_array_nelem(array); i++)
  {
    /* 递归 迭代 */
    toml_table_t* tab = toml_table_at(array, i);
    if (tab) {
      toml_to_lua_table(L, tab); lua_rawseti(L, -2, i+1);
      continue;
    }
    toml_array_t* arr = toml_array_at(array, i);
    if (arr) {
      toml_to_lua_array(L, arr); lua_rawseti(L, -2, i+1);
      continue;
    }

    o = toml_bool_at(array, i);
    if (o.ok) {
      lua_pushboolean(L, o.u.b); lua_rawseti(L, -2, i+1);
      continue;
    }
    o = toml_int_at(array, i);
    if (o.ok) {
      lua_pushinteger(L, o.u.i); lua_rawseti(L, -2, i+1);
      continue;
    }
    o = toml_double_at(array, i);
    if (o.ok) {
      lua_pushnumber(L, o.u.d); lua_rawseti(L, -2, i+1);
      continue;
    }
    o = toml_string_at(array, i);
    if (o.ok) {
      lua_pushstring(L, o.u.s); lua_rawseti(L, -2, i+1);
      xrio_free(o.u.s);
      continue;
    }
    o = toml_timestamp_at(array, i);
    if (o.ok) {
      lua_newtable(L);
      if (o.u.ts->year) {
        lua_pushinteger(L, *o.u.ts->year); lua_setfield(L, -2, "year");
      }
      if (o.u.ts->month) {
        lua_pushinteger(L, *o.u.ts->month); lua_setfield(L, -2, "month");
      }
      if (o.u.ts->day) {
        lua_pushinteger(L, *o.u.ts->day); lua_setfield(L, -2, "day");
      }
      if (o.u.ts->hour) {
        lua_pushinteger(L, *o.u.ts->hour); lua_setfield(L, -2, "hour");
      }
      if (o.u.ts->minute) {
        lua_pushinteger(L, *o.u.ts->minute); lua_setfield(L, -2, "min");
      }
      if (o.u.ts->second) {
        lua_pushinteger(L, *o.u.ts->second); lua_setfield(L, -2, "sec");
      }
      if (o.u.ts->millisec) {
        lua_pushinteger(L, *o.u.ts->millisec); lua_setfield(L, -2, "ms");
      }
      xrio_free(o.u.ts); lua_rawseti(L, -2, i+1);
      continue;
    }
  }
}

static inline void toml_to_lua_table(lua_State *L, toml_table_t* table)
{
  lua_newtable(L); int i = 0; toml_datum_t o;
  while (1)
  {
    const char* key = toml_key_in(table, i++);
    if (!key)
      break;
    /* 递归 迭代 */
    toml_table_t* tab = toml_table_in(table, key);
    if (tab) {
      toml_to_lua_table(L, tab); lua_setfield(L, -2, key);
      continue;
    }
    toml_array_t* arr = toml_array_in(table, key);
    if (arr) {
      toml_to_lua_array(L, arr); lua_setfield(L, -2, key);
      continue;
    }

    o = toml_bool_in(table, key);
    if (o.ok) {
      lua_pushboolean(L, o.u.b); lua_setfield(L, -2, key);
      continue;
    }
    o = toml_int_in(table, key);
    if (o.ok) {
      lua_pushinteger(L, o.u.i); lua_setfield(L, -2, key);
      continue;
    }
    o = toml_double_in(table, key);
    if (o.ok) {
      lua_pushnumber(L, o.u.d); lua_setfield(L, -2, key);
      continue;
    }
    o = toml_string_in(table, key);
    if (o.ok) {
      lua_pushstring(L, o.u.s); lua_setfield(L, -2, key);
      xrio_free(o.u.s);
      continue;
    }
    o = toml_timestamp_in(table, key);
    if (o.ok) {
      lua_newtable(L);
      if (o.u.ts->year) {
        lua_pushinteger(L, *o.u.ts->year); lua_setfield(L, -2, "year");
      }
      if (o.u.ts->month) {
        lua_pushinteger(L, *o.u.ts->month); lua_setfield(L, -2, "month");
      }
      if (o.u.ts->day) {
        lua_pushinteger(L, *o.u.ts->day); lua_setfield(L, -2, "day");
      }
      if (o.u.ts->hour) {
        lua_pushinteger(L, *o.u.ts->hour); lua_setfield(L, -2, "hour");
      }
      if (o.u.ts->minute) {
        lua_pushinteger(L, *o.u.ts->minute); lua_setfield(L, -2, "min");
      }
      if (o.u.ts->second) {
        lua_pushinteger(L, *o.u.ts->second); lua_setfield(L, -2, "sec");
      }
      if (o.u.ts->millisec) {
        lua_pushinteger(L, *o.u.ts->millisec); lua_setfield(L, -2, "ms");
      }
      xrio_free(o.u.ts); lua_setfield(L, -2, key);
      continue;
    }
  }
}

static int toml_load(lua_State *L)
{
  size_t bsize;
  const char* buffer = luaL_checklstring(L, 1, &bsize);
  if (!buffer || bsize < 1) {
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[toml errorl]: Invalid toml buffer.");
    return 2;
  }

  /* 解决警告 */
  char *conf = lua_newuserdata(L, bsize + 1);
  memcpy(conf, buffer, bsize); conf[bsize] = 0x0;

  char ebuf[256]; memset(ebuf, 0x0, 256);
  toml_table_t* root = toml_parse(conf, ebuf, 256);
  if (!root){
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[toml errorl]: %s", ebuf);
    return 2;
  }

  toml_to_lua_table(L, root);
  toml_free(root);
  return 1;
}

static int toml_loadfile(lua_State *L)
{
  size_t fszie;
  const char* filename = luaL_checklstring(L, 1, &fszie);
  if (!filename || fszie < 1) {
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[toml errorl]: Invalid filename.");
    return 2;
  }

  FILE* fp = fopen(filename, "r");
  if (!fp) {
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[toml errorl]: file `%s` does not exist.", filename);
    return 2;
  }

  char ebuf[256]; memset(ebuf, 0x0, 256);
  toml_table_t* root = toml_parse_file(fp, ebuf, 256);
  if (!root){
    lua_pushboolean(L, 0);
    lua_pushfstring(L, "[toml errorl]: %s", ebuf);
    return 2;
  }

  toml_to_lua_table(L, root);
  toml_free(root);
  return 1;
}

LUAMOD_API int luaopen_toml(lua_State *L)
{
  luaL_checkversion(L);
  const luaL_Reg toml_libs[] = {
    {"load", toml_load},
    {"loadfile", toml_loadfile},
    {NULL, NULL},
  };
  luaL_newlib(L, toml_libs);
  return 1;
}
