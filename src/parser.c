#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "parser.h"
#include "common.h"

void output_file(int fd, struct  dbheader_t *dbhdr, struct employee_t *employees)
{
    if(fd < 0)
    {
        printf("Got a bad fd from the user");
        return;
    
    }
    int realcount = dbhdr->count;
    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);
    lseek(fd, 0, SEEK_SET);
    write(fd,dbhdr,sizeof(struct dbheader_t));
    int i=0;
    for(;i<realcount;i++)
    {
        printf("output file");
        employees[i].hours = htonl(employees[i].hours);
        write(fd,&employees[i], sizeof(struct employee_t));
    }
    return;
}

int create_db_header(int fd, struct dbheader_t **headerOut)
{
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if(header == -1)
    {
        printf("Malloc failed to create db_header \n");
        return STATUS_NOK;
    }
    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);
    *headerOut = header;
    printf("DB HEADER CREATED succesfully");
    
    return STATUS_OK;
}

int validate_db_header(int fd, struct dbheader_t **headerOut)
{
    if(fd < 0)
    {
        printf("Got a bad fd from the user");
        return STATUS_NOK;
    }
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if(header == -1)
    {
        printf("Malloc failed to create db_header \n");
        return STATUS_NOK;
    }
    if(read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t))
    {
        fprintf(stderr, "Short read: Expected %zu, got %zd\n", sizeof(struct dbheader_t), read(fd, header, sizeof(struct dbheader_t)));
        printf("error");
        perror("read");
        free(header);
        return STATUS_NOK;
    }
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);
    if(header->magic != HEADER_MAGIC)
    {
        printf("Improper header MAGIC\n");
        free(header);
        return STATUS_NOK; 
    }
    if(header->version != 1)
    {
        printf("Improper header version\n");
        free(header);
        return STATUS_NOK; 
    }
    struct stat dbstat = {0};
    fstat(fd,&dbstat);
    if(header->filesize != dbstat.st_size){
        printf("Corrputed database\n");
        free(header);
        return STATUS_NOK;
    }
    *headerOut = header;
    return STATUS_OK;
}

int reade_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employesOut)
{
    if(fd < 0)
    {
        printf("Got a bad fd from the user");
        return STATUS_NOK;
    }

    int count = dbhdr->count;
    struct employee_t *employees = calloc(count, sizeof(struct employee_t ));

    if(employees == -1)
    {
        printf("Malloc failed\n");
        return STATUS_NOK;
    }
    read(fd,employees,count * sizeof(struct employee_t));
    int i=0;
    for(;i<count;){
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employesOut = employees;
    return STATUS_OK;
}
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addString)
{
    printf("AddEmployee =%s\n",addString);
    char *name = strtok(addString,",");
    char *addr = strtok(NULL, ",");
    char *hours = strtok(NULL, ",");

    strncpy(employees[dbhdr->count -1].name, name, sizeof(employees[dbhdr->count -1].name));
    strncpy(employees[dbhdr->count -1].address, addr, sizeof(employees[dbhdr->count -1].address));
    employees[dbhdr->count -1].hours = atoi(hours);
    printf("%s %s %s",name,addr,hours);
    return STATUS_OK;
}

void list_employees( struct  dbheader_t *dbhdr, struct employee_t *employees)
{
    int i=0;
    for(;i<dbhdr->count;)
    {
        printf("Employee %d\n", i);
        printf("\t Name %s\n", employees[i].name);
        printf("\t Address %s\n", employees[i].address);
        printf("\t Hours %d\n", employees[i].hours);
    }
}