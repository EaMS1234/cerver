#include <iostream>
#include <netinet/in.h>
#include <unistd.h>


typedef struct
{
  int socket;           // default socket file
  sockaddr_in address;  // socket's address
  int address_len;      // necessary because temporary sockets can change the address size
}
in_socket_fd;


int open_socket(in_socket_fd * sock, int port);
int accept_connection(in_socket_fd * sock);


int open_socket(in_socket_fd * sock, int port)
{
  // Returns -1 in case of an error. Otherwise, returns 0.

  sock->address_len = sizeof(sock->address);  // necessary because temporary sockets can change the address size
  
  int opt = 1;                                // necessary because sockopt needs the variable's address
  
  int err;                                    // used to check for errors along the way
  int temp;                                   // used as a temporary socket

  // Creates an IPV4 socket
  sock->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (sock->socket == 0)
  {
    std::cout << "Could not create socket" << std::endl;
    return -1;
  }

  // Sets the values in the address
  sock->address.sin_family = AF_INET;
  sock->address.sin_addr.s_addr = INADDR_ANY;
  sock->address.sin_port = htons(port);  // ports and addresses need to be mirrorred

  // Makes sure the OS can reuse the same port
  setsockopt(sock->socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  // Binds the socket to its host
  err = bind(sock->socket, (struct sockaddr *)&sock->address, sock->address_len);
  if (err < 0)
  {
    std::cout << "Could not bind socket to address" << std::endl;
    return -1;
  }

  // Start de-facto listening for connections
  err = listen(sock->socket, 5);
  if (err < 0)
  {
    std::cout << "Could not listen" << std::endl;
    return -1;
  }

  std::cout << "http://localhost:" << port << std::endl;

  return 0;
}

int accept_connection(in_socket_fd * sock)
{
  // Returns -1 in case of an error. Otherwise, returns 0.

  // Waits for a connection. When accepted, saves it as a temporary socket
  int temp = accept(sock->socket, (struct sockaddr *)&sock->address, (socklen_t *)&sock->address_len);
  
  if (temp < 0)
  {
    std::cout << "Could not accept connection" << std::endl;
    close(temp);
    return -1;
  }

  return temp;
}

