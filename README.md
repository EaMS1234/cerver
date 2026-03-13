# cerver
Simple HTTP server in C++. Can serve static files like HTML, JavaScript, CSS and images inside a folder, respecting the directory structure of the host.

## Dependencies
- Linux-only (you will need the `sys` library)
- GCC (tested on 15.2.1, should work on older versions too)

## Build
- `make` to build the `cerver` binary file

## Run
You should have a folder named `html/` on the same directory as the `cerver` file. Run `./cerver` to create the sockets and start waiting for connections.

### Changing the default port
For default the server will listen on the port `8080`. You can change this by altering the line `err = open_socket(&sock, 8080);` in the `src/main.cpp` file before building.

## Limitations
- This is still a work in progress. For now it still ignores HTTP headers in the request, and only implements a few in the response;
- It sucessfully parses `.html`, `.js`, `.css`, `.xml`, `.png`, `.jpg` or `.jpeg` and `.gif` files with the correct `Content-Type` header, but everything else falls into `application/octet-stream` binaries;
- Routes are static and directly translate to the directory structure inside the `html` folder.
