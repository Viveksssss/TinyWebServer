#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <unistd.h>
#include <signal.h>
#include <sys/type.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <map>

#include "../lock/locker.h"
#include "../CGImysql/sql_connection_pool.h"
#include "../timer/lst_timer.h"
#include "../log/log.g"

class http_conn{
    public:
        static const int FILENAME_LEN = 200;
        static const int READ_BUFFER_SIZE = 2048;
        static const int WRITE_BUFFER_SIZE = 1024;
        enum class METHOD{
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

        enum class CHECK_STATE{
            CHECK_STATE_REQUESTLINE,
            CHECK_STATE_HEADER,
            CHECK_STATE_CONTENT
        };

        enum class HTTP_CODE{
            NO_REQUEST, // the request is not complete
            GET_REQUEST, // the complete request
            BAD-REQUEST, // str error 
            NO_REQUEST,
            FORBIDDEN_REQUEST,
            FILE_REQUEST,
            INTERNAL_ERROR, // the internnal error
            CLOSED_CONNECTION
        };
        enum class LINE_STATUS{
            LINE_OK,
            LINE_BAD,
            LINE_OPEN
        };

    public:
        http_conn(){}
        ~http_conn(){}

    public:
        void init(int sockfd,const sockaddr_in &addr,char*,int,int,string user,string passwd,string sqlname);
        void close_conn(bool real_close = true);
        void process();
        void read_once();
        void write();
        sockaddr_in*get_address(){
            return &m_address;
        }
        void initmysql_result(connection_pool*connPool);
        int timer_flag;
        int improv;
    private:
        void init();
        HTTP_CODE process_read();
        bool process_write(HTTP_CODE ret);
        HTTP_CODE parse_request_line(char*text); 

};



#endif
