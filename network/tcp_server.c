#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <linux/if.h>

#define SERVPORT 3333 /*port */
#define BACKLOG 10 /* max client */

int main()
{
	int sockfd,client_fd; 
	struct sockaddr_in my_addr; /* loacl */
	struct sockaddr_in remote_addr; 
	int sin_size;
	struct ifreq interface;
	socklen_t ifreq_len = sizeof(interface);
	int index;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket fail！"); exit(1);
	}
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(SERVPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero),8); 

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind error！");
		exit(1);
	}

#if 0
	strncpy(interface.ifr_name, "eth0", IFNAMSIZ);
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,(char *)&interface, sizeof(interface)) < 0) {
		close(sockfd);
		printf("setsockopt SO_BINDTODEVICE error!\n");
		return -1;
	}

	errno = 0;

	if ((index = getsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, &ifreq_len)) < 0) {
		close(sockfd);
		perror("getsockopt SO_BINDTODEVICE error!\n");
		printf("interface index=%d\n", index);
		return -1;
	}
	printf("interface index=%d\n", index);
#endif

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen error！");
		exit(1);
	}
	while(1) {
		sin_size = sizeof(struct sockaddr_in);
		if ((client_fd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size)) == -1) {
			perror("accept error");
			continue;
		}
		printf("REC FROM： %s\n",inet_ntoa(remote_addr.sin_addr));

		if (fork() == 0) {
			if (send(client_fd, "HelloWorld!\n", 26, 0) == -1)
				perror("send error！");
			close(client_fd);
			exit(0);
		}else
			close(client_fd);
	}
}
