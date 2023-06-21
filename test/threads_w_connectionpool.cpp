#include <iostream>

#include "CommonConnectionPool.h"

// wsl2 ubuntu: g++ -g -o threads_w_connectionpool threads_w_connectionpool.cpp -I../include ../src/*.cpp -lmysqlclient -lpthread
// vmware centos: g++ -g -o threads_w_connectionpool threads_w_connectionpool.cpp -I../include ../src/*.cpp  -L/usr/lib64/mysql -lmysqlclient -lpthread

int main() {
    
    // 有连接池 二线程 访问数据库 1000 次、5000 次、10000 次

    clock_t begin = clock();

    std::thread t1([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 500; ++i) {
            char sql[1024] = {0};
            sprintf(
                sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20,
                "male");
            std::shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
    });
    std::thread t2([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 500; ++i) {
            char sql[1024] = {0};
            sprintf(
                sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhang san", 20,
                "male");
            std::shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
    });

    t1.join();
    t2.join();

    clock_t end = clock();
    std::cout << (end - begin) << "us" << std::endl;
    // std::cout << (double)(end - begin) / CLOCKS_PER_SEC << "s" << std::endl;

    return 0;
}
