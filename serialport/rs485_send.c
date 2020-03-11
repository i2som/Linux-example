#include <stdio.h>
#include <termios.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#include <asm-generic/ioctls.h> /* TIOCGRS485 + TIOCSRS485 ioctl definitions */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "serial.h"

/**
 * @brief: open serial port
 * @Param: dir: serial device path
 */
int open_port(char *dir)
{
	int fd ;
	fd = open(dir, O_RDWR);
	if(fd < 0) {
		perror("open_port");
	}
	return fd;
}

/**
 * @brief: print usage message
 * @Param: stream: output device
 * @Param: exit_code: error code which want to exit
 */
void print_usage (FILE *stream, int exit_code)
{
	fprintf(stream, "Usage: option [ dev... ] \n");
	fprintf(stream,
		"\t-h  --help     Display this usage information.\n"
		"\t-d  --device   The device ttyS[0-3] or ttySCMA[0-1]\n"
		"\t-b  --baudrate Set the baud rate you can select\n" 
		"\t               [230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300]\n"
		"\t-s  --string   Write the device data\n"
		"\t-e  --1 or 0   Write 1 to enable the rs485_mode(only at atmel)\n");
	exit(exit_code);
}

int main(int argc, char *argv[])
{
	char write_buf[20] = {0x0};
	char read_buf[100];
	int fd, i, len, nread,r, count = 0;
	pid_t pid;
	int next_option;
	struct termios oldtio;
	int speed ;
	char *device;
	int spee_flag = 0, device_flag = 0;
	const char *const short_options = "hd:s:b:e:c:";

	const struct option long_options[] = {
		{ "help",	0, NULL,	'h'},
		{ "device",	1, NULL,	'd'},
		{ "string",	1, NULL,	's'},
		{ "baudrate",	1, NULL,	'b'},
		{ "count",	1, NULL,	'c'},
		{ NULL,		0, NULL,	0}
	};

	if (argc < 2) {
		print_usage (stdout, 0);
		exit(0);
	}

	while (1) {
		next_option = getopt_long (argc, argv, short_options, long_options, NULL);
		if (next_option < 0)
			break;
		switch (next_option) {
			case 'h':
				print_usage (stdout, 0);
				break;
			case 'd':
				device = optarg;
				device_flag = 1;
				break;
			case 'b':
				speed = atoi(optarg);
				spee_flag = 1;
				break;
			case 'e':
				r = atoi(optarg);
				break;
			case 'c':
				count = atoi(optarg);
				break;
			case '?':
				print_usage (stderr, 1);
				break;
			default:
				abort ();
		}
	}

	if ((!device_flag)||(!spee_flag)) {
		print_usage (stderr, 1);
		exit(0);
	}

	/* open serial port */
	fd = open_port(device);
	if (fd < 0) {
		perror("open failed");
		return -1;
	}
	if(r)
	{
		rs485_enable(fd, RS485_ENABLE);
	}
	/* set serial port */
	i = set_port(fd, speed, 8, 'N', 1);
	if (i < 0) {
		perror("set_port failed");
		return -1;
	}
	for(i = 0; i < 20; i++)
		write_buf[i] = i;

	while (1) {
		/* print data to serial port */
		printf("SEND[%03d]: ", sizeof(write_buf));
		for(i = 0; i < sizeof(write_buf);i++)
		    printf("0x%02x ", write_buf[i]);
		printf("\n");
		write(fd, write_buf, sizeof(write_buf));
		sleep(1);
		if(count > 1)
			count -= 1;
		else
			break;
	}

	/* restore the old configuration */
	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd);

	return 0;
}
