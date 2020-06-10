#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define CLOCKID CLOCK_REALTIME

void printTime(){
	struct tm *cursystem;
	time_t tm_t;
	time(&tm_t);
	cursystem = localtime(&tm_t);
	char tszInfo[2048] ;
	sprintf(tszInfo, "%02d:%02d:%02d", cursystem->tm_hour,
		cursystem->tm_min, cursystem->tm_sec);
	printf("[%s]",tszInfo);
}

void timer_thread(union sigval v)
{
	printTime();
	printf("timer_thread function! %d\n", v.sival_int);
}

int main()
{
	timer_t timerid;
	struct sigevent evp;
	memset(&evp, 0, sizeof(struct sigevent));

	evp.sigev_value.sival_int = 111;
	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = timer_thread;

	if (timer_create(CLOCKID, &evp, &timerid) == -1) {
		perror("fail to timer_create");
		exit(-1);
	}

	struct itimerspec it;
	it.it_interval.tv_sec = 1;
	it.it_interval.tv_nsec = 0;
	it.it_value.tv_sec = 3;
	it.it_value.tv_nsec = 0;

	if (timer_settime(timerid, 0, &it, NULL) == -1)
	{
		perror("fail to timer_settime");
		exit(-1);
	}
	pause();

	return 0;
}
