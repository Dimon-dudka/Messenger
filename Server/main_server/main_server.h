//  Class that describes server API

#pragma once

#include <QHash>
#include <vector>
#include <QObject>
#include <QString>
#include <QPointer>
#include <QJsonArray>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>

#include "sql_engine.h"

enum class SQL_STATE{
    NONE,
    REGISTRATION_SUCCESS,
    REGISTRATION_FAIL,
    LOGIN_SUCCESS,
    LOGIN_FAIL_LOGIN,
    LOGIN_FAIL_PASSWORD,
    USERS_NOT_FOUND,
    USERS_FOUND_SUCCESS,
};

class main_server : public QObject
{
    Q_OBJECT
private:

    SQL_STATE sql_answer;

    QHash<QString,QPointer<QTcpSocket>> login_and_socket_table;

    QPointer<sql_engine> data_base;

    QPointer<QTcpServer> user_server;
    quint16 next_block_size;

    std::vector<QString> list_of_users;

private slots:

    void new_connection_slot();
    void ready_read_slot();

    //  SQL slots
    //  Registration slots
    void registration_answer_slot(QTcpSocket* user_socket,const QString &login);
    void registration_fail_slot();
    void registration_success_slot();

    //  Login slots
    void login_answer_slot(QTcpSocket * user_socket,const QString &login);
    void login_success_slot();
    void login_fail_login_slot();
    void login_fail_pass_slot();

    //  Find Users slots
    void logins_list_answer_slot(QTcpSocket * user_socket,const QString& login);
    void become_users_list_answer_slot(const std::vector<QString>logins_list);
    void users_not_found_slot();
    void users_found_success_slot();

public:
    explicit main_server(QObject * parent = 0);
};
