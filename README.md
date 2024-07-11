# Simple Multi-threaded HTTP Server

This is a simple multi-threaded HTTP server implemented in C. The server can handle multiple concurrent connections and serves different types of requests.

## Features

- Handles multiple concurrent connections using threads.
- Supports `GET` requests for:
  - Echoing back a message.
  - Serving files from a specified directory.
  - Displaying the `User-Agent` header.

## Prerequisites

- GCC (GNU Compiler Collection)
- POSIX-compliant operating system (e.g., Linux, macOS)

## Building the Server

  To build the server, run the following command in the terminal:
  
  ```sh
  gcc -pthread -o server server.c
  ```
## Running the Server
  To run the server, use the following command:

  ```sh
  ./server --directory <path_to_directory>
  ```
  Replace <path_to_directory> with the path to the directory where your files are stored.

  Example:
  
  ```sh
  ./server --directory /tmp
  ```
## Usage
  Basic HTTP Requests
  The server supports the following types of requests:

1. Echo Request:

    The server echoes back any message sent in the URL path.
    
    Example:
    
      ```sh
      curl http://localhost:4221/echo/HelloWorld
      ```
    
    Response:
    
      ```
      HelloWorld
      ```
2. File Request:

    The server serves files from the specified directory.

    Example:
      
      ```sh
      curl http://localhost:4221/files/filename.txt
      ```
    Response:
    
      Contents of filename.txt.

3.User-Agent Request:

  The server responds with the User-Agent header sent by the client.
  
  Example:
  
  ```sh
  curl http://localhost:4221/user-agent
  ```
  Response:
  
  Contents of the User-Agent header.

##  Example Commands

- Start the server and specify the directory:
  ```sh
  ./server --directory /path/to/files
  ```
- Send a request to echo a message:
  ```sh
  curl http://localhost:4221/echo/HelloWorld
  ```

- Send a request to get a file:
  ```sh
  curl http://localhost:4221/files/test.txt
  ```

- Send a request to get the User-Agent:

  ```sh
  curl http://localhost:4221/user-agent
  ```

## License
This project is licensed under the MIT License.
