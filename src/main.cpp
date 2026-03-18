#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <lua.hpp>

#include "../include/server.hpp"
#include "../include/http.hpp"


// Global routes
std::vector<route> routes;


int create_route(lua_State *L);

int main(int argc, char* argv[])
{
  int temp;
  int err;

  in_socket_fd sock;

  lua_State *L = luaL_newstate();

  int port = 8080;
  std::string path = "./html/";

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0)
    {
      std::cout << "usage: cerver [--help | --port <port>] [<directory> | <file>.lua]\n" << std::endl;
      std::cout << "Directory: the directory with the files to be served" << std::endl;
      std::cout << "File: serves custom routes from a .lua file instead of a directory.\n" << std::endl;
      std::cout << "--help: Shows this message" << std::endl;
      std::cout << "--port: Change the port used while listening for requests\n" << std::endl;

      return 0;
    }
    else if (strcmp(argv[i], "--port") == 0)
    {
      port = std::stoi(argv[i + 1]);
      i++;
    }
    else
    {
      path = argv[i];
    }
  }

  err = open_socket(&sock, port);
  if (err != 0)
  {
    return err;
  }

  if (std::filesystem::path(path).has_extension())
  {
    lua_register(L, "create_route", create_route);

    // Loads the lua script
    luaL_openlibs(L);
    luaL_dofile(L, path.c_str());
  }
  else
  {
    routes.push_back({"/", "serve_directory", path});
  }

  while (1)
  {
    temp = accept_connection(&sock);
    if (temp != -1)
    {
      std::thread msg(message, temp, routes);
      msg.detach();
    }
  }

  close(sock.socket);

  lua_close(L);

  return 0;
}

int create_route(lua_State *L)
{
  route r;

  r.URL = luaL_checkstring(L, 1);
  r.function = luaL_checkstring(L, 2);
  r.path = luaL_checkstring(L, 3);
  
  routes.push_back(r);

  return 1;
}

