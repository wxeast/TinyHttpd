#ifndef _HTTPD_
#define _HTTPD_ 
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<assert.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/epoll.h>
#define SUCCESS 0
#define NOTICE 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

#define SIZE 1024


int   startup(const  char * ip,int  port);
void  printf_log(char * argc,int  level);
void* http_handler(void *  arg);
int   get_line(int  sock,char buf[],int size);
static int   echo_www(int sock,const char * path,int size);
static int   exe_cgi(int sock,const  char  * mothed,const  char * path,char * query_string);
static void  drop_hander(int sock);
void  echo_string(int sock);
#endif
