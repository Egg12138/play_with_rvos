#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

pid_t Fork(void);

int main(void)
{

	pid_t pid;
	int x = 1;
	
	pid = Fork();

	if (pid >= 0) {
		printf("chid[%d](parent[%d])x=%d\n", getpid(), getppid(), ++x);
	}	

	printf("parent[%d]: x=%d\n", getpid(),--x);

	return 0;
}




pid_t Fork(void) {

	pid_t  pid;
	if ((pid == fork()) < 0) {
		fprintf(stderr, "Failed to fork the process.\n");
		exit(0);
	}
	return pid;




}
