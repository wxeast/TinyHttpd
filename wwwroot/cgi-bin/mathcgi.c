#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

void  mymath(char * arg)
{
	printf("%s\n",arg);
	char *argv[3];
	char  *start = arg;
	int  i = 0;
	while(*start)
	{
		if(*start == '=')
		{
			argv[i++] = ++start;
		}
		else if(*start == '&')
		{
			*start = 0;	
			++start;
		}
		else
		{
			++start;
		}
	}
	argv[i] = NULL;
	int data1= atoi(argv[0]);
	int data2= atoi(argv[1]);
	printf("<html><body>");
	printf("%d + %d = %d<br/>",data1,data2,data1+data2);
	printf("%d - %d = %d<br/>",data1,data2,data1-data2);
	printf("%d * %d = %d<br/>",data1,data2,data1*data2);
	printf("%d / %d = %d<br/>",data1,data2,data2 == 0 ? 0 :data1+data2);
	printf("%d %% %d = %d<br/>",data1,data2,data2 == 0 ? 0 :data1%data2);
	printf("</body></html>");
	
}
int main()
{
	printf("Hello CGI\n");
	char * method = NULL;
	char query_string[1024]={0};
	int content_length = -1;
	if((method =getenv("METHOD")) != NULL)
	{
		if(strcasecmp(method,"GET") == 0)
		{
			//get方法
			char * tmp = NULL;
			if((tmp  =getenv("QUERYSTRING"))!= NULL)
			{
				strcpy(query_string,tmp);
			}

		}
		if(strcasecmp(method,"POST") == 0)
		{
			//post方法
			if(getenv("CONTENTLENGTH")!= NULL)
			{
				content_length = atoi(getenv("CONTENTLENGTH"));
			}
			if(content_length >  0)
			{
				int  i = 0 ;
				for(;i < content_length;++i)
				{
					read(0,&query_string[i],1);
				}
				query_string[i] = '\0';
			}
		}
	}
	assert(query_string);
	mymath(query_string);
	return  ;
}
