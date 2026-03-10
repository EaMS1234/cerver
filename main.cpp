#include <cstring>
#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>


int main()
{
  int sockfd;                      // default socker file
  struct sockaddr_in address;      // socket's address
  int addr_len = sizeof(address);  // necessary because temporary sockets can change the address size
  int opt = 1;                     // necessary because sockopt needs the variable's address
  
  char buffer[1024] = {0};         // buffer with the received message
  msghdr msg;                      // the message itself
  iovec vec;                       // io vector for the message

  int err;                         // used to check for errors along the way
  int temp;                        // used as a temporary socket

  // Creates an IPV4 socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == 0)
  {
    std::cout << "Could not create socket\n";
    return sockfd;
  }

  // Sets the values in the address
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);  // ports and addresses need to be mirrorred

  // Makes sure the OS can reuse the same port
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  // Binds the socket to its host
  err = bind(sockfd, (struct sockaddr *)&address, addr_len);
  if (err < 0)
  {
    std::cout << "Could not bind socket to address\n";
    return err;
  }

  // Start de-facto listening for connections
  err = listen(sockfd, 5);
  if (err < 0)
  {
    std::cout << "Could not listen\n";
    return err;
  }

  // Sets up the IO vector and the message
  vec.iov_base = &buffer;
  vec.iov_len = 1024;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;
  
  // Waits for a connection. When accepted, saves it as a temporary socket
  temp = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&addr_len);
  if (temp < 0)
  {
    std::cout << "Could not accept connection\n";
    close(temp);
  }

  // Keeps trying to receive messages
  while (1)
  {
    // clears the buffer
    memset(buffer, 0, 1024);

    if (recvmsg(temp, &msg, 0) > 0)
    {
      std::cout << buffer;
    }
    else
    {
      close(temp);
      break;
    }
  }
  
  close(sockfd);

  return 0;
}

