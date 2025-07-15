CXX ?= g++
DEBUG ?= 1
CXXFLAGS += -std=c++11 
ifeq ($(DEBUG),1)
	CXXFLAGS += -g
else 
	CXXFLAGS += -O2
endif

server:main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/mysql_connection_pool.cpp webServer.cpp config.cpp
	$(CXX) -o server $^ $(CXXFLAGS) -lpthread -lmysqlclient

clean:
	rm -r server *.o