#include <cstring>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "../include/http.hpp"


void message(int socket, std::vector<route> routes)
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
    response res;  // Stores the data for the response

    // clears the buffer
    memset(buffer, 0, 1024);

    if (recvmsg(socket, &msg, 0) > 0)
    {
      err = parserequest(&req, buffer);
      if (err != 0)
      {
        res.status = 400;
        res.content = "400: Bad Request";
        res.connection = "close";
        respond(socket, &res);
        break;
      }
      else
      {
        std::cout << buffer << std::endl;

        res.status = 404;
        res.content = "404: Not Found";

        for (route r : routes)
        {
          // If the path of the request contains the route's URL as a prefix
          if (req.path.compare(0, r.URL.size(), r.URL) == 0)
          {
            if (r.function == "serve_directory")
            {
              req.path.erase(0, r.URL.size() - 1);
              serve_directory(&req, &res, &r);
            }
            
            if (r.function == "serve_file")
            {
              req.path = r.path;
              serve_file(&req, &res, &r);
            }

            break;
          } 
        }

        respond(socket, &res);

        if (res.connection == "close")
        {
          break;
        }
      }
    }
    else
    {
      break;
    }
  }

  std::cout << "Connection closed" << std::endl;
  close(socket);
}


int parserequest(request * req, char * message)
{
  char buffer[1024] = {0};
  std::queue<std::string> data;  // Three elements (front to back): Method, Path, Version
                                
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
  req->path = data.front();
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

void respond(int socket, response * resp)
{
  std::string text;

  text = "HTTP/1.1 ";
  text.append(std::to_string(resp->status));
  text.append("\nServer: Cerver\nContent-Length: ");
  text.append(std::to_string(resp->content.length()));

  if (resp->status < 400 && resp->status >= 300)
  {
    text.append("\nLocation: ");
    text.append(resp->location);
  }

  if (!resp->connection.empty())
  {
    text.append("\nConnection: ");
    text.append(resp->connection);
  }
  else
  {
    text.append("\nConnection: keep-alive");
  }

  if (!resp->content_type.empty())
  {
    text.append("\nContent-Type: ");
    text.append(resp->content_type);
  }
  else
  {
    text.append("\nContent-Type: text/plain");
  }
  
  text.append("\n\n");
  text.append(resp->content);

  send(socket, text.c_str(), text.length(), 0);
}

void serve_directory(request *req, response *res, route * r)
{
  std::string path = std::filesystem::canonical(r->path);
  
  req->path.insert(0, path);

  // If the URL is a directory, tries to get the index.html file inside it
  if (req->path.back() == '/')
  {
    req->path.append("index.html");
  }
  else if (std::filesystem::is_directory(req->path))
  {
    // Redirect the request to the directory
     
    req->path.append("/");
    res->status = 301;
    res->location = req->path;
    res->location.erase(0, path.size() + 1);
    res->location.insert(0, r->URL);

    return;
  }

  if (!std::filesystem::exists(req->path))
  {
    res->status = 404;
    res->content = "404: Not Found";

    return;
  }
  else if (std::filesystem::canonical(req->path).string().rfind(path, 0) != 0)
  {
    // Checks if the path is inside the 'root' of the server. If not, Bad Request.

    res->status = 400;
    res->content = "400: Bad Request";
    res->connection = "close";
  }
  else
  {
    serve_file(req, res, r);
  }
}

void serve_file(request *req, response *res, route * r)
{
  std::string path = std::filesystem::canonical(req->path);
  std::ifstream file;  // The file to be served
 
  file.open(path);
  
  if (!file.is_open())
  {
    res->status = 404;
    res->content = "404: Not Found";

    return;
  }

  res->content = "";
  
  // Saves the file in the response
  std::string line;
  while (std::getline(file, line))
  {
    res->content.append(line);
    res->content.append("\n");
  }

  res->status = 200;

  // checks the type of file
  std::string extension = std::filesystem::path(req->path).extension().string();

  if (extension == ".html")
  {
    res->content_type = "text/html";
  }
  else if (extension == ".js")
  {
    res->content_type = "text/javascript";
  }
  else if (extension == ".css")
  {
    res->content_type = "text/css";
  }
  else if (extension == ".xml")
  {
    res->content_type = "text/xml";
  }
  else if (extension == ".png")
  {
    res->content_type = "image/png";
  }
  else if (extension == ".jpg" || extension == ".jpeg")
  {
    res->content_type = "image/jpeg";
  }
  else if (extension == ".gif")
  {
    res->content_type = "image/gif";
  }
  else if (extension == ".txt" || extension == ".md" || !std::filesystem::path(req->path).has_extension())
  {
    res->content_type = "text/plain";
  }
  else
  {
    res->content_type = "application/octet-stream";
  }
}

