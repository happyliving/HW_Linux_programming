#include "util.h"

struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html", "text/html" },
	{"oga", "audio/ogg"},
	{"mp4", "video/mp4"},
    {"txt", "text/plain" },
    {0,0} };

void handle_socket(int fd, struct sockaddr_in *sin, char *root)
{
    int file_fd, buflen, len;
    long i, ret;
    char * fstr;
	int has_content = 0;
    static char buffer[BUFSIZE+1];
	static char response_msg[BUFSIZE+1];
	char **file_list;
	char content[BUFSIZE+1];

	file_list = malloc( 10 * sizeof(char*));
	for (int i = 0; i < 10; i++){
		file_list[i] = malloc(sizeof(char) * 256);
	}

	// printf("connected from %s:%d\n",
	// 	inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

    ret = read(fd, buffer, BUFSIZE);   /* 讀取瀏覽器要求 */
    if (ret==0||ret==-1) {
     /* 網路連線有問題，所以結束行程 */
        exit(3);
    }

    /* 程式技巧：在讀取到的字串結尾補空字元，方便後續程式判斷結尾 */
    if (ret > 0 && ret < BUFSIZE)
        buffer[ret] = 0;
    else
        buffer[0] = 0;

    /* 移除換行字元 */
    for (i=0;i<ret;i++) 
        if (buffer[i]=='\r'||buffer[i]=='\n')
            buffer[i] = 0;
    
    /* 只接受 GET 命令要求 */
    if (strncmp(buffer,"GET ",4)&&strncmp(buffer,"get ",4))
        exit(3);
    
    /* 我們要把 GET /index.html HTTP/1.1 後面的 HTTP/1.1 用空字元隔開 */
    for(i = 4;i < BUFSIZE; i++) {
        if(buffer[i] == ' ') {
            buffer[i] = 0;
            break;
        }
    }

	/* 當客戶端要求根目錄時讀取 index.html */
    if (!strncmp(&buffer[0],"GET /\0",6)||!strncmp(&buffer[0],"get /\0",6) )
        strcpy(buffer,"GET /index.html\0");

	/* 檢查客戶端所要求的檔案格式 */
    buflen = strlen(buffer);
    fstr = (char *)0;

	if(strcmp(buffer, "GET /favicon.ico") == 0){
		return;
	}

    for(i = 0; extensions[i].ext != 0; i++) {
        len = strlen(extensions[i].ext);
        if(!strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
            fstr = extensions[i].filetype;
            break;
        }
    }


	if(strcmp(buffer, "GET /index.html\0") != 0){
		char temp[BUFSIZE];
		sprintf(temp, "GET %s/%s", root ,buffer+5);
		strcpy(buffer, temp);
	}
	
	if(is_regular_file(buffer+5) == 0){ // Is a directory
		int file_num;
		DIR * dir;
		struct dirent * ptr;
		int has_index = 0;
		dir = opendir(buffer+5);
		while((ptr = readdir(dir)) != NULL)
		{
			if(strcmp(ptr->d_name, "index.html") == 0){
				has_index = 1;
				sprintf(buffer, "GET /%s%s", buffer+5, ptr->d_name);
			}
		}
		closedir(dir);
		if(has_index == 0){
			file_num = ls(buffer+5, file_list);
			to_html(file_list, content, file_num);
			has_content = 1;
		}
		fstr = extensions[8].filetype;
	}

    /* 檔案格式不支援 */
    if(fstr == 0) {
        fstr = extensions[i-1].filetype;
    }

	if(access(&buffer[5], F_OK) == 0){ // file exist
		if(access(&buffer[5], R_OK) == 0){ // file accessible
			file_fd = open(&buffer[5], O_RDONLY);
			sprintf(response_msg,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
		}else{
			sprintf(response_msg,"HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/plain\r\n\r\n");
		}
	}else{
		sprintf(response_msg,"HTTP/1.1 403 FORBIDDEN\r\nContent-Type: text/plain\r\n\r\n");
	}

	printf("res: %s\n", response_msg);

	write(fd, response_msg, strlen(response_msg));
	if(has_content){
		write(fd, content, sizeof(content));
		has_content = 0;
	}
	
    /* 讀取檔案內容輸出到客戶端瀏覽器 */
    while ((ret=read(file_fd, buffer, BUFSIZE))>0) {
        write(fd,buffer,ret);
    }

    exit(1);
}

int is_regular_file(const char *path)
{
    int len = strlen(path);
	if(path[len-1] == '/'){
		return 0;
	}
	else{
		// struct stat path_stat;
		// stat(path, &path_stat);
		// return S_ISREG(path_stat.st_mode);
		return 1;
	}
}

int ls(char *dir, char **list)
{
	int size = 0;
	FILE *in = NULL;
	char temp[256];
	char cmd[40] = "ls -i -al ";
	char cmd_cp[40];
	strcat(cmd, dir);
	strcpy(cmd_cp, cmd);
	strcat(cmd, " | wc -l");

	in = popen(cmd, "r");
	fgets(temp, 255, in);
	size = atoi(temp) - 1;

	in=popen(cmd_cp, "r"); //for reading
	fgets(temp, 255, in); // discard total: xxx, first line
	
	for(int i = 0; i < size; i++) {
		fgets(list[i], 255, in);   
	}

	for(int i = 0; i < size; i++) {
		if(i == size-1){
			break;
		}
		strcpy(list[i], list[i+1]);
	}

	return size;
}

void to_html(char **list, char *content, int size)
{
	memset(content, 0, BUFSIZE+1);
	sprintf(content, "<html> \
						<head> \
							<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"> \
							<style> \
							body {	\
								font-family: monospace;	\
								white-space: pre; \
							} \
							</style> \
						</head><body><hr>");
	for(int i=0; i<size; i++){
		strcat(content, list[i]);
		strcat(content, "\r\n\r\n");
	}
	strcat(content, "<hr></body></html>\r\n");
}