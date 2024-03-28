#include "connection.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

// HTTP codes and files
const char *HTTP_404 = "404 NOT FOUND";
const char *HTTP_404_PAGE = "404.html";

const char *HTTP_400 = "400 BAD REQUEST";
const char *HTTP_400_PAGE = "400.html";

const char *HTTP_200 = "200 OK";
const char *default_page = "index.html";


void connection::error(std::string msg) {
  std::cout << msg << std::endl;
  std::cout << "Errno: " << errno << std::endl; 
  
  exit(1);
}


void connection::get_response(char *headers, connection::Response *response) {
  /*Initalize variables, HTTP response codes and HTML strings */

  char HTTP_STATUS[32];
  int file_socket;
  int content_length;

  /* Get File To Open */
  char file_name[32];

  strcpy(file_name, headers + 5); // Get file name that is being requested

  // fix user doing unreasonable URL (does not work for now, crashes server :( )
  if (strchr(file_name, '\0') == 0) {
    strcpy(file_name, HTTP_400_PAGE);
    strcpy(HTTP_STATUS, HTTP_400);
  } else {
    *strchr(file_name, ' ') = 0;
    strcpy(HTTP_STATUS, HTTP_200);
  }

  // Set page to default page if accessed url is blank / "/"
  if (file_name[0] == '\0') {
    strcpy(file_name, default_page);
  }

  /* Open File And Get Information About It */
  file_socket = open(file_name, O_RDONLY);

  if (file_socket == -1) { // we check if it's -1 cause it would prob mean that the requested file does not exist
    file_socket = open(HTTP_404_PAGE, O_RDONLY);
    strcpy(HTTP_STATUS, HTTP_404);
  }
  else {
    strcpy(HTTP_STATUS, HTTP_200);
  }

  struct stat fst; // Create FSTAT structure to hold info about file
  fstat(file_socket, &fst); // Populate the fst structure
  content_length = fst.st_size; // set content_length to the size in the fstat struture
  

  /* Sprintf The Response */
  char HTTP_RESPONSE[512] = {0}; // Doesn't need to be this big, just so when we add more headers it can be good i guess
  sprintf(HTTP_RESPONSE, "HTTP/1.1 %s\r\nContent-Length: %i\r\n\r\n", HTTP_STATUS, content_length); // the ending double CRLF tells the client that the HTTP headers have ended, and it is now the file that is being sent.
  

  /* Set Response Vars In Structure Appropriately */
  strcpy(response->HTTP_RESPONSE, HTTP_RESPONSE);
  response->content_length = content_length;
  response->file_socket = file_socket;

  // Debug
  std::cout << "File being sent: " << file_name << ", http text response being sent: " << HTTP_RESPONSE << std::endl;
}


void connection::cleanup(int fd1, int fd2) {
  close(fd1);
  close(fd2);
}


void *connection::handle_connection(void *client) {
  pthread_detach(pthread_self()); // deatch our thread from what called it

  int *client_fd = (int*)(client); // cast the value from the void pointer


  /* Read Data That We Recieved */
  char buffer[4096];
 /* 
  // Peak at message
  ssize_t bytes_read = recv(*client_fd, buffer, sizeof(buffer), MSG_PEEK);
  if (bytes_read == -1) {
    connection::error("Failed to peak at msg");
  }
*/
  // Read Data Normally
  //std::cout << bytes_read << std::endl;
  if (recv(*client_fd, buffer, sizeof(buffer), 0) == -1) {
    connection::error("Failed to recieve messages from client properly.");
  }


  /* Pass Buffer Into Function With Struct*/
  connection::Response response;
  connection::get_response(buffer, &response);


  /* Respond To Client */
  if (send(*client_fd, response.HTTP_RESPONSE, strlen(response.HTTP_RESPONSE), 0) == -1) { // send back HTTP Headers
    connection::error("Failed to send back data");
  }

  // Send File
  if (sendfile(*client_fd, response.file_socket, 0, response.content_length) == -1) {
   connection::error("Serving data failed");
  }


  /* Cleanup */
  connection::cleanup(response.file_socket, *client_fd);
  
  pthread_exit(NULL);
}
