#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

int64_t current_millis();
void timetostr(const time_t ltime,char *stime);

int main()
{
	char str[30];
	
	time_t ms = current_millis();
	printf("ms = %ld\n", ms);

	timetostr(ms, str);
	printf("localtime = %s\n", str);

	return 0;
}

int64_t current_millis()
{
	struct timeval timeMs;
	gettimeofday(&timeMs, 0);
	return timeMs.tv_usec / 1000 + timeMs.tv_sec * 1000;
}

void timetostr(const time_t ltime,char *stime)
{
	char str[30], str1[30];
	time_t ms;
	time_t Ltime;

	sprintf(str, "%ld", ltime);
	strcpy(str1, str+strlen(str)-3);
	str[strlen(str) - 3] = '\0';

	Ltime=atoi(str);
	ms = atoi(str1);
	if (stime==0) return;

	strcpy(stime,"");
	
	struct tm sttm = *localtime( &Ltime );
	
	sttm.tm_year=sttm.tm_year+1900;
	sttm.tm_mon++;
	snprintf(stime,26,"%04u-%02u-%02u %02u:%02u:%02u:%ums",sttm.tm_year,
                    sttm.tm_mon,sttm.tm_mday,sttm.tm_hour,
                    sttm.tm_min,sttm.tm_sec, ms);
    return;
}
