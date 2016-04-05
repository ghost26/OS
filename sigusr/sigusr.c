#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void my_handler(int sig, siginfo_t *siginfo, void *context) {
	printf("SIGUSR%d from process id = %d\n", sig == SIGUSR1 ? 1 : 2, siginfo->si_pid);
	exit(0);
}

int main() {
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);
	sigaddset(&act.sa_mask, SIGUSR2);

	act.sa_sigaction = &my_handler; 
	if (sigaction(SIGUSR1, &act, NULL) || sigaction(SIGUSR2, &act, NULL)) {
		perror("Problems with setting handler at signal");
		return errno;
	}

	sleep(10);
	printf("No signals were caught\n");
	return 0;
}
