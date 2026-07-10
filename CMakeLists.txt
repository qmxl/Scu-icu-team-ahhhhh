cmake_minimum_required(VERSION 3.10)
project(campus_trade)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include")
set(MYSQL_LIBRARY "C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/lib
    ${MYSQL_INCLUDE_DIR}
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/db/ConnectionPool.cpp
    ${PROJECT_SOURCE_DIR}/src/cache/CacheManager.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/HashUtil.cpp
    ${PROJECT_SOURCE_DIR}/src/service/UserService.cpp
    ${PROJECT_SOURCE_DIR}/src/service/GoodsService.cpp
    ${PROJECT_SOURCE_DIR}/src/service/TransactionService.cpp
    ${PROJECT_SOURCE_DIR}/src/service/ConfigService.cpp
    ${PROJECT_SOURCE_DIR}/src/service/StatisticsService.cpp
)

add_executable(campus_trade ${SOURCES})

target_link_libraries(campus_trade
    ${MYSQL_LIBRARY}
    ws2_32
)