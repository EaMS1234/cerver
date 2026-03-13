all: build clean

http.a:
	g++ -c src/http.cpp -o http.a

server.a:
	g++ -c src/server.cpp -o server.a

build: server.a http.a
	g++ src/main.cpp server.a http.a -Iinclude -o cerver

clean: build
	rm ./*.a

