cmake_minimum_required(VERSION 3.5)

project(Server VERSION 1.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Sql)

if(QT_FOUND)
    message("QT found")
else()
    message(FATAL_ERROR "Cannot find QT")
endif()

set(HEADERS
        ${CMAKE_SOURCE_DIR}/main_server/main_server.h
        ${CMAKE_SOURCE_DIR}/sql_engine/sql_engine.h
        ${CMAKE_SOURCE_DIR}/logger/logger.h
        ${CMAKE_SOURCE_DIR}/thread_sheduler/thread_sheduler.h
        ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads/user_info_thread/user_info_thread.h
        ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads/messages_info_thread/messages_info_thread.h
        ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads/request_types.h
)

set(SOURCES
        main.cpp
        ${CMAKE_SOURCE_DIR}/main_server/main_server.cpp
        ${CMAKE_SOURCE_DIR}/sql_engine/sql_engine.cpp
        ${CMAKE_SOURCE_DIR}/logger/logger.cpp
        ${CMAKE_SOURCE_DIR}/thread_sheduler/thread_sheduler.cpp
        ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads/user_info_thread/user_info_thread.cpp
        ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads/messages_info_thread/messages_info_thread.cpp
)

add_executable(Server ${SOURCES} ${HEADERS}
    ${CMAKE_SOURCE_DIR}/resources/server_structure.qmodel
)

target_include_directories(Server
    PUBLIC ${CMAKE_SOURCE_DIR}/main_server
    PUBLIC ${CMAKE_SOURCE_DIR}/sql_engine
    PUBLIC ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads
    PUBLIC ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads/messages_info_thread
    PUBLIC ${CMAKE_SOURCE_DIR}/sql_engine/sql_threads/user_info_thread
    PUBLIC ${CMAKE_SOURCE_DIR}/logger
    PUBLIC ${CMAKE_SOURCE_DIR}/resources
    PUBLIC ${CMAKE_SOURCE_DIR}/thread_sheduler
)

target_link_libraries(Server
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
)
