#include <asm-generic/errno-base.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <err.h>
// #include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/rcs"
#define BACKLOG 50


struct RCmdOpts {
  uint16_t port;
  bool shell;
};

/// Parse the command line options. Sets defauts if none are given.
int parse_opts(int argc, char *argv[], struct RCmdOpts *opts) {
  // Set defaults
  opts->port = 5599;
  opts->shell = false;

  static struct option long_opts[] = {
    {"port",  required_argument, NULL, 'p'},
    {"shell", no_argument, NULL, 's'},
    {"help",  no_argument, 0, 0},
    {0, 0, 0, 0},
  };

  int c;
  while ((c = getopt_long(argc, argv, "p:sh", long_opts, NULL)) != -1) {
    switch (c) {
      case 0: break;
      case 'p': opts->port = atoi(optarg); break;
      case 's': opts->shell = true; break;
      case 'h':
        printf("Usage: %s --port <PORT> --shell\n", argv[0]);
        return 0;
      case '?':
        break;
    }
  }

  return 0;
}

int handler(int peer) {
  char buffer[4096] = { 0 };
  size_t buffer_len = sizeof(buffer) / sizeof(buffer[0]);

  for (;;) {
    size_t n = recv(peer, buffer, sizeof(buffer), 0);
    if (n > 0) {
      printf("Buffer: %s\n", buffer);
    } else if (n == 0) {
      break;
    } else {
      printf("Error occurred: %d", errno);
      if (errno == EINTR) continue;
      close(peer);
      perror("Could not read from peer");

      return 1;
    }  
  }
  
  return 0;
}

int run(uint16_t port, bool shell) {
  int socket_fd, peer;
  int opt = 1;
  struct sockaddr_in address;
  // Clear out the address
  memset(&address, 0, sizeof(address));
  socklen_t addrlen = sizeof(address);

  // Create the socket
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    close(socket_fd);

    err(EXIT_FAILURE, "Could not create socket");
  }

  // Set socket options
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    close(socket_fd);

    err(EXIT_FAILURE, "Could not set socket options");
  }

  // Bind the socket
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);
  if ((bind(socket_fd, (struct sockaddr*)&address, sizeof(address))) < 0) {
    close(socket_fd);

    err(EXIT_FAILURE, "Could not bind to socket");
  }

  // Listen for connections
  if (listen(socket_fd, 3) < 0) {
    close(socket_fd);

    err(EXIT_FAILURE, "Could not listen on socket");
  }

  if ((peer = accept(socket_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
    close(socket_fd);

    err(EXIT_FAILURE, "Could not accept connection on socket %d", socket_fd);
  }

  if (handler(peer) < 0) {
    close(peer);
    close(socket_fd);

    err(EXIT_FAILURE, "TCP handler encountered an error");
  }

  close(peer);
  close(socket_fd);

  return 0;
}


int main(int argc, char *argv[])
{
  struct RCmdOpts opts;
  if (parse_opts(argc, argv, &opts) != 0) {
    return EXIT_FAILURE;
  }
        
  printf("RCmdOpts: %d, %d\n", opts.port, opts.shell);

  if ((run(opts.port, opts.shell)) < 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
