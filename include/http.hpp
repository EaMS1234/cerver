#include <string>
#include <vector>


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
  std::string path;
}
request;

// TODO: Status codes and headers
typedef struct
{
  int status;
  http_version version;
  std::string content;

  std::string content_type;
  std::string location;
  std::string connection;
}
response;

typedef struct
{
  std::string URL;
  std::string function;
  std::string path;
}
route;


void message(int socket, std::vector<route> routes);
int parserequest(request * req, char * message);
void respond(int socket, response * resp);

void serve_directory(request * req, response * res, route * r);
void serve_file(request * req, response * res, route * r);

