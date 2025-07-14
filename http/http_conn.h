#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <map>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../CGImysql/mysql_connection_pool.h"
#include "../lock/locker.h"
#include "../log/log.h"
#include "../timer/lst_timer.h"

class http_conn
{
  public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    enum class METHOD
    {
        GET,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };

    enum class CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };

    enum class HTTP_CODE
    {
        NO_REQUEST,  // the request is not complete
        GET_REQUEST, // the complete request
        BAD_REQUEST, // str error
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR, // the internnal error
        CLOSED_CONNECTION
    };
    enum class LINE_STATUS
    {
        LINE_OK,
        LINE_BAD,
        LINE_OPEN
    };

  public:
    http_conn()
    {
    }
    ~http_conn()
    {
    }

  public:
    void init(int sockfd, const sockaddr_in &addr, char *, int, int, std::string user, std::string passwd,
              std::string sqlname);
    void close_conn(bool real_close = true);
    void process();
    bool read_once();
    bool write();
    sockaddr_in *get_address()
    {
        return &m_address;
    }
    void initmysql_result(connection_pool *connPool);
    int timer_flag;
    int improv;

  private:
    void init();
    HTTP_CODE process_read();
    bool process_write(HTTP_CODE ret);
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();

    char *get_line()
    {
        return m_read_buf + m_start_line;
    }
    LINE_STATUS parse_line();
    void unmap();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

  public:
    static int m_epollfd;
    static int m_user_count;
    MYSQL *mysql;
    int m_state; // 0 for read,and 1 for write
  private:
    // 网络连接相关变量
    int m_sockfd;          // 套接字文件描述符，用于客户端通信
    sockaddr_in m_address; // 存储客户端地址信息（IP+端口）

    // 读缓冲区及相关状态
    char m_read_buf[READ_BUFFER_SIZE]; // 存储从客户端读取的原始数据
    long m_read_idx;                   // 读缓冲区当前写入位置
    long m_checked_idx;                // 当前已解析的数据位置
    int m_start_line;                  // 当前解析行的起始位置（用于HTTP行解析）

    // 写缓冲区及相关状态
    char m_write_buf[WRITE_BUFFER_SIZE]; // 存储要发送给客户端的数据
    int m_write_idx;                     // 写缓冲区当前写入位置

    // HTTP解析状态机
    CHECK_STATE m_check_state; // 当前HTTP解析状态（如解析请求行/头/体）
    METHOD m_method;           // HTTP请求方法（GET/POST等）

    // HTTP请求关键字段
    char *m_url;           // 请求URL（指向m_read_buf中的位置）
    char *m_version;       // HTTP版本（如HTTP/1.1）
    char *m_host;          // Host头字段内容
    long m_content_length; // 请求体长度（POST请求用）
    bool m_linger;         // 是否保持长连接（Connection: keep-alive）

    // 文件相关
    char m_real_file[FILENAME_LEN]; // 请求资源的本地路径
    char *m_file_address;           // 文件内存映射地址
    struct stat m_file_stat;        // 文件状态信息
    struct iovec m_iv[2];           // 分散写结构（用于writev优化）
    int m_iv_count;                 // iovec有效计数

    // CGI相关
    int cgi;        // 是否启用CGI模式（1=启用）
    char *m_string; // 存储CGI或扩展数据

    // 发送状态
    int bytes_to_send;   // 剩余待发送字节数
    int bytes_have_send; // 已发送字节数

    // 服务器配置
    char *doc_root;                             // 网站根目录路径
    std::map<std::string, std::string> m_users; // 用户数据库（模拟）
    int m_TRIGMode;                             // 触发模式（ET/LT）
    int m_close_log;                            // 是否关闭日志（调试用）

    // 数据库配置（模拟）
    char sql_user[100];   // 数据库用户名
    char sql_passwd[100]; // 数据库密码
    char sql_name[100];   // 数据库名
};

#endif
