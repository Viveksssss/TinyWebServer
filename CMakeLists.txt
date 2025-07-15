cmake_minimum_required(VERSION 3.10)
project(TinyWebServer)

set(CMAKE_CXX_STANDARD 17)

# 包含所有源文件
file(GLOB SOURCES
        "*.cpp"
        "./http/*.cpp"
        "./CGImysql/*.cpp"
        # "./log/*.cpp"
        "./threadpool/*.h"
        "./timer/*.cpp"
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接必要的库
target_link_libraries(${PROJECT_NAME} pthread mysqlclient)