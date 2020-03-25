#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <getopt.h>

char *program_name;

void print_usage (FILE *stream, int exit_code)
{
	fprintf(stream, "Usage: %s [ dev... ] \n", program_name);
	fprintf(stream,
		"\t-h  --help		Display this usage information.\n"
		"\t-d  --device		The device can[0-1]\n"
		"\t-i  --id		Set the can id that want to receive\n");
	exit(exit_code);
}

int hex2dec(const char * phex)
{
	int dwhexnum=0;

	if ((phex[0] == '0') && (phex[1] == 'x' || phex[1] == 'X')) {
		phex = phex + 2;
	}

	for (; *phex!=0 ; phex++) {
		dwhexnum *= 16;
		if ((*phex>='0') && (*phex<='9'))
			dwhexnum += *phex-'0';
		else if ((*phex>='a') && (*phex<='f'))
			dwhexnum += *phex-'a'+10;
		else if ((*phex>='A') && (*phex<='F'))
			dwhexnum += *phex-'A'+10;
		else {
			printf("hex format error!\n");
			exit(0);
		}
	}

	return dwhexnum;
}

int main(int argc, char* argv[])
{
	int s, nbytes, i, ret;
	char *device="can0";
	int id, next_option, device_flag=0, id_flag=0;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
	const char *const short_options = "hd:i:";
	const struct option long_options[] = {
		{ "help",   0, NULL, 'h'},
		{ "device", 1, NULL, 'd'},
		{ "id", 1, NULL, 'i'},
		{ NULL,     0, NULL, 0  }
	};

	program_name = argv[0];
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
			case 'i':
				id = hex2dec(optarg);
				id_flag = 1;
				break;
			default:
				abort ();
		}
	}

	if (!device_flag || !id_flag) {
		print_usage (stdout, 0);
		exit(0);
	}

	/* create a socket */
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if(s < 0) {
		perror("socket PF_CAN failed\n");
		return -1;
	}

	strcpy(ifr.ifr_name, device);
	/* determine the interface index */
	ret = ioctl(s, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		perror("ioctl failed\n");
		return -1;
	}

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	/* bind the socket to a CAN interface */
	ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0) {
		perror("bind failed\n");
		return -1;
	}

	if (id_flag) {
		/* define the filter rules */
		rfilter[0].can_id   = id;
		rfilter[0].can_mask = CAN_SFF_MASK;
		/* Set the filter rules */
		setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	}

	while(1) {
		/* receive frame */
		nbytes = read(s, &frame, sizeof(frame));
		/* printf the received frame */
		if (nbytes > 0) {
			printf("%s  %#x  [%d]  ", ifr.ifr_name, frame.can_id, frame.can_dlc);
			for (i = 0; i < frame.can_dlc; i++)
				printf("%#x ", frame.data[i]);
			printf("\n");
		}
	}

	close(s);

	return 0;
}
