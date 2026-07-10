cmake_minimum_required(VERSION 3.20)
project(campus_secondhand VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# 查找依赖包
find_package(Drogon CONFIG REQUIRED)
find_package(mysql-connector-c CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# 设置源文件目录
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# 收集源文件
file(GLOB_RECURSE SRC_FILES 
    ${SRC_DIR}/controller/*.cpp
    ${SRC_DIR}/service/*.cpp
    ${SRC_DIR}/dao/*.cpp
    ${SRC_DIR}/model/*.cpp
    ${SRC_DIR}/utils/*.cpp
    ${SRC_DIR}/middleware/*.cpp
)

# 添加可执行文件
add_executable(${PROJECT_NAME} 
    ${SRC_DIR}/main.cpp
    ${SRC_FILES}
)

# 添加头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}
    ${PROJECT_SOURCE_DIR}/config
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Drogon::Drogon
    mysql-connector-c::mysql-connector-c
    redis++::redis++_static
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
)

# 复制配置文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/config $<TARGET_FILE_DIR:${PROJECT_NAME}>/config
)

# 复制资源目录到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

# 复制前端文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/views $<TARGET_FILE_DIR:${PROJECT_NAME}>/views
)