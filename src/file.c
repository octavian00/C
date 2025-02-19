#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "file.h"
#include "common.h"

int create_db_file(char *fliename){
    int fd = open(fliename, O_RDONLY);

    if(fd != -1){
        close(fd);
        printf("File already exists");
        return STATUS_NOK;
    }

    fd = open(fliename, O_RDWR| O_CREAT, 0644);
    if(fd == -1){
        perror("open");
        return STATUS_NOK;
    }
    return fd;
}

int open_db_file(char *filename){
    int fd = open(filename, O_RDWR, 0644);
    if(fd == -1){
        perror("open");
        return STATUS_NOK;
    }
    return fd; 
}