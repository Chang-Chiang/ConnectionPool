#include "Connection.h"

#include <iostream>

#include "public.h"

Connection::Connection() {
    // 初始化数据库连接
    _conn = mysql_init(nullptr);
}

Connection::~Connection() {
    // 释放数据库连接资源
    if (_conn != nullptr) {
        mysql_close(_conn);
    }
}

bool Connection::connect(
    std::string ip, unsigned short port, std::string username, std::string password,
    std::string dbname) {
    // 连接数据库
    // 加载配置文件这里连接不上啊
    // 问题 linux 与 windows 换行的区别
    // linux 默认 \n (LF), wondows \r\n (CRLF)
    MYSQL* p = mysql_real_connect(
        _conn, ip.c_str(), username.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
    // MYSQL* p = mysql_real_connect(
    //     _conn, "127.0.0.1", "root", "123456", "test_connectionpool", 3306, nullptr, 0);

    if (p != nullptr) {
        return true;
    } else {

        // fprintf(stderr, "Failed to connect to database: Error: %s\n",
        //   mysql_error(&mysql));
        LOG(mysql_error(_conn)); // 连接不成功时打印信息
        LOG("CONNECT FAILED!");
        return false;
    }
    // return p != nullptr;
}

bool Connection::update(std::string sql) {
    // 更新操作 insert、delete、update
    if (mysql_query(_conn, sql.c_str())) {
        LOG("更新失败:" + sql);
        return false;
    }
    return true;
}

MYSQL_RES* Connection::query(std::string sql) {
    // 查询操作 select
    if (mysql_query(_conn, sql.c_str())) {
        LOG("查询失败:" + sql);
        return nullptr;
    }
    return mysql_use_result(_conn);
}