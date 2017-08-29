#include"httpd.h"

static  void Usage(const  char  * proc)
{
	printf("Usage :%s [ip] [port]\n",proc);
}


int main(int argc,char *  argv[])
{
	if(argc != 3)
	{
		Usage(argv[0]);
		return 1;
	}
	//得到监听套接字
	int listen_sock = startup(argv[1],atoi(argv[2]));
    //建立epoll模型
    int epollfd  =epoll_create(256);
    //对epoll模型进行管理
    struct  epoll_event event;
    event.events = EPOLLIN;
    event.data.fd =  listen_sock;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listen_sock,&event);
	int maxsize =SIZE;
	int timeout = -1;
	while(1)
	{
		int  n =0 ;
		struct epoll_event ev[maxsize];
		switch(n = epoll_wait(epollfd,ev,maxsize,timeout))
		{
			case 0:
				break;
			case -1:
				printf_log(strerror(errno),FATAL);
				break;
			default:
			{
				int i = 0;
				for(;i<n;++i)
				{
					int fd = ev[i].data.fd;
					if(fd == listen_sock&&ev[i].events&EPOLLIN)
					{					
						struct sockaddr_in client ;
						socklen_t len = sizeof(client);
						int sock = accept(listen_sock,\
						(struct sockaddr*)&client, &len); 
						if(sock < 0 )
						{
							printf_log(strerror(errno),NOTICE);
							continue;
						}
						printf("get client....ip :%s,port :%d \n",\
						inet_ntoa(client.sin_addr),ntohs(client.sin_port));
						pthread_t pt;
						//要是创建进程失败
						int ret = pthread_create(&pt,NULL,http_handler,\
						(void*)sock);
						if(ret!=0)
						{
							printf_log(strerror(errno),WARNING);
							close(sock);
							continue;
						}
						//进程创建成功
						//设置进程状态为分离状态
						pthread_detach(pt);
					}
					else 
					{}
				}
			}
			break;
		}
	}
	close(listen_sock);
	return  0;
}
