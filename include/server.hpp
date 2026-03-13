#include <netinet/in.h>


typedef struct
{
  int socket;           // default socket file
  sockaddr_in address;  // socket's address
  int address_len;      // necessary because temporary sockets can change the address size
}
in_socket_fd;


int open_socket(in_socket_fd * sock, int port);
int accept_connection(in_socket_fd * sock);

