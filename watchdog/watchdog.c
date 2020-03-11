#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <sys/signal.h>
#include <linux/watchdog.h>

//watchdog
#define WATCHDOG_IOCTL_BASE     'W'

int wdt_status= 1;
int wdt_fd;
int time_out = 5;

int main(int argc, char *argv[])
{
	int new_time;
	int i;
	int ret, count = 20;
	struct watchdog_info wdt_info;

	wdt_fd = open("/dev/watchdog0", O_RDWR);
	if(wdt_fd == -1)
		perror("Open Watchdog ERROR!\n");

	//get watchdog infomation struct
	ioctl(wdt_fd, WDIOC_GETSUPPORT, &wdt_info);
	printf("options=%d, identify=%s\n", wdt_info.options, wdt_info.identity);

	//set timeout
	ioctl(wdt_fd, WDIOC_SETTIMEOUT, &time_out);

	//read the timeout value
	ioctl(wdt_fd, WDIOC_GETTIMEOUT, &time_out);

	new_time = time_out;
	printf("time_value=%d\n", new_time);
#if 0
	//disable the watchdog
	i=WDIOS_DISABLECARD;
	printf("%d\n",ioctl(wdt_fd,WDIOC_SETOPTIONS,&i));

	//enable the watchdog
	i=WDIOS_ENABLECARD;
	printf("%d\n",ioctl(wdt_fd,WDIOC_SETOPTIONS,&i));
#endif
	while(1)
	{
		count--;

		//feed time
		if(wdt_status == 1)
		{
			ioctl(wdt_fd,WDIOC_KEEPALIVE,NULL);
			//write(wdt_fd,NULL,1);
			//feed time, too
			printf("Feed time!\n");

		}

		sleep(1);

		if(count == 0)
		{
			wdt_status = 0;
			printf("End of feeding time!\n");
		}
	}

	close(wdt_fd);
	return 0;
}

