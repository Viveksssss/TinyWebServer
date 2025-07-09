cmake_minimum_required(VERSION 3.10.0)
project(TinyWebServer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 自动收集所有源文件
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*/[^b]*.cpp"  # 排除build开头的目录
    "${CMAKE_CURRENT_SOURCE_DIR}/*/[^b]*.cpp"
)
# 创建可执行文件
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${HEADERS}  # 可选：将头文件也添加到项目中以便IDE显示
)