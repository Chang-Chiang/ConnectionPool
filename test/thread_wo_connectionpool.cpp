#include <iostream>

#include "Connection.h"

// wsl2 ubuntu: g++ -g -o thread_wo_connectionpool thread_wo_connectionpool.cpp -I../include ../src/*.cpp -lmysqlclient
// vmware centos: g++ -g -o thread_wo_connectionpool thread_wo_connectionpool.cpp -I../include ../src/*.cpp  -L/usr/lib64/mysql -lmysqlclient -lpthread
// aliyun: g++ -g -o thread_wo_connectionpool thread_wo_connectionpool.cpp -std=c++11 -I../include ../src/*.cpp  -L/usr/lib64/mysql -lmysqlclient -lpthread

int main() {

    // 无连接池 单线程 访问数据库 1000 次、5000 次、10000 次
    clock_t begin = clock();
    for (int i = 0; i < 1000; ++i) {
        Connection conn;
        char       sql[1024] = {0};
        sprintf(
            sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
        conn.connect("127.0.0.1", 3306, "root", "123456", "test_connectionpool");
        conn.update(sql);
    }
    clock_t end = clock();
    std::cout << "[1000 times takes]: " << (end - begin) << "us" << std::endl;
    // std::cout << "[1000 times takes]: " << (double)(end - begin) / CLOCKS_PER_SEC << "s" << std::endl;

    return 0;
}
