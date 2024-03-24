#pragma once
#include <sys/socket.h>
#include <string>

namespace connection {
  void error(std::string msg, int err_no);

  char *get_file_from_headers(char headers[512]);

  void cleanup(int fd1, int fd2);

  void *handle_connection(void *client);
}
