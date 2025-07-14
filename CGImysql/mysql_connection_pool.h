#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include "../lock/locker.h"
#include "../log/log.h"
#include <error.h>
#include <iostream>
#include <list>
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include <string>

class connection_pool {
public:
  MYSQL *GetConnection();
  bool ReleaseConnection(MYSQL *conn);
  int GetFreeConn();
  void DestroyPool();

  static connection_pool *GetInstance();
  void init(string url, string user, string passwd, string dataBaseName,
            int port, int maxConn, int close_log);

private:
  connection_pool();
  ~connection_pool();

  int m_maxConn;
  int m_curConn;
  int m_freeConn;

  locker lock;
  std::list<MYSQL *> connList;
  sem reserve;

public:
  string m_url;
  string m_port;
  string m_user;
  string m_passwd;
  string m_dataBaseName;
  int m_close_log;
};

class connectionRAII {
public:
  connectionRAII(MYSQL **con, connection_pool *connPool);
  ~connectionRAII();

private:
  MYSQL *connRAII;
  connection_pool *poolRAII;
};

#endif
