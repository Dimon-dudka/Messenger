cmake_minimum_required(VERSION 3.5)

project(Messenger VERSION 0.1 LANGUAGES CXX)

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
    ${CMAKE_SOURCE_DIR}/widget_manager/widget_manager.h
    ${CMAKE_SOURCE_DIR}/login_menu/login_menu.h
    ${CMAKE_SOURCE_DIR}/registration_menu/registration_menu.h
    ${CMAKE_SOURCE_DIR}/fails_menu/fails_menu.h
    ${CMAKE_SOURCE_DIR}/messenger_main_menu/messenger_main_menu.h
    ${CMAKE_SOURCE_DIR}/find_users_menu/find_users_menu.h
    ${CMAKE_SOURCE_DIR}/sql_engine/sql_engine.h
    ${CMAKE_SOURCE_DIR}/logger/logger.h
)

set(SOURCES
    main.cpp
    ${CMAKE_SOURCE_DIR}/widget_manager/widget_manager.cpp
    ${CMAKE_SOURCE_DIR}/login_menu/login_menu.cpp
    ${CMAKE_SOURCE_DIR}/registration_menu/registration_menu.cpp
    ${CMAKE_SOURCE_DIR}/fails_menu/fails_menu.cpp
    ${CMAKE_SOURCE_DIR}/messenger_main_menu/messenger_main_menu.cpp
    ${CMAKE_SOURCE_DIR}/find_users_menu/find_users_menu.cpp
    ${CMAKE_SOURCE_DIR}/sql_engine/sql_engine.cpp
    ${CMAKE_SOURCE_DIR}/logger/logger.cpp
)

add_executable(Messenger ${SOURCES} ${HEADERS}
    resources/messenger_structure.qmodel
    )

target_include_directories(Messenger
    PUBLIC ${CMAKE_SOURCE_DIR}/resources
    PUBLIC ${CMAKE_SOURCE_DIR}/widget_manager
    PUBLIC ${CMAKE_SOURCE_DIR}/login_menu
    PUBLIC ${CMAKE_SOURCE_DIR}/registration_menu
    PUBLIC ${CMAKE_SOURCE_DIR}/fails_menu
    PUBLIC ${CMAKE_SOURCE_DIR}/messenger_main_menu
    PUBLIC ${CMAKE_SOURCE_DIR}/find_users_menu
    PUBLIC ${CMAKE_SOURCE_DIR}/sql_engine
    PUBLIC ${CMAKE_SOURCE_DIR}/logger
)

target_link_libraries(Messenger
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
)
