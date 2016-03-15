#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

const size_t BUFFER_SIZE = 4096;

int cat(int fdes_in, int fdes_out) {
  char buffer[BUFFER_SIZE];
  ssize_t read_bytes = 0;
  while (1) {
    read_bytes = read(fdes_in, buffer, sizeof(buffer));
    if (read_bytes < 0) {
      if (errno == EINTR) {
        continue;
      }
      break;
    } else if (read_bytes == 0) {
      break;
    }
    ssize_t written_bytes = 0;
    while (written_bytes < read_bytes) {
      ssize_t cur = write(fdes_out, buffer + written_bytes, read_bytes - written_bytes);
      if (cur >= 0) {
        written_bytes += cur;
      } else {
        if (errno == EINTR) {
          continue;
        }
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }
    }
  }
  if (read_bytes == -1) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }
  return 0;
}

int main(int argc, char **argv)
{
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      int filedes = 0;
      if ((filedes = open(argv[i], O_RDONLY)) >= 0) {
        if (cat(filedes, STDOUT_FILENO) == -1) {
          return -1;
        }
      } else {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }
      if (close(filedes) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
      }
    }
  } else {
    if (cat(STDIN_FILENO, STDOUT_FILENO) == -1) {
      return -1;
    }
  }
  return 0;
}
