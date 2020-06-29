
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/times.h>
#include <unistd.h>
#include <termios.h>

void set_option(int fd)
{
	struct termios opts;
	tcgetattr(fd, &opts);

	opts.c_cflag |= CLOCAL|CREAD;

	opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //raw input
	opts.c_oflag &= ~OPOST; // raw output

	opts.c_cc[VTIME] = 1;
	opts.c_cc[VMIN] = 50;

	tcsetattr(fd, TCSANOW, &opts);
}

int send_data(int fd, const char *data, int datalen)
{
	int len = 0;
	len = write(fd, data, datalen);
	if(len == datalen)
	{
		return 0;
	} else {
		tcflush(fd, TCOFLUSH);
		return -1;
	}
}

int receive(int fd, uint8_t *data, int datalen)
{

	int read_len;
	if((read_len = read(fd, data, datalen))>0)
	{
		return read_len;
	} else {
		return -1;
	}
}

int main( int argc, char *argv[])
{
	int fd;
	int ret;
	uint8_t buff[1024];
	uint8_t senddata[50] = "hello virtual uart0\r";

	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd <= 0)
	{
		printf("uart open fail\n");
		return -1;
	}

	fcntl(fd, F_SETFL, 0);

	set_option(fd);


	while (1) {
		send_data(fd, senddata, strlen(senddata));

		memset(buff, 0, sizeof(buff));
		ret = receive(fd, buff, sizeof(buff));
		if(ret > 0){
			printf("RECV[%d]: ", ret);
			for(int i = 0; i< ret; i++)
				printf("0x%02x ", buff[i]);
			printf("\r\n");
		};
	}

	close(fd);
	return 0;
}
