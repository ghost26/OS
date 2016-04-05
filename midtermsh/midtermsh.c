#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

struct sigaction prev;
pid_t childs_pid[4096];
ssize_t childs_count = 0;

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

struct execargs {
    int argc;
    char **argv;
};

struct execargs *build_execargs(int argc, char **argv) {
    struct execargs *args = malloc(sizeof(struct execargs));
    args->argc = argc;
    args->argv = malloc(sizeof(char*) * (argc));
    int i;
    for (i = 0; i < argc; i++) {
        args->argv[i] = argv[i];
    }
    return args;
}

int stopall() {
  int i = 0;
  for (i = 0; i < childs_count; i++) {
    kill(childs_pid[i], SIGTERM);
    waitpid(childs_pid[i], NULL, 0);
  }
  return -1;
}

void sig_for_forked(int sig) {
  stopall();
}

void sig_handler(int sig) {
    if (sig == SIGINT) {
        write_(STDOUT_FILENO, "\n$ ", 3);
    }
}

int exec(struct execargs *args) {
    signal(SIGINT, SIG_DFL);
    execvp(args->argv[0], args->argv);
    return -1;
}

int run_programs(struct execargs** progs, size_t progs_count) {
  if (progs_count == 0) {
    return 0;
  }
  struct sigaction sig_act;
  sig_act.sa_handler = sig_for_forked;
  sig_act.sa_flags = 0;
  char *parts[255];
  int fpipe[2];
  int spipe[2];
  int i;
  for (i = 0; i < progs_count; i++) {
      memset(parts, 0, sizeof(parts));
      pipe2(spipe, O_CLOEXEC);
      pid_t newpid = fork();
      if (newpid < 0) {
          perror("Fork failed\n");
      } else {
          childs_pid[childs_count++] = newpid;
      }
      if (newpid == 0) {
        if (i > 0) {
          dup2(fpipe[0], 0);
          close(fpipe[0]);
          close(fpipe[1]);
        }
        if (i != progs_count - 1) dup2(spipe[1], 1);
          exec(progs[i]);
      } else {
        if (i > 0) {
      	   close(fpipe[0]);
           close(fpipe[1]);
      	}
      }
      fpipe[0] = spipe[0];
      fpipe[1] = spipe[1];
    }
	int j = 0;
	for(;j < childs_count; j++) {
	waitpid(childs_pid[j], 0, 0);
	}
    return 0;
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
    signal(SIGINT, sig_handler);
    memset(buf, 0, sizeof(buf));
    write_(STDOUT_FILENO, "$ ", 2);
    ssize_t read_bytes = read_(STDIN_FILENO, buf, sizeof(buf));
    char* tokens[4096];
    size_t len = split(buf, "|\n", tokens);
    size_t i = 0;
    struct execargs *programs[4096];
    for(;  i < len; i++) {
      char* argg[4096];
      int prog_argc = split(tokens[i], " \n", argg);
      programs[i] = build_execargs(prog_argc, argg);
    }
    int res = run_programs(programs, len);
  }
  return 0;
}
