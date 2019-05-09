#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include <inttypes.h>
#include <asr_frame.h>

int write_n(int sockfd, char *buff, int size)
{
	ssize_t n = 0;
	while (size > 0) {
		if ((n = write(sockfd, buff, size)) <= 0) {
			return n;
		}
		size -= n;  // 用于循环发送且退出功能
		buff += n;  // 用于计算已发buffer的偏移量
	}
	return 1;
}

int read_n(int sockfd, char *buff, int size)
{
	ssize_t n = 0;
	while (size > 0) {  // 剩余部分大于0
		if ((n = read(sockfd, buff, size)) <= 0) {
			printf("read: %d\n", n);
			return n;
		}
		size -= n;
		buff += n;
	}
	return 1;
}

static void Usage(const char* proc)
{
	printf("Usage: %s <port>\n",proc);
}


// int StartUp(const char* ip, int port)
int StartUp(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);  // create socket
	if (sock < 0) {
		perror("socket");
		exit(2);
	}

	struct sockaddr_in local;  // IPV4
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr(ip);

	if (bind(sock, (struct sockaddr *)&local,
				sizeof (local)) < 0) {  // bind
		perror("bind");
		exit(3);
	}

	// "echo 1024 > /proc/sys/net/core/somaxconn"
	if (listen(sock, SOMAXCONN) < 0) {
		perror("listen");
		exit(4);
	}
	return sock;  // return an "listen_sock";
}

void* request(void* arg)
{
	int new_sock = (intptr_t)arg;


	int flags  = fcntl(new_sock, F_GETFL, 0);
	// fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);  // 设置成非堵塞
	fcntl(new_sock, F_SETFL, flags&~O_NONBLOCK);  // 设置成堵塞

	printf("get a new client\n");
	while (1) {  // first read,then write
		char buf[1024];

		/* ssize_t s = read(new_sock, buf, sizeof (buf)); */
		// MSG_DONTWAIT | MSG_WAITALL; 要和fcntl(NONBLOCK)一致
		ssize_t s = recv(new_sock, buf, 8, MSG_WAITALL);

		/* @TODO asr
		 *   1. 解析收到的数据
		 *   2. 调用极限元asr
		 *   3. 将结果发送到客户端
		 */

		// 在一般情况下, recv(MSG_WAITALL)读完指定的长度才返回\
		// 在异常情况下有可能提前返回
		if (s > 0) {  // read successful
			buf[s] = '\0';
			printf("client#:%s\n", buf);
			// echo
			write_n(new_sock, buf, strlen(buf));
		} else if (s == 0) {  // read to file's end
			printf("client exit\n");
			break;
		} else {  // read fail
			perror("read");
			break;
		}
	}
	close(new_sock);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		Usage(argv[0]);
		return 1;
	}

	// listen_sock
	int listen_sock = StartUp(atoi(argv[1]));
	while (1) {
		struct sockaddr_in client;  // get sender's information
		socklen_t len = sizeof(client);

		// get real socket that we will operator
		int new_sock = accept(listen_sock,
				(struct sockaddr*)&client,
				&len);
		if (new_sock < 0) {
			perror("accept");
			continue;
		}

		pthread_t id;
		pthread_create( &id, NULL,
				&request,
				(void *)(intptr_t)new_sock);
		pthread_detach(id);
	}
	return 0;
}
