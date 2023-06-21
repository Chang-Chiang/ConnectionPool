# 指定头文件路径
INCL = -I../include

# 指定 cpp 文件路径
CPP = ../src/*.cpp

# 编译参数
CFLAGS = -g 

# 需要编译的文件
all:thread_wo_connectionpool thread_w_connectionpool threads_wo_connectionpool threads_wo_connectionpool

# 文件的编译规则
thread_wo_connectionpool:thread_wo_connectionpool.cpp  # 所依赖的文件
	g++ $(CFLAGS) -o thread_wo_connectionpool thread_wo_connectionpool.cpp $(INCL) $(CPP) -lmysqlclient
	cp thread_wo_connectionpool ../bin/.
thread_w_connectionpool:thread_w_connectionpool.cpp  
	g++ $(CFLAGS) -o thread_w_connectionpool thread_w_connectionpool.cpp $(INCL) $(CPP) -lmysqlclient -lpthread 
	cp thread_w_connectionpool ../bin/.
threads_wo_connectionpool:threads_wo_connectionpool.cpp  
	g++ $(CFLAGS) -o threads_wo_connectionpool threads_wo_connectionpool.cpp $(INCL) $(CPP) -lmysqlclient -lpthread
	cp threads_wo_connectionpool ../bin/.
threads_w_connectionpool:threads_w_connectionpool.cpp  
	g++ $(CFLAGS) -o threads_w_connectionpool threads_w_connectionpool.cpp $(INCL) $(CPP) -lmysqlclient -lpthread
	cp threads_w_connectionpool ../bin/.

# 编译的清理
clean:
	rm -f thread_wo_connectionpool thread_w_connectionpool threads_wo_connectionpool threads_wo_connectionpool