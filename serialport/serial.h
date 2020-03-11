#ifndef __SERIAL__
#define __serial__

typedef enum {
	RS485_DISABLE = 0,
	RS485_ENABLE = 1
} RS485_ENABLE_t;

int set_port(int fd, int nSpeed, int nBits, char nEvent, int nStop);
int rs485_enable(const int fd, const RS485_ENABLE_t enable);
#endif
