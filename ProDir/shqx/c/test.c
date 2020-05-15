#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int g_lob = 6;

int main()
{
	int var = 88;
	int pid;
	printf("before vfork.\n");
	if ((pid = vfork())<0)
		//err_sys("vfork error");
		{ printf("vfork error\n"); return 0; }
	else if (0 == pid) {
		++g_lob;
		++var;
		_exit(0);
	}

	printf("pid:%d, g_lob:%d, var:%d\n", (long)pid, g_lob, var);
	exit(0);

	return 0;
}
