#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "execute.h"

int execute(const char *cmd)
{
	pid_t pid = fork();
	if (pid < 0) {
		return -1;
	} else if (pid == 0) {	/* child */
		execl("/bin/sh", "sh", "-c", cmd, (char *)0);	/* execute the command */
		/* the execl() functions return only if an error has occurred */
		exit(EXIT_FAILURE);
	}

	/* parent */
	int status;
	int rc = waitpid(pid, &status, 0);

	if (-1 == rc)
		return -1; /* waitpid failed */

	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	} else {
		/* if we get here, child did not exit cleanly */
		return -1;
	}
}
