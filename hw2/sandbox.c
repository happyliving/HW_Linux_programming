#include "sandbox.h"

int main(int argc, char *argv[], char *envp[]){
    
    int cmd_opt;
    char path[MAX_PATH_LENGTH] = "./sandbox.so";
    char directory[MAX_PATH_LENGTH] = "BASE=.";

    while((cmd_opt = getopt(argc, argv, "p:d:")) != -1){
        switch (cmd_opt) {
            case 'p':
                memcpy(path, optarg, sizeof(optarg));
                break;
            case 'd':
                memset(directory, 0, sizeof(path));
                memcpy(directory, "BASE=", sizeof(optarg));
                strcat(directory, optarg);
                //memcpy(directory, optarg, sizeof(optarg));
                break;
            case '?':
                printf("usage: ./hw2 [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n\t-p: set the path to sandbox.so, default = ./sandbox.so\n\t-d: restrict directory, default = .\n\t--: seperate the arguments for sandbox and for the executed command\n");
                return 0;
            default:
                fprintf(stderr, "no command given.\n");
                return 0;
        }
    }

    setenv("LD_PRELOAD", path, 1);

    if(argc == 1){
        fprintf(stderr, "no command given.\n");
        return 0;
    }

    //printf("path:%s\ndir:%s\n", path, directory);
    char **command = (char**) malloc(sizeof(char*) * (argc - optind + 1));
 
    for(int i = optind; i < argc; i++){      
        command[i-optind] = (char *)malloc(sizeof(char) * 20);
        strcpy(command[i-optind], argv[i]);
    }
    command[argc - optind] = NULL;

    putenv(directory);
    execvp(command[0], command);

    unsetenv("BASE");
    return 0;
}
