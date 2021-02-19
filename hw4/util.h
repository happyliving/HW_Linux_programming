#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#define BUFSIZE 8096
void handle_socket(int fd, struct sockaddr_in *sin, char *root);
int is_regular_file(const char *path);
int ls(char *dir,  char **list);
void to_html(char **list, char *content, int size);