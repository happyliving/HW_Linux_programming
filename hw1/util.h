#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <getopt.h>

#define MAX_TCP_UDP_NUM 100
#define MAX_PROCESS_NUM 1000
#define MAX_FILE_IN_PID 100
#define PATH_SIZE 50
#define PROCESS_ID_SIZE 6
#define ADDRESS_SIZE 30
#define BUFFER_SIZE 500

struct connection{
    char local_addr[ADDRESS_SIZE];
    char remote_addr[ADDRESS_SIZE];
    unsigned int inode_arr;
    char pid_arr[ADDRESS_SIZE];
    char name_arr[ADDRESS_SIZE];
};

void get_directory(char pid_arr[][6], int *pid_arr_size, const char *path);
void read_file_content(struct connection data[], int *num, const char *path, int af);
void read_program_name(const char *pid, char program_name[]);
void display(struct connection data[], int size, const char *type, const char *filter);