#include "util.h"

void get_directory(char pid_arr[][6], int *pid_arr_size, const char *path){
    struct dirent *de;  // Pointer for directory entry

    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir(path); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        return; 
    } 
  
    while ((de = readdir(dr)) != NULL){
        if(de->d_name[0] >= '0' && de->d_name[0] <= '9'){
            strcpy(pid_arr[*(pid_arr_size)], de->d_name);
            (*pid_arr_size)++;
        }
    }
    closedir(dr);     
    return; 
}

void read_file_content(struct connection data[], int *num, const char *path, int af){
    char line_buffer[BUFFER_SIZE];
    FILE *fp;
    unsigned int local_addr_ip, local_addr_port;
    unsigned int remote_addr_ip, remote_addr_port;
    unsigned int inode;
    unsigned int line_id;

    unsigned int temp;

    char str_ip[30];
    int str_port;

    if((fp=fopen(path, "r")) == NULL){ // cannot open file
        return;
    }

    // Read first line of titles
    if(fgets(line_buffer, BUFFER_SIZE-1, fp) == NULL){
        fclose(fp);
        return;
    }

    // Read line by line
    while(fgets(line_buffer, BUFFER_SIZE-1, fp)){

        sscanf(line_buffer, "%d: %X:%X %X:%X %X %X:%X %X:%X %X %X %X %X", 
                &line_id, &local_addr_ip, &local_addr_port, &remote_addr_ip, &remote_addr_port, 
                &temp, &temp, &temp, &temp, &temp, &temp, &temp, &temp, &inode);
        /*
        printf("num(%d) local_address(0x%X:0x%X) remote_address(0x%X:0x%X) inode(0x%X)\n\n",
               line_id, local_addr_ip, local_addr_port, remote_addr_ip, remote_addr_port, inode);
        */      

        // Local address
        memset(str_ip, '\0', sizeof(char) * ADDRESS_SIZE);
        inet_ntop(af, (void *)&local_addr_ip, str_ip, sizeof(char) * 20);
        str_port = (int)local_addr_port;
        sprintf(data[*num].local_addr, "%s:%d", str_ip, str_port);

        // Remote address
        memset(str_ip, '\0', sizeof(char) * ADDRESS_SIZE);
        inet_ntop(af, (void *)&remote_addr_ip, str_ip, sizeof(char) * 20);
        str_port = (int)remote_addr_port;
        sprintf(data[*num].remote_addr, "%s:%d", str_ip, str_port);

        data[*num].inode_arr = inode;
        *num = *num + 1;
    }

    fclose(fp);
    return;
}

void read_program_name(const char *pid, char program_name[]){
    char path[PATH_SIZE];
    FILE *fp;

    sprintf(path, "/proc/%s/comm", pid);
    
    if((fp=fopen(path, "r")) == NULL){ // cannot open file
        return;
    }

    // Read first line of titles
    fgets(program_name, BUFFER_SIZE-1, fp);
    fclose(fp);
    return;
}

void display(struct connection data[], int size, const char *type, const char *filter){
    if(filter == NULL){
        for(int i=0; i<size; i++){
            printf("%s\t%30s%30s\t%s\\%s\n",
                type, data[i].local_addr, data[i].remote_addr,
                data[i].pid_arr, data[i].name_arr);
        }  
    }
    else{
        for(int i=0; i<size; i++){
            if(strncmp(data[i].name_arr, filter, strlen(filter)) == 0){
                printf("%s\t%30s%30s\t%s\\%s\n",
                type, data[i].local_addr, data[i].remote_addr,
                data[i].pid_arr, data[i].name_arr);
            }
        }
    }
    return;
}