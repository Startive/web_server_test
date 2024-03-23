#include "connection.hpp"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>

void connection::error(std::string msg, int err_no) {
  std::cout << msg << std::endl;
  std::cout << "Errno: " << err_no << std::endl; 
  
  exit(1);
}

void connection::handle_connection(int client_fd) {
  /* Read Data That We Recieved */
  char buffer[512] = {0};
  read(client_fd, buffer, sizeof(buffer));
  
  char *file_to_open = buffer + 5;
  *strchr(file_to_open, ' ') = 0; // when we reach a space, we set the byte to \0
 
  std::cout << file_to_open << std::endl;

  // if requested file is "/", then change file_to_open to index.html
  const char *default_page = "index.html";
  if (file_to_open == "") {
    strcpy(file_to_open, default_page);
  }

  std::cout << "Requested file: " << file_to_open << std::endl;


  /* Respond To Client */
  int opened_file = open(file_to_open, O_RDONLY); // Open our file
  struct stat fst; // Create FSTAT structure to hold information about our file, however we will just use this for getting the amount of bytes in the file :)
  fstat(opened_file, &fst);
  int size = fst.st_size; // set size to the size in the fstat struture

  // Respond with HTTP headers here
  char response_ok[64];
  sprintf(response_ok, "HTTP/1.1 200 OK\r\nContent-Length: %i\r\n\r\n", size); // the ending double CRLF tells the client that the HTTP headers have ended, and it is now the file that is being sent.
  std::cout << "Responded with: " << response_ok << std::endl;

  if (send(client_fd, response_ok, strlen(response_ok), 0) == -1) {
    connection::error("Failed to send back data", errno);
  }

  if (sendfile(client_fd, opened_file, 0, size) == -1) {
    // 42 is size of file but is temp rn
    connection::error("Serving html failed", errno);
  }

  close(opened_file);
}
