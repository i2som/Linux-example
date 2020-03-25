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

int time_out = 5;
static const char *device = "/dev/watchdog";
static int enable = 1;
static int keepalive = 20;

int main(int argc, char *argv[])
{
	int new_time;
	int i, ret;
	struct watchdog_info wdt_info;
	int wdt_fd;
	int opts = 0;

	parse_opts(argc, argv);
	wdt_fd = open(device, O_RDWR);
	if(wdt_fd < 0) {
		perror("Open Watchdog failed!\n");
		return -1;
	};

	if(enable == 0) {
		// needs write 'V' before disable the watchdog, it means WDT_OK_TO_CLOSE
		if (write(wdt_fd, "V", 1) != 1) {
			printf("write WDT_OK_TO_CLOSE failed!");
		}
		opts = WDIOS_DISABLECARD;
		ret = ioctl(wdt_fd, WDIOC_SETOPTIONS, &opts);
		printf("watchdog disable!\r\n");

	} else {
#if 0
		//enable the watchdog
		i=WDIOS_ENABLECARD;
		printf("%d\n",ioctl(wdt_fd,WDIOC_SETOPTIONS,&i));
#endif

		//get watchdog infomation struct
		ioctl(wdt_fd, WDIOC_GETSUPPORT, &wdt_info);
		printf("options=%d, identify=%s\n", wdt_info.options, wdt_info.identity);

		//set timeout
		ioctl(wdt_fd, WDIOC_SETTIMEOUT, &time_out);

		//read the timeout value
		ioctl(wdt_fd, WDIOC_GETTIMEOUT, &time_out);

		new_time = time_out;
		printf("time_value=%d\n", new_time);

		if(keepalive > 0) {
			for(i = 0; i < keepalive; i++) {
				ret = ioctl(wdt_fd, WDIOC_KEEPALIVE, NULL);
				printf("Feed the watchdog! keepalive = %d, ret = %d\n", i + 1, ret);
				sleep(1);
			}
		}
	};

	ret = close(wdt_fd);
	if(ret != 0) {
		printf("the watchdog device close failed, watchdog will keep on!\r\n");
	};

	return ret;
}

void print_usage(const char *prog)
{
	printf("Usage: %s [-desk]\r\n", prog);
	puts("  -d --device       device to use (default /dev/watchdog0)\r\n"
	     "  -e --enable       0 = disable, 1 = enable(default)\r\n"
	     "  -s --timeout      seconds for timeout setting, 1~4194304(default 20)\r\n"
	     "  -k --keepalive    seconds for keep alive test show, 0~4194304(default 5)\r\n");
	exit(1);
}

void parse_opts(int argc, char *argv[])
{
	int c;
	static const struct option lopts[] = {
		{"device",    1, 0, 'd'},
		{"enable",    1, 0, 'e'},
		{"timeout",   1, 0, 's'},
		{"keepalive", 1, 0, 'k'},
		{NULL, 0, 0, 0},
	};


	while (1) {
		c = getopt_long(argc, argv, "d:e:s:k:", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'd':
			device = optarg;
			break;
		case 'e':
			enable = atoi(optarg);
			if ((enable < 0) || (enable > 1)) {
				printf("enable parameter error,it must be 0 or 1\r\n");
				exit(1);
			}
			break;
		case 's':
			time_out = atoi(optarg);
		        if ((time_out <= 0) || (time_out > (0x10000000 >> 6))) {
				printf("timeout parameter error,it must be 1 to 4194304\r\n");
				exit(1);
			}
			break;
	        case 'k':
			keepalive = atoi(optarg);
			if ((keepalive < 0) || (keepalive> (0x10000000 >> 6))) {
				printf("keepalive parameter error,it must be 0 to 4194304\r\n");
				exit(1);
			}
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

