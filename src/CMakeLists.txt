# 定义了SRC_LIST变量，包含当前目录下所有的源文件
aux_source_directory(. SRC_LIST)

# 编译动态库
add_library(connectionpool SHARED ${SRC_LIST})
# 指定动态链接时需要依赖的库文件
target_link_libraries(connectionpool mysqlclient pthread)