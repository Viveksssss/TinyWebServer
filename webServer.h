#ifndef __WEBSERVER_H_
#define __WEBSERVER_H_

#include <arpa/inet.h>
#include <cassert>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./http/http_conn.h"
#include "./threadpool/threadpool.h"
#include "timer/lst_timer.h"

const int MAX_FD = 65536;           // 最大的文件描述符
const int MAX_EVENT_NUMBER = 10000; // 最大的事件数
const int TIMESLOT = 5;             // 最小的超时单位
using namespace std;

class WebServer
{
  public:
    WebServer();
    ~WebServer();

    void init(int port, std::string user, std::string passwd, std::string dataBaseName, int log_write, int opt_linger,
              int trigmode, int sql_num, int thread_num, int close_log, int actor_model);

    void thread_pool();
    void sql_pool();
    void log_write();
    void trig_mode();
    void eventListen();
    void eventLoop();
    void timer(int connfd, struct sockaddr_in client_address);
    void adjust_timer(util_timer *timer);
    void deal_timer(util_timer *timer, int sockfd);
    bool dealClientData();
    bool dealWithSignal(bool &timeout, bool &stop_server);
    void dealWithRead(int sockfd);
    void dealWithWrite(int sockfd);

  public:
    /* 基本变量 */
    int m_port;
    char *m_root;
    int m_log_write;
    int m_close_log;
    int m_actorModel;

    int pipefd[2]; // 管道通信
    int m_epollfd;
    http_conn *users;

    /* 连接数据库 */
    connection_pool *m_connPool;
    std::string m_user;
    std::string m_passwd;
    std::string m_dataBaseName;
    int m_sql_num;

    /* 线程池 */
    threadpool<http_conn> *m_pool;
    int m_thread_num;

    /* epoll_event */
    epoll_event events[MAX_EVENT_NUMBER];

    int m_listenfd;
    int m_opt_linger;
    int m_trigmode;
    int m_listenTrigmode;
    int m_connTrigmode;

    /* 定时器 */
    client_data *users_timer;
    Utils utils;
};

#endif
