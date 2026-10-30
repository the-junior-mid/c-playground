#include <asm-generic/socket.h>
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
  int port;
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

int run(int port, bool shell) {
  int socket_fd, peer;
  int opt = 1;
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);

  if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    err(EXIT_FAILURE, "Could not create socket");
  }
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    err(EXIT_FAILURE, "Could not set socket options");
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if ((bind(socket_fd, (struct sockaddr*)&address, sizeof(address))) < 0) {
    err(EXIT_FAILURE, "Could not bind to socket");
  }
  if (listen(socket_fd, 3) < 0) {
    err(EXIT_FAILURE, "Could not listen on socket");
  }

  ssize_t bytes_read;
  char buffer[1024] = { 0 };
  size_t buffer_len = sizeof(buffer) / sizeof(buffer[0]);
  if ((peer = accept(socket_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
    err(EXIT_FAILURE, "Could not accept connection on socket %d", socket_fd);
  }
  // Read up to the second to last character for null term room
  bytes_read = read(peer, buffer, buffer_len - 1);
  printf("buffer: %s\n", buffer);

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
