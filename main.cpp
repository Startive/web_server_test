#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/sendfile.h>

#define PORT 8080

void error(std::string msg, int err_no) {
  std::cout << msg << std::endl;
  std::cout << "Errno: " << err_no << std::endl; 
  
  exit(1);
}

int main(int argc, char **argv) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0); // create socket

  if (socket_fd == -1) {
    error("Failed to create socket", errno);
  }

  const int enable = 1; // weird C stuff in these 2 lines, but it works so what else could you have asked for?
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) { // makes it so address can be reused
    error("Setsockopt failed", errno);
  }


  /* Binding */
  struct sockaddr_in addr = {
    AF_INET,
    htons(PORT),
    0
  };
  
  if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) { // bind to 8080
    error("Failed to bind to port 8080", errno);
  }

  
  /* Listening */
  if (listen(socket_fd, 10) == -1) {
    error("Failed to listen for incoming connections", errno);
  }

  /* Main Loop */
  while (true) {

   /* Accepting Connections */
   struct sockaddr socket_peer;
   socklen_t peer_len;

   int client_fd = accept(socket_fd, &socket_peer, &peer_len); 
   if (client_fd == -1) {
     error("Accepting failed", errno);
   }

  
   /* Read Data That The Client Sent To Us */
   char buffer[512] = {0};
   read(client_fd, buffer, sizeof(buffer));

   char *file_to_open = buffer + 5; // get the file to open
   *strchr(file_to_open, ' ') = 0; //
   std::cout << file_to_open << std::endl;
    
   // open file and serve to user (will put all this into another function soon called "handle_connection()")
   int opened_file = open(file_to_open, O_RDONLY);
   sendfile(client_fd, opened_file, 0, 256); // 256 is meant to be the size of the file, change later to make it accurate

  }

  return 0;
}
