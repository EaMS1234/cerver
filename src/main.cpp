#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include "../include/server.hpp"
#include "../include/http.hpp"


int main(int argc, char* argv[])
{
  int temp;
  int err;

  in_socket_fd sock;


  int port = 8080;
  std::string path = "./html/";

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0)
    {
      std::cout << "USAGE: cerver [options] folder\n" << std::endl;
      std::cout << "Folder: the directory with the files you want to serve\n" << std::endl;
      std::cout << "Options:\n  --help: prints this" << std::endl;
      std::cout << "  --port <port>: port for listening to HTTP requests" << std::endl;

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

  while (1)
  {
    temp = accept_connection(&sock);
    if (temp != -1)
    {
      std::thread msg(message, temp, path);
      msg.detach();
    }
  }
  
  close(sock.socket);

  return 0;
}

