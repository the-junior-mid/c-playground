#include <asm-generic/errno-base.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/wait.h>
#include <err.h>
// #include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/rcs"
#define BACKLOG 50


typedef struct {
  int rc;
  char *stdout;
  char *stderr;
} Output;

typedef struct {
  uint16_t port;
  bool shell;
} RCmdOpts;

/// Parse the command line options. Sets defauts if none are given.
int parse_opts(int argc, char *argv[], RCmdOpts *opts) {
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

int run_command(char *cmd, bool shell, Output *out) {
  printf("Running command: %s\n", cmd);
  pid_t pid = fork();
  // Check the fork succeeded
  if (pid < 0) {
    fprintf(stderr, "could not run fork: %s", strerror(errno));
    return 1;
  }

  // From here only the child should run the code
  if (pid == 0) {
    char *argv[] = {"whoami"};
    execvp(argv[0], argv);
    fprintf(stderr, "could not call execvp: %s", strerror(errno));
    return 1;
  }

  int status;
  if (waitpid(pid, &status, 0) < 0) {
    fprintf(stderr, "waitpid failed: %s", strerror(errno));
    return 1;
  } else if (WIFEXITED(status)) {
    out->rc = WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    printf("Terminated by signal: %d\n", WTERMSIG(status));
  }

  return 0;
}

int handler(int peer, bool shell) {
  char buffer[4096] = { 0 };
  size_t buffer_len = sizeof(buffer) / sizeof(buffer[0]);

  for (;;) {
    size_t n = recv(peer, buffer, buffer_len - 1, 0);
    if (n > 0) {
      printf("Buffer: %s\n", buffer);
      Output out;
      memset(&out, 0, sizeof(out));
      if (run_command(buffer, shell, &out) < 0) {
        fprintf(stderr, "ERROR: could not run command: %s", strerror(errno));
      }
      printf("Output: \n\t%d\n\t%s\n\t%s\n", out.rc, out.stdout, out.stderr);
      // Clear the buffer
      memset(&buffer, 0, buffer_len);
    } else if (n == 0) {
      break;
    } else {
      printf("Error occurred: %s", strerror(errno));
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

  if (handler(peer, shell) < 0) {   
    close(socket_fd);

    err(EXIT_FAILURE, "TCP handler encountered an error");
  }

  close(socket_fd);

  return 0;
}


int main(int argc, char *argv[])
{
  RCmdOpts opts;
  if (parse_opts(argc, argv, &opts) != 0) {
    return EXIT_FAILURE;
  }
        
  printf("RCmdOpts: %d, %d\n", opts.port, opts.shell);

  if ((run(opts.port, opts.shell)) < 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
