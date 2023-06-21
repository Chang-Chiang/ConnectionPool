#include <iostream>
#include <thread>

#include "Connection.h"

// wsl2 ubuntu: g++ -g -o threads_wo_connectionpool threads_wo_connectionpool.cpp -I../include ../src/*.cpp -lmysqlclient -lpthread
// vmware centos: g++ -g -o threads_wo_connectionpool threads_wo_connectionpool.cpp -I../include ../src/*.cpp  -L/usr/lib64/mysql -lmysqlclient -lpthread
// aliyun: g++ -g -o threads_wo_connectionpool threads_wo_connectionpool.cpp -std=c++11 -I../include ../src/*.cpp  -L/usr/lib64/mysql -lmysqlclient -lpthread

int main() {

    // 无连接池 二线程 访问数据库 1000 次、5000 次、10000 次

    // 提前登录数据库
    Connection conn;
    conn.connect("127.0.0.1", 3306, "root", "123456", "test_connectionpool");

    clock_t begin = clock();

    std::thread t1([]() {
        for (int i = 0; i < 500; ++i) {
            Connection conn;
            char       sql[1024] = {0};
            sprintf(
                sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20,
                "male");
            conn.connect("127.0.0.1", 3306, "root", "123456", "test_connectionpool");
            conn.update(sql);
        }
    });
    std::thread t2([]() {
        for (int i = 0; i < 500; ++i) {
            Connection conn;
            char       sql[1024] = {0};
            sprintf(
                sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20,
                "male");
            conn.connect("127.0.0.1", 3306, "root", "123456", "test_connectionpool");
            conn.update(sql);
        }
    });

    t1.join();
    t2.join();

    clock_t end = clock();
    std::cout << (end - begin) << "us" << std::endl;
    // std::cout << (double)(end - begin) / CLOCKS_PER_SEC << "s" << std::endl;

    return 0;
}
