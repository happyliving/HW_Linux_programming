/*
 * 
 * Minimum Requirement:
 * 
 * chdir chmod chown creat fopen
 * link mkdir open openat opendir 
 * readlink remove rename rmdir 
 * stat symlink unlink
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <string.h>



#define log(format, ...) \
        fprintf(stdout, "[sandbox] " format , ##__VA_ARGS__);
#define bind_origin(name) \
        origin_##name = dlsym(handle, #name);

#define MAXBUFSIZE 1024

static int (*origin_chdir)(const char *path) = NULL;
static int (*origin_chmod)(const char *pathname, mode_t mode) = NULL;
static int (*origin_chown)(const char *pathname, uid_t owner, gid_t group) = NULL;
static int (*origin_creat)(const char *pathname, mode_t mode) = NULL;
static FILE* (*origin_fopen)(const char *pathname, const char *mode) = NULL;
static int (*origin_link)(const char *oldpath, const char *newpath) = NULL;
static int (*origin_mkdir)(const char *pathname, mode_t mode) = NULL;
static int (*origin_open)(const char *pathname, int flags, ...) = NULL;
static int (*origin_openat)(int dirfd, const char *pathname, int flags, ...) = NULL;
static DIR* (*origin_opendir)(const char *name) = NULL;
static ssize_t (*origin_readlink)(const char *pathname, char *buf, size_t bufsiz) = NULL;
static int (*origin_remove)(const char *pathname) = NULL;
static int (*origin_rename)(const char *oldpath, const char *newpath) = NULL;
static int (*origin_rmdir)(const char *pathname) = NULL;
static int (*origin_stat)(const char *pathname, struct stat *statbuf) = NULL;
static int (*origin_symlink)(const char *target, const char *linkpath) = NULL;
static int (*origin_unlink)(const char *pathname) = NULL;

static __attribute__((constructor)) void init_before_main()
{
    // Bind to origin library
    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    if(handle != NULL)
    {
        bind_origin(chdir);
        bind_origin(chmod);
        bind_origin(chown);
        bind_origin(creat);
        bind_origin(fopen);
        bind_origin(link);
        bind_origin(mkdir);
        bind_origin(open);
        bind_origin(openat);
        bind_origin(opendir);
        bind_origin(readlink);
        bind_origin(remove);
        bind_origin(rename);
        bind_origin(rmdir);
        bind_origin(stat);
        bind_origin(symlink);
        bind_origin(unlink);
    }
}

static __attribute__((destructor)) void end_after_main();

char *check_path(const char *path){
    char *base;
    char real_path[MAXBUFSIZE];
    char real_base[MAXBUFSIZE];

    base = getenv("BASE");
    realpath(path, real_path);
    realpath(base, real_base);
    //printf("real_path: %s\n", real_path);
    //printf("real_base: %s\n", real_base);
    char *ptr_position = strstr(real_path, real_base);
    return ptr_position;
}

//--------------------API injection------------------------//


int chdir(const char *path)
{
    char *ptr_position;
    ptr_position = check_path(path); 
    if(ptr_position == NULL){
        printf("[sandbox] chdir: access to %s is not allowed\n", path);
        return -1;
    }
    return origin_chdir(path);
}

int chmod(const char *pathname, mode_t mode){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] chmod: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_chmod(pathname, mode);
}

int chown(const char *pathname, uid_t owner, gid_t group){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] chown: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_chown(pathname, owner, group);
}

int creat(const char *pathname, mode_t mode){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] creat: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_creat(pathname, mode);
}

FILE *fopen(const char *pathname, const char *mode){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] fopen: access to %s is not allowed\n", pathname);
        return NULL;
    }
    return origin_fopen(pathname, mode);
}

int link(const char *oldpath, const char *newpath){
    char *old_path_ptr_position;
    char *new_path_ptr_position;
    old_path_ptr_position = check_path(oldpath); 
    new_path_ptr_position = check_path(newpath);
    if(old_path_ptr_position == NULL){
        printf("[sandbox] link: access to %s is not allowed\n", old_path_ptr_position);
        return -1;
    }
    if(new_path_ptr_position == NULL){
        printf("[sandbox] link: access to %s is not allowed\n", new_path_ptr_position);
        return -1;
    }
    return origin_link(oldpath, newpath);
}

int mkdir(const char *pathname, mode_t mode){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] mkdir: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_mkdir(pathname, mode);
}
int open(const char *pathname, int flags, ...){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] open: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_open(pathname, flags);
}
int openat(int dirfd, const char *pathname, int flags, ...){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] open: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_openat(dirfd, pathname, flags);
}

DIR* opendir(const char *name){
    char *ptr_position;
    ptr_position = check_path(name); 
    if(ptr_position == NULL){
        printf("[sandbox] opendir: access to %s is not allowed\n", name);
        return NULL;
    }
    return origin_opendir(name);
}
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] readlink: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_readlink(pathname, buf, bufsiz);
}

int remove(const char *pathname){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] remove: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_remove(pathname);
}
int rename(const char *oldpath, const char *newpath){
    char *old_path_ptr_position;
    char *new_path_ptr_position;
    old_path_ptr_position = check_path(oldpath); 
    new_path_ptr_position = check_path(newpath);
    if(old_path_ptr_position == NULL){
        printf("[sandbox] rename: access to %s is not allowed\n", old_path_ptr_position);
        return -1;
    }
    if(new_path_ptr_position == NULL){
        printf("[sandbox] rename: access to %s is not allowed\n", new_path_ptr_position);
        return -1;
    }
    return origin_rename(oldpath, newpath);
}
int rmdir(const char *pathname){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] rmdir: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_rmdir(pathname);
}
int stat(const char *pathname, struct stat *statbuf){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] stat: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_stat(pathname, statbuf);
}
int symlink(const char *target, const char *linkpath){
    char *target_ptr_position;
    char *link_ptr_position;
    target_ptr_position = check_path(target); 
    link_ptr_position = check_path(linkpath);
    if(target_ptr_position == NULL){
        printf("[sandbox] symlink: access to %s is not allowed\n", target);
        return -1;
    }
    if(link_ptr_position == NULL){
        printf("[sandbox] symlink: access to %s is not allowed\n", linkpath);
        return -1;
    }
    return origin_rename(target, linkpath);
}
int unlink(const char *pathname){
    char *ptr_position;
    ptr_position = check_path(pathname); 
    if(ptr_position == NULL){
        printf("[sandbox] stat: access to %s is not allowed\n", pathname);
        return -1;
    }
    return origin_unlink(pathname);
}

















    