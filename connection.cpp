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

void *connection::handle_connection(void *client) {
  pthread_detach(pthread_self()); // deatch our thread from what called it

  int *client_fd = (int*)(client); // cast the value from the void pointer

  /* Read Data That We Recieved */
  char buffer[512] = {0};
  read(*client_fd, buffer, sizeof(buffer));
  
  char *file_to_open = buffer + 5;
  *strchr(file_to_open, ' ') = 0; // when we reach a space, we set the byte to \0

  // if requested file is "/", then change file_to_open to index.html
  const char *default_page = "index.html";
  if (file_to_open[0] == '\0') {
    strcpy(file_to_open, default_page);
  }

  std::cout << "Requested file: " << file_to_open << std::endl;


  /* Respond To Client */
  char HTTP_STATUS[32];

  int opened_file = open(file_to_open, O_RDONLY); // Open our file
  
  // If file fails to open, which most of the time will only be caused by it not existing, set the file to 404.html.
  if (opened_file == -1) {
    opened_file = open("404.html", O_RDONLY);
    strcpy(HTTP_STATUS, "HTTP/1.1 404 NOT FOUND");
  } else {
    strcpy(HTTP_STATUS, "HTTP/1.1 200 OK");
  }

  struct stat fst; // Create FSTAT structure to hold information about our file, however we will just use this for getting the amount of bytes in the file :)
  fstat(opened_file, &fst); // populate the structure
  int size = fst.st_size; // set size to the size in the fstat struture

  // Respond with HTTP headers here
  char response[64];

  sprintf(response, "%s\r\nContent-Length: %i\r\n\r\n", HTTP_STATUS, size); // the ending double CRLF tells the client that the HTTP headers have ended, and it is now the file that is being sent.
  std::cout << "Responded with: " << response << std::endl;

  if (send(*client_fd, response, strlen(response), 0) == -1) { // send back HTTP Headers
    connection::error("Failed to send back data", errno);
  }


  // Send File
  if (sendfile(*client_fd, opened_file, 0, size) == -1) {
    connection::error("Serving data failed", errno);
  }


  /* Cleanup */
  close(opened_file);
  close(*client_fd);

  pthread_exit(NULL);
}
