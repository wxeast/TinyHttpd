ROOT_PATH=$(shell pwd)
LIB_PATH=$(ROOT_PATH)/lib/lib
INCLUDE_PATH=$(ROOT_PATH)/lib/include


.PHONY:all
all:insert_cgi select_cgi

insert_cgi:insert_cgi.o sql_api.o
	g++ -o $@ $^ -L$(LIB_PATH) -lmysqlclient 	#-l加上需要连接的库 -L加上库的搜索路径
select_cgi:select_cgi.o sql_api.o
	g++ -o $@ $^ -L$(LIB_PATH) -lmysqlclient 	#-l加上需要连接的库 -L加上库的搜索路径
%.o:%.cpp
	g++ -c $< -I$(INCLUDE_PATH) 	#-I加上头文件的搜索路径              


.PHONY:clean
clean:
	rm -f  insert_cgi select_cgi *.o
