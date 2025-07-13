#ifndef LST_TIMER
#define LST_TIMER

#include <arpa/inet.h>
#include <assert.h>
#include <ctime>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../log/log.h"
#include <time.h>

class util_timer;
struct client_data {
  sockaddr_in address;
  int sockfd;
  util_timer *timer;
};

class util_timer {
public:
  util_timer() : prev(nullptr), next(nullptr) {}

public:
  time_t expire;

  void (*cb_func)(client_data *);
  client_data *user_data;
  util_timer *prev;
  util_timer *next;
};

class sort_timer_lst {
public:
  sort_timer_lst();
  ~sort_timer_lst();

  void add_timer(util_timer *timer);
  void adjust_timer(util_timer *timer);
  void del_timer(util_timer *);
  void tick();

private:
  void add_timer(util_timer *timer, util_timer *lst_head);
  util_timer *head;
  util_timer *tail;
};

class Utils {
public:
  Utils();
  ~Utils();

  void init(int timeslot);

  int setNonBlocking(int fd);

  void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

  static void sig_handler(int sig);

  void addsig(int sig, void(handler)(int), bool restart = true);

  void timer_handler();

  void show_error(int connfd, const char *info);

public:
  static int *u_pipefd;
  sort_timer_lst m_timer_lst;
  static int u_epollfd;
  int m_TIMESLOT;
};

void cb_func(client_data *user_data);

#endif
