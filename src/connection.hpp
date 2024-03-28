#pragma once
#include <sys/socket.h>
#include <string>

namespace connection {
  void error(std::string msg);

  // Response structure that is populated by get_response
  typedef struct {
  char HTTP_RESPONSE[512];
  int content_length;
  int file_socket;
  } Response;

  // Takes in HTTP headers and populates the connection::Response structure
  void get_response(char *headers, connection::Response *response); // the code in this is REALLY shitty cause i overcomplicated it

  void cleanup(int fd1, int fd2);

  void *handle_connection(void *client);
}
