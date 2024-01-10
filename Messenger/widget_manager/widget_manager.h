//  Widget Manager
//  Compound of Widgets
//  And relations between them
//  And also client app backend

#pragma once

#include <QWidget>
#include <QString>
#include <QPointer>
#include <QTcpSocket>
#include <QApplication>
#include <QStackedWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

//  widgets
#include "fails_menu.h"
#include "login_menu.h"
#include "registration_menu.h"
#include "messenger_main_menu.h"
#include "find_users_menu.h"

enum class WHAT_FAIL{
    NONE,
    NETWORK_FAIL,
    REGISTRATION_BACKEND_FAIL,
    REGISTRATION_FRONTEND_FAIL,
    LOGIN_FRONTEND_FAIL,
    LOGIN_BACKEND_FAIL,
    FIND_USERS_FRONTEND_FAIL,
    FIND_USERS_BACKEND_FAIL,
};

enum class LOGIN_STATUS{
    OK,
    FAIL,
};

class widget_manager : public QStackedWidget
{
    Q_OBJECT
    
private:

    //  App status
    WHAT_FAIL fail_type;
    LOGIN_STATUS login_status;

    //  Socket items
    QPointer<QTcpSocket> user_socket;
    quint16 block_size;

    //  Backend items
    QString user_login;
    QString user_password;

    //  Widgets smart pointers
    QPointer <login_menu> login_menu_widget;
    QPointer <fails_menu> fails_menu_widget;
    QPointer <registration_menu> registration_menu_widget;
    QPointer <messenger_main_menu> messenger_menu_widget;
    QPointer <find_users_menu> find_users_widget;
    
private slots:

    //  Socket slots
    void on_socket_connected();
    void on_socket_disconnected();
    void on_socket_ready_read();
    void on_socket_display_error(QAbstractSocket::SocketError socket_error);
    bool connect_to_host();

    void register_login_to_server_slot(const QString& login,
                        const QString &password,const QString &type);

    void find_users_to_server_slot(const QString &part_of_login);


    //  Frontend slots
    void choise_slot_after_problem_menu();

    void set_current_fails_menu_slot();
    void set_current_login_menu_slot();
    void set_current_registration_menu_slot();
    void set_current_messenger_main_slot();
    void set_current_find_users_menu_slot();

    //  Fails slots
    void registration_frontend_fail_slot();
    void registration_backend_fail_slot();

    void login_backend_fail_slot();
    void login_frontend_fail_slot();

    void find_users_backend_fail_slot();
    void find_users_frontend_fail_slot();
    
public:
    
    widget_manager(QStackedWidget *parrent = 0);
    

};

