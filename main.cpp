#include <cstring>
#include <iostream>
#include <thread>
#include <queue>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>


// TODO: Add other methods
typedef enum
{
  GET = 0
}
http_method;

// TODO: Add support to version 2
typedef enum
{
  VER_11 = 1
}
http_version;


// TODO: Parse the headers as well
typedef struct
{
  http_method method;
  http_version version;
  char * path;  
}
request;


void message(int socket);
int parserequest(request * req, char * message);


int main()
{
  int sockfd;                      // default socker file
  struct sockaddr_in address;      // socket's address
  int addr_len = sizeof(address);  // necessary because temporary sockets can change the address size
  int opt = 1;                     // necessary because sockopt needs the variable's address
  
  int err;                         // used to check for errors along the way
  int temp;                        // used as a temporary socket

  // Creates an IPV4 socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == 0)
  {
    std::cout << "Could not create socket" << std::endl;
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
    std::cout << "Could not bind socket to address" << std::endl;
    return err;
  }

  // Start de-facto listening for connections
  err = listen(sockfd, 5);
  if (err < 0)
  {
    std::cout << "Could not listen" << std::endl;
    return err;
  }

  while (1)
  {
    // Waits for a connection. When accepted, saves it as a temporary socket
    temp = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&addr_len);
    if (temp < 0)
    {
      std::cout << "Could not accept connection" << std::endl;
      close(temp);
    }
    else
    {
      std::thread msg(message, temp);
      msg.detach();
    }
  }
  
  close(sockfd);

  return 0;
}


void message(int socket)
{
  char buffer[1024] = {0};         // buffer with the received message
  msghdr msg;                      // the message itself
  iovec vec;                       // io vector for the message
  request req;
  
  int err;

  // Sets up the IO vector and the message
  vec.iov_base = &buffer;
  vec.iov_len = 1024;
  msg.msg_iov = &vec;
  msg.msg_iovlen = 1;

  while(1)
  {
    // clears the buffer
    memset(buffer, 0, 1024);

    if (recvmsg(socket, &msg, 0) > 0)
    {
      err = parserequest(&req, buffer);
      if (err != 0)
      {
        send(socket, "HTTP/1.1 400", 13, 0);
        break;
      }
      else
      {
        send(socket, "HTTP/1.1 200\nServer: Cerver\nConnection: close\nContent-Length: 21\nContent-Type: text/html\n\n<p>Hello, World!</p>\n", 112, 0);
      }
    }
    else
    {
      std::cout << "Connection closed" << std::endl;
      break;
    }
  }

  close(socket);
}


int parserequest(request * req, char * message)
{
  char buffer[1024] = {0};
  std::queue<std::string> data;  // Three elements (front to back): Method, Path, Version
                                
  std::cout << message << std::endl;

  int i = 0;
  int j = 0;
  while (1)
  {
    if (message[i] == ' ' || message[i] == '\n' || message[i] == '\0')
    {
      data.emplace(buffer);
      memset(buffer, 0, 1024);

      j = i + 1;

      if (message[i] != ' ')
      {
        break;
      }
    }
    else
    {
      buffer[i - j] = message[i];
    }

    i++;
  }

  // Failed to grab the method, version and path somewhere along the way
  if (data.size() != 3)
  {
    return 1;
  }

  if (strcmp(data.front().c_str(), "GET") == 0)
  {
    req->method = GET;
    data.pop();
  }
  else
  {
    return -1;
  }

  // HTTP Path
  req->path = (char *)data.front().c_str();
  data.pop();
 
  // HTTP Version
  if (strcmp(data.front().c_str(), "HTTP/1.1"))
  {
    req->version = VER_11;
    data.pop();
  }
  else
  {
    return -1;
  }

  return 0;
}

