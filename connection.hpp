#pragma once
#include <sys/socket.h>
#include <string>

namespace connection {
  void error(std::string msg, int err_no);

  void *handle_connection(void *client);
}
