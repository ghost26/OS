#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

struct execargs_t {
    int argc;
    char **argv;
};

struct execargs_t *build_execargs(int argc, char **argv) {
    struct execargs_t *args = malloc(sizeof(struct execargs_t));
    args->argc = argc;
    args->argv = malloc(sizeof(char*) * (argc+3));
    int i;
    for (i = 0; i < argc; i++) {
        args->argv[i] = argv[i];
    }
    return args;
}

ssize_t write_(int fd, const void *buf, size_t count) {
    ssize_t total_written = 0;
    ssize_t now_written;
    while (total_written < count && (now_written = write(fd, buf + total_written, count - total_written)) > 0) {
        total_written += now_written;
    }
    return total_written;
}

ssize_t read_(int fd, void *buf, size_t count) {
    int total_read = 0;
    int now_read;
    while (total_read < count && (now_read = read(fd, buf + total_read, count - total_read)) > 0) {
        total_read += now_read;
        if (*(char*)(buf + total_read - 1) == '\n') {
          break;
        }
    }
    return total_read;
}

void sig_handler(int sig) {
    if (sig == SIGINT) {
        write_(STDOUT_FILENO, "\n$ ", 3);
    }
}

size_t split(char* str, char* separator, char* tok[]) {
  char * pch = NULL;
  pch = strtok (str, separator);
  size_t token_count = 0;
  while (pch != NULL)
  {
    tok[token_count] = malloc(sizeof(char) * strlen(pch));
    memcpy(tok[token_count], pch, strlen(pch) * sizeof(char));
    token_count++;
    pch = strtok (NULL, separator);
  }
  return token_count;
}

int main(int argc, char **argv) {
  char buf[4096];
  while(1) {
    memset(buf, 0, sizeof(buf));
    write_(STDOUT_FILENO, "$ ", 2);
    ssize_t read_bytes = read_(STDIN_FILENO, buf, sizeof(buf));
    char* tokens[4096];

    size_t len = split(buf, "|\n", tokens);
    size_t i = 0;
    for(;  i < len; i++) {
      struct execargs_t *programs[255];
      char* argg[4096];
      int prog_argc = split(tokens[i], " \n", argg);
      programs[i] = build_execargs(prog_argc, argg);
      system(argg[1]);
    }

    signal(SIGINT, sig_handler);
  }
  return 0;
}
