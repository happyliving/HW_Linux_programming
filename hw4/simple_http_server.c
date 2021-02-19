#include "util.h"

int main(int argc, char *argv[])
{
	pid_t pid;
	int fd, pfd;
	unsigned val;
	struct sockaddr_in sin, psin;
	if(argc < 3) {
		fprintf(stderr, "usage: %s [port] [dir of root]\n", argv[0]);
		return(-1);
	}
	signal(SIGCHLD, SIG_IGN);
	if((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return(-1);
	}
	val = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
		perror("setsockopt");
		return(-1);
	}

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argv[1]));
	if(bind(fd, (struct sockaddr*) &sin, sizeof(sin)) < 0) {
		perror("bind");
		return(-1);
	}
	if(listen(fd, SOMAXCONN) < 0) {
		perror("listen");
		return(-1);
	}
	while(1) {
		val = sizeof(psin);
		bzero(&psin, sizeof(psin));
		if((pfd = accept(fd, (struct sockaddr*) &psin, &val))<0) {
			perror("accept");
			return(-1);
		}
		if((pid = fork()) < 0) {
			perror("fork");
			return(-1);
		} else if(pid == 0) {	/* child */
			close(fd);
			// serv_client(pfd, &psin);
			handle_socket(pfd, &psin, argv[2]);
			exit(0);
		}
		/* parent */
		close(pfd);
	}
}