#include "CommonConnectionPool.h"

#include "public.h"

ConnectionPool* ConnectionPool::getConnectionPool() {
    // 静态局部变量初始化，编译器自动进行 lock 与 unlock
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::loadConfigFile() {
    FILE* pf = fopen("../config/mysql.conf", "r");
    if (pf == nullptr) {
        LOG("mysql.conf file does not exist!");
        return false;
    }

    while (!feof(pf)) {
        char line[1024] = {0};
        fgets(line, 1024, pf);
        std::string str = line;
        int         idx = str.find('=', 0);

        // 无效的配置项
        if (idx == -1) {
            continue;
        }

        // password=123456\n    LF
        // password=123456\r\n  CRLF
        int         endidx = str.find('\n', idx);
        std::string key = str.substr(0, idx);
        std::string value = str.substr(idx + 1, endidx - idx - 1);

        if (key == "ip") {
            _ip = value;
        }
        else if (key == "port") {
            _port = atoi(value.c_str());
        }
        else if (key == "username") {
            _username = value;
        }
        else if (key == "password") {
            _password = value;
        }
        else if (key == "dbname") {
            _dbname = value;
        }
        else if (key == "initSize") {
            _initSize = atoi(value.c_str());
        }
        else if (key == "maxSize") {
            _maxSize = atoi(value.c_str());
        }
        else if (key == "maxIdleTime") {
            _maxIdleTime = atoi(value.c_str());
        }
        else if (key == "connectionTimeOut") {
            _connectionTimeout = atoi(value.c_str());
        }
    }
    fclose(pf);
    return true;
}

ConnectionPool::ConnectionPool() {
    // 加载配置项
    if (!loadConfigFile()) {
        return;
    }

    // 创建初始数量的连接
    for (int i = 0; i < _initSize; ++i) {
        Connection* p = new Connection();
        p->connect(_ip, _port, _username, _password, _dbname);

        // 刷新开始空闲的起始时间
        p->refreshAliveTime();
        _connectionQue.push(p);
        _connectionCnt++;
    }

    // 启动一个新的线程，作为连接的生产者 linux thread => pthread_create
    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    // 启动一个新的定时线程，扫描超过 maxIdleTime 时间的空闲连接，进行对于的连接回收
    std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

void ConnectionPool::produceConnectionTask() {
    for (;;) {
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (!_connectionQue.empty()) {
            cv.wait(lock); // 队列不空，此处生产线程进入等待状态，释放锁
        }

        // 连接数量没有到达上限，继续创建新的连接
        if (_connectionCnt < _maxSize) {
            Connection* p = new Connection();
            p->connect(_ip, _port, _username, _password, _dbname);

            // 刷新开始空闲的起始时间
            p->refreshAliveTime();
            _connectionQue.push(p);
            _connectionCnt++;
        }

        // 通知消费者线程，可以消费连接了
        cv.notify_all();
    }
}

std::shared_ptr<Connection> ConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(_queueMutex);
    while (_connectionQue.empty()) {
        // 不要用 sleep！
        if (std::cv_status::timeout ==
            cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout))) {
            if (_connectionQue.empty()) {
                LOG("Timeout... Failed to obtain connection!");
                return nullptr;
            }
        }
    }

    // shared_ptr智能指针析构时，会把 connection 资源直接 delete 掉，
    // 相当于调用 connection 的析构函数，connection 就被 close 掉了。
    // 这里需要自定义 shared_ptr 的释放资源的方式，把 connection 直接归还到 queue 当中
    std::shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection* pcon) {
        // 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
        std::unique_lock<std::mutex> lock(_queueMutex);

        // 刷新开始空闲的起始时间
        pcon->refreshAliveTime();
        _connectionQue.push(pcon);
    });

    _connectionQue.pop();

    // 消费完连接以后，通知生产者线程检查一下
    // 如果队列为空了，赶紧生产连接
    cv.notify_all();

    return sp;
}

void ConnectionPool::scannerConnectionTask() {
    for (;;) {
        // 通过 sleep 模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

        // 扫描整个队列，释放多余的连接
        std::unique_lock<std::mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize) {
            Connection* p = _connectionQue.front();
            if (p->getAliveTime() >= (_maxIdleTime * 1000)) {
                _connectionQue.pop();
                _connectionCnt--;

                // 调用 ~Connection() 释放连接
                delete p;
            }
            else {

                // 队头的连接没有超过 _maxIdleTime，其它连接肯定没有
                break;
            }
        }
    }
}