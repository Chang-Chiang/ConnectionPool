#pragma once
#include <mysql/mysql.h>

#include <ctime>
#include <string>

/**
 * @brief 实现 MySQL 数据库的操作
 *
 */
class Connection {
public:
    // 初始化数据库连接
    Connection();

    // 释放数据库连接资源
    ~Connection();

    // 连接数据库
    bool connect(
        std::string ip, unsigned short port, std::string user, std::string password,
        std::string dbname);

    // 更新操作 insert、delete、update
    bool update(std::string sql);

    // 查询操作 select
    MYSQL_RES* query(std::string sql);

    // 刷新连接的起始的空闲时间点
    void refreshAliveTime() { _alivetime = clock(); }

    // 返回存活的时间
    clock_t getAliveTime() const { return clock() - _alivetime; }

private:
    MYSQL*  _conn;      // 表示和 MySQL Server 的一条连接
    clock_t _alivetime; // 记录进入空闲状态后的起始存活时间
};