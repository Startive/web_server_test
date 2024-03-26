#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "connection.hpp"

#define PORT 8080


int main(int argc, char **argv) {
  /* To prevent sigpipe from stopping program */
  //sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);
  /*                                          */

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0); // create socket

  if (socket_fd == -1) {
    connection::error("Failed to create socket", errno);
  }

  const int enable = 1; // weird C stuff in these 2 lines, but it works so what else could you have asked for?
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) { // makes it so address can be reused
    connection::error("Setsockopt failed", errno);
  }


  /* Binding */
  struct sockaddr_in addr = {
    AF_INET,
    htons(PORT),
    0
  };
  
  if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) { // bind to 8080
    connection::error("Failed to bind to port 8080", errno);
  }

  
  /* Listening */
  if (listen(socket_fd, 0) == -1) {
    connection::error("Failed to listen for incoming connections", errno);
  }

  std::cout << "Listening on http://127.0.0.1:" << PORT << std::endl;

  /* Main Loop */
  while (true) {

   /* Accepting Connections */
   struct sockaddr socket_peer;
   socklen_t peer_len;

   int client_fd = accept(socket_fd, &socket_peer, &peer_len); 
   if (client_fd == -1) {
     connection::error("Accepting failed", errno);
   }
  

   /* Create New Thread */
   pthread_t thread_id; // store our thread id
   pthread_create(&thread_id, NULL, connection::handle_connection, (void*)&client_fd);
   pthread_join(thread_id, NULL); // waits for thread to execute
  }

  return 0;
}
