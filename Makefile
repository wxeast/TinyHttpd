ROOT_PATH=$(shell pwd) #得到根目录的路径
CONF=$(ROOT_PAth)/conf #得到配置文件的路径
PLUGIN=$(ROOT_PATH)/plugin #得到脚本文件的路径
WWWROOT=$(ROOT_PATH)/wwwroot #得到当前的工作目录

FLAGS= -D_STDOUT_  #
LDFLAGS= -g -lpthread #-static
bin=httpd          
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g')
$(bin):$(obj)
	gcc -o $@  $^ $(LDFLAGS)
%.o:%.c
	gcc -c $< $(FLAGS)

.PHONY:clean
clean:
	rm -f $(obj) $(bin)
.PHONY:debug
debug:
	echo $(src)
	echo $(obj)
	echo $(bin)
