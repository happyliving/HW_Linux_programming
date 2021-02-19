# include "util.h"

int main(int argc, char *argv[]){
    int pid_arr_size = 0;
    char pid_arr[MAX_PROCESS_NUM][PROCESS_ID_SIZE];

    int tcp_size = 0;
    struct connection tcp[MAX_TCP_UDP_NUM];

    int tcp6_size = 0;
    struct connection tcp6[MAX_TCP_UDP_NUM];

    int udp_size = 0;
    struct connection udp[MAX_TCP_UDP_NUM];

    int udp6_size = 0;
    struct connection udp6[MAX_TCP_UDP_NUM];

    read_file_content(tcp, &tcp_size, "/proc/net/tcp", AF_INET);
    read_file_content(tcp6, &tcp6_size, "/proc/net/tcp6", AF_INET6);
    read_file_content(udp, &udp_size, "/proc/net/udp", AF_INET);
    read_file_content(udp6, &udp6_size, "/proc/net/udp6", AF_INET6);

    get_directory(pid_arr, &pid_arr_size, "/proc/");  
    for(int i = 0; i < pid_arr_size; i++){
        char proc_fd_path[PATH_SIZE];
        char temp_path[PATH_SIZE];
        char fd_des_buf[255];
        ssize_t fd_des_len;
        int dir_arr_size = 0;
        char dir_arr[MAX_FILE_IN_PID][6];

        sprintf(proc_fd_path, "/proc/%s/fd/", pid_arr[i]);
        get_directory(dir_arr, &dir_arr_size, proc_fd_path);

        for(int j = 0; j < dir_arr_size; j++){ 
            sprintf(temp_path, "/proc/%s/fd/%s", pid_arr[i], dir_arr[j]);
            if ((fd_des_len = readlink(temp_path, fd_des_buf, sizeof(fd_des_buf)-1)) != -1)
                fd_des_buf[fd_des_len] = '\0';
            
            if (strncmp("socket", fd_des_buf, 6)==0){
                // Extract inode_number in socket
                unsigned int inode_num = 0;
                for(int k = 0; k < strlen(fd_des_buf); k++)
                {
                    if((fd_des_buf[k] >= '0' && fd_des_buf[k] <= '9'))
                        inode_num = inode_num * 16 + fd_des_buf[k] - '0';
                }

                // tcp
                for(int k = 0; k < tcp_size; k++){
                    if(inode_num == tcp[k].inode_arr){
                        strcpy(tcp[k].pid_arr, pid_arr[i]);
                        read_program_name(pid_arr[i], tcp[k].name_arr);
                    }
                }

                // tcp6
                for(int k = 0; k < tcp6_size; k++){
                    if(inode_num == tcp6[k].inode_arr){
                        strcpy(tcp6[k].pid_arr, pid_arr[i]);
                        read_program_name(pid_arr[i], tcp6[k].name_arr);
                    }
                }

                // udp
                for(int k = 0; k < udp_size; k++){
                    if(inode_num == udp[k].inode_arr){
                        strcpy(udp[k].pid_arr, pid_arr[i]);
                        read_program_name(pid_arr[i], udp[k].name_arr);
                    }
                }

                // udp6
                for(int k = 0; k < udp6_size; k++){
                    if(inode_num == udp6[k].inode_arr){
                        strcpy(udp6[k].pid_arr, pid_arr[i]);
                        read_program_name(pid_arr[i], udp6[k].name_arr);
                    }
                }
            }
            
        }
        
    }

    int c; // default if no argument
    const char *optstring = "t::u::";
    struct option opts[] = {
        {"tcp", 2, NULL, 't'},
        {"udp", 2, NULL, 'u'},
        {NULL, 0, NULL, 0}
    };
    while((c = getopt_long_only(argc, argv, optstring, opts, NULL)) != -1){
        switch(c){
            case 't':
                printf("List of TCP connections:\n");
                printf("%s\t%30s%30s\tPID\\Program name and arguments\n", "Proto", "Local Address", "Foreign Address");
                if(argc > 2){
                    const char* filter = argv[2];
                    printf("%s\n", argv[2]);
                    display(tcp, tcp_size, "tcp", filter);
                    display(tcp6, tcp6_size, "tcp6", filter);
                }else{
                    display(tcp, tcp_size, "tcp", NULL);
                    display(tcp6, tcp6_size, "tcp6", NULL);
                }           
                break;
            case 'u':
                printf("\nList of UDP connections:\n");
                printf("%s\t%30s%30s\tPID\\Program name and arguments\n", "Proto", "Local Address", "Foreign Address");
                if(argc > 2){
                    const char* filter = argv[2];
                    display(udp, udp_size, "udp", filter);
                    display(udp6, udp6_size, "udp6", filter);
                }else{
                    display(udp, udp_size, "udp", NULL);
                    display(udp6, udp6_size, "udp6", NULL);
                }  
                break;
            default:
                printf("Not accepted\n");
                break;
        }
    }

    if(argc == 1){
        printf("List of TCP connections:\n");
        printf("%s\t%30s%30s\tPID\\Program name and arguments\n", "Proto", "Local Address", "Foreign Address");
        display(tcp, tcp_size, "tcp", NULL);
        display(tcp6, tcp6_size, "tcp6", NULL);

        printf("\nList of UDP connections:\n");
        printf("%s\t%30s%30s\tPID\\Program name and arguments\n", "Proto", "Local Address", "Foreign Address");
        display(udp, udp_size, "udp", NULL);
        display(udp6, udp6_size, "udp6", NULL);
    }
    return 0;
}   