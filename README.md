# cerver
Simple HTTP server in C++. Can serve static files like HTML, JavaScript, CSS and images inside a folder, respecting the directory structure of the host.

## Dependencies
- Linux-only (you will need the `sys` library)
- GCC (tested on 15.2.1, should work on older versions too)

## Build
- `make` to build the `cerver` binary file

## Run
You should have a folder named `html/` on the same directory as the `cerver` file. Run `./cerver` to create the sockets and start waiting for connections on `http://localhost:8080/`.

### Changing the default port
Use `cerver --port <number>` when running to change the port to something else.

### Changing the directory
Use `cerver <folder>` to serve the files in another directory (The user running the command must have access to the folder).

## Limitations
- This is still a work in progress. For now it still ignores HTTP headers in the request, and only implements a few in the response;
- It sucessfully parses `.html`, `.js`, `.css`, `.xml`, `.png`, `.jpg` or `.jpeg`, `.gif` and plain text files with the correct `Content-Type` header, while everything else falls into `application/octet-stream` binary files category;
- Routes are static and directly translate to the directory structure inside the `html` folder;
- No HTTPS.
