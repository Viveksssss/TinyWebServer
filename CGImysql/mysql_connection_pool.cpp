#include "mysql_connection_pool.h"
#include <iostream>
#include <pthread.h>

using namespace std;

connection_pool::connection_pool() {
  m_curConn = 0;
  m_freeConn = 0;
}

connection_pool *connection_pool::GetInstance() {
  static connection_pool connPool;
  return &connPool;
}

void connection_pool::init(string url, string user, string passwd,
                           string dataBaseName, int port, int maxConn,
                           int close_log) {
  m_url = url;
  m_port = port;
  m_user = user;
  m_passwd = passwd;
  m_dataBaseName = dataBaseName;
  m_close_log = close_log;

  for (int i = 0; i < maxConn; ++i) {
    MYSQL *con = nullptr;
    con = mysql_init(con);

    if (con == nullptr) {
      LOG_ERROR("Mysql Error");
      exit(1);
    }

    con = mysql_real_connect(con, url.c_str(), user.c_str(), passwd.c_str(),
                             dataBaseName.c_str(), port, nullptr, 0);
    if (con == nullptr) {
      LOG_ERROR("Mysql Error");
      exit(1);
    }
    connList.push_back(con);
    ++m_freeConn;
  }
  reserve = sem(m_freeConn);
  m_maxConn = m_freeConn;
}

MYSQL *connection_pool::GetConnection() {
  MYSQL *con = nullptr;
  if (!connList.size()) {
    return nullptr;
  }
  reserve.wait();
  lock.lock();
  con = connList.front();
  connList.pop_front();

  --m_freeConn;
  ++m_curConn;

  lock.unlock();
  return con;
}

bool connection_pool::ReleaseConnection(MYSQL *con) {
  if (con == nullptr) {
    return false;
  }
  lock.lock();

  connList.push_back(con);
  ++m_freeConn;
  --m_curConn;

  lock.unlock();
  reserve.post();

  return true;
}

void connection_pool::DestroyPool() {
  lock.lock();

  if (connList.size() > 0) {
    list<MYSQL *>::iterator it;
    for (it = connList.begin(); it != connList.end(); ++it) {
      MYSQL *con = *it;
      mysql_close(con);
    }
    m_curConn = 0;
    m_freeConn = 0;
    connList.clear();
  }

  lock.unlock();
}

int connection_pool::GetFreeConn() { return this->m_freeConn; }

connection_pool::~connection_pool() { DestroyPool(); }

connectionRAII::connectionRAII(MYSQL **SQL, connection_pool *connPool) {
  *SQL = connPool->GetConnection();
  connRAII = *SQL;
  poolRAII = connPool;
}

connectionRAII::~connectionRAII() { poolRAII->ReleaseConnection(connRAII); }
