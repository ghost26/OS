#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void cat(int fdes_in, int fdes_out) {
  char buffer[4096];
  ssize_t read_bytes = 0;
  while ((read_bytes = read(fdes_in, buffer, sizeof(buffer))) > 0) {
    ssize_t written_bytes = 0;
    while (written_bytes < read_bytes) {
      ssize_t cur = write(fdes_out, buffer + written_bytes, read_bytes - written_bytes);
      if (cur >= 0) {
        written_bytes += cur;
      } else {
        fprintf(stderr, "%s\n", strerror(errno));
        return;
      }
    }
  }
  if (read_bytes == -1) {
    fprintf(stderr, "%s\n", strerror(errno));
  }
}

int main(int argc, char **argv)
{
  cat(STDIN_FILENO, STDOUT_FILENO);
  return 0;
}
