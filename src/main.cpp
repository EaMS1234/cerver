#include <thread>
#include <unistd.h>

#include "../include/server.hpp"
#include "../include/http.hpp"


int main()
{
  int temp;
  int err;

  in_socket_fd sock;
  
  err = open_socket(&sock, 8080);
  if (err != 0)
  {
    return err;
  }

  while (1)
  {
    temp = accept_connection(&sock);
    if (temp != -1)
    {
      std::thread msg(message, temp);
      msg.detach();
    }
  }
  
  close(sock.socket);

  return 0;
}

