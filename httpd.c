#include"httpd.h"

int startup(const  char * ip,int  port)
{
	int  sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock< 0)
	{
		printf_log(strerror(errno),FATAL);
		exit(2);
	}
	//设置端口与ip复用
	int opt = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	//绑定本地ip与端口
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);
	if(bind(sock,(struct sockaddr*)&local,sizeof(local))< 0)
	{
		printf_log(strerror(errno),FATAL);
		exit(3);
	}
	//设置监听状态
	if(listen(sock,10)<0)
	{
		printf_log(strerror(errno),FATAL);
		exit(4);
	}
	return sock;
}


void printf_log(char * argc,int  level)
{
#ifdef _STDOUT_
	const  char * const Levels[] = {
						"SUCCESS",
						"NOYICE",
						"WARNING",
						"ERROR",
						"FATAL"	};

	printf("%s [%s]\n",argc,Levels[level%5]);
#endif 
}
void echo_string(int sock)
{}

//读取一行   返回  0 -》“\0”  1 -> "\n"
//对于不同的平台的换行  三种情况 \n  、 \r 、\r\n 
int  get_line(int  sock,char buf[],int size)
{
	int len = 0;
	char  ch = '\0';
	while(ch!= '\n'&& len < size-1) 
	{
		if(recv(sock,&ch,1,0)>0) 
		{
			if(ch == '\r')
			{
				recv(sock,&ch,1,MSG_PEEK);
				if(ch == '\n')
				{
					recv(sock,&ch,1,0);
				}
				ch = '\n';
			}
		}
		buf[len++] =ch;
	}
	buf[len]=0;
	return  len;
}

//清空头部
static void drop_header(int sock)
{
	char  line[SIZE];
	int ret = -1;
	do
	{
		ret = get_line(sock,line,sizeof(line));
	}while(ret > 0&&strcmp(line,"\n"));
}
//实现cgi模式下的 响应
static int exe_cgi(int sock,const  char  * method,const  char * path,char * query_string)
{
	int content_length= -1;//定义post方法的参数长度
	//要是为GET方法
	//printf("%s\n",mothed);
	if(strcasecmp(method,"GET") == 0)
	{
		assert(query_string);
		drop_header(sock);//已经得到参数了 ，直接清除头部
	}else{//post方法
		//得到参数的长度
		char line[SIZE];
		int ret = -1;
		do
		{
			ret = get_line(sock,line,sizeof(line));
			if(ret>0 && strncasecmp(line,"Content-Length: ",strlen("content-length: "))==0)
			{
				content_length = atoi(&line[strlen("content-length: ")]);
			}
		}while(ret>0&&strcmp(line,"\n"));//知道读到空行
		if(content_length == -1)
		{
			echo_string(sock);
			return 11;
		}
	}
	//先将响应首部 发送
	const  char  * msg ="HTTP/1.0 200 OK\r\n";
	send(sock,msg,strlen(msg),0);
	const char *type="Content-Type:text/html;charset=ISO-8859-1\r\n";
	send(sock, type, strlen(type), 0);
	const  char * null_string = "\r\n";
	send(sock,null_string,strlen(null_string),0);
	
	//执行 服务器下的可执行文件
	int input[2];//表示的是 子进程的  读 
	int output[2];//表示子线程的写 
	if(pipe(input)||pipe(output))
	{
		//创建管道失败
		printf_log(strerror(errno),FATAL);
		return 12;
	}
	//创建进程
	pid_t id = fork();
	if(id < 0)
	{
		printf_log(strerror(errno),FATAL);
		return 13;
	}else if(id == 0)//child
	{
		close(input[1]);//关掉写
		close(output[0]);//管道读
		char method_env[SIZE/10];
		char query_string_env[SIZE];
		char content_length_env[SIZE/10];
		sprintf(method_env,"METHOD=%s",method);
		putenv(method_env);
		if(strcasecmp(method,"GET") == 0)
		{
			sprintf(query_string_env,"QUERYSTRING=%s",query_string);
			putenv(query_string_env);
		}else{
			sprintf(content_length_env,"CONTENTLENGTH=%d",content_length);
			putenv(content_length_env);
		}
		dup2(input[0],0);
		dup2(output[1],1);
		execl(path,path,NULL);
		printf("execl error\n");
		exit(1);
	}else//parent
	{
		close(input[0]);//关掉读
		close(output[1]);//管道写
		int  i= 0;
		char  ch = '\0';
		for(;i< content_length;++i)
		{
			int s  = recv(sock,&ch,1,0);//读取参数
			if(s > 0)
			{
				write(input[1],&ch,1);
			}
			else
			{
				echo_string(sock);
				close(input[1]);
				close(output[0]);
				return 14; 
			}
		}
		ch = '\0';
		//while(recv(output[0],&ch,1,0) > 0)
		while(read(output[0],&ch,1) > 0)
		{
			send(sock,&ch,1,0);
		}
		int ret=  waitpid(id,NULL,0);
		if(ret != id)
		{
			printf_log(strerror(errno),FATAL);
			return 14;
		}
		close(input[1]);
		close(output[0]);
	}
	return  0;
}
//实现正常模式下的 响应
static int echo_www(int sock,const char * path,int size)
{
	int fd  = open(path,O_RDONLY);
	if(fd <  0)
	{
		printf_log(strerror(errno),FATAL);
		return  9;
	}
	const  char  * msg ="HTTP/1.0 200 OK\r\n";
	send(sock,msg,strlen(msg),0);
	const  char * null_string = "\r\n";
	send(sock,null_string,strlen(null_string),0);
	if(sendfile(sock,fd,NULL,size)< 0 )
	{
		echo_string(sock);
		close(fd);
		return 10;
	}
	close(fd);
	return  0;
}


void * http_handler(void  * arg)
{
	//得到一个客户端套接字
	int sock = (int)arg;
#ifdef _DEBUG_
	char buf[SIZE];
	do
	{
		int ret = get_line(sock,buf,sizeof(buf));
		if(ret > 0 )
			printf("%s\n",buf);
		else 
		{
			break;
		}
	}while(1);
#endif
//1、得到http协议的请求行
	int ret = 0;
	//先得到第一行的http的请求行;
	char  http[SIZE];
	char  path[SIZE];//绝对的资源路径
	char* query_string = NULL;//表示的是请求的参数
	int   cgi = 0 ;//表示请求资源 的 模式
	char  method[SIZE/10];//请求方法
	char  url[SIZE];//资源路径
	get_line(sock,http,sizeof(http));
	if(strlen(http) ==0 )
	{
		ret = 5;
		goto end;
	}
//2、得到请求行中的请求方法与资源路径
	//在请求行中得到 http的请求方法 与 资源路径
	int  i ,j;
	i= 0;//i--》http[i];
	j= 0;//j--》mothed[j];
	while(http[i] != '\0'&&
			http[i] != ' '&&
			j < sizeof(method)-1)
	{
		method[j] = http[i];
		i++;
		j++;
	}
	method[j] = 0 ;
	//当前  i--》 ‘ ’ 但是可能请求行中的空格不止一个
	while(http[i] != '\0'&&http[i]== ' ')
	{
		i++;
	}
	//此时出来之后，，，i---》资源路径
	//j --》url[j];
	j =0;
	while(http[i] != '\0'&&
			http[i] != ' '&&
			j < sizeof(method)-1)
	{
		url[j] = http[i];
		j++;
		i++;
	}
	url[j] =0;
	//到此我们得到http请求的方法 与 请求的资源路径
//3、找到请求的参数  query_string
	query_string = url;
	while(*query_string != '\0')
	{
		if(*query_string == '?')
		{
			cgi = 1;//表示找到了参数
			*query_string = '\0';
			query_string++;
			break;
		}
		query_string++;
	}
//4、得到资源的绝对路径
	sprintf(path,"wwwroot%s",url);
	if(path[strlen(path) -1] == '/')
	{
		//表示的是当前的资源路径为 目录文件
		strcat(path,"index.html");
	}
	//判断路径的合法性
	struct stat st;
	if(stat(path,&st) != 0)
	{
		//表示的是路径不是合法的
		ret = 6;
		echo_string(sock);
		goto end;
	}else{
		if(S_ISDIR(st.st_mode))//表示的是 不以 / 结尾的目录
		{
			strcat(path,"/index.html");
		}else if((st.st_mode & S_IXUSR)||
				 (st.st_mode & S_IXGRP)||
				 (st.st_mode & S_IXOTH))
		{
			//表示的是可执行的文件
			cgi =1;
		}
		else//普通的文件
		{}
	}
//5、判断http的请求方法类型 ：在次我们只需要考虑到get 、post方法
	if(strcasecmp(method,"GET")&&strcasecmp(method,"POST"))
	{
		//表示当前的方式不是GET 也不是 POST方
		echo_string(sock);
		ret = 7;
		goto end;
	}
	if(strcasecmp(method,"POST") == 0)
	{
		//当前的http请求方法为 POST方法
		cgi =1;
	}

//6、对于不同的请求模式 实现不同的方法
	if(cgi)
	{
		//CGI模式
		printf("cgi enter \n");
		printf("method = %s\n",method);
		exe_cgi(sock,method,path,query_string);
	}else{
		//normal模式
		//先清除 头部
		drop_header(sock);
		echo_www(sock,path,st.st_size);
	}

end:
	close(sock);
	return  (void*)ret;
}
