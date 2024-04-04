#pragma once

#include <QTcpSocket>
#include <QHash>
#include <QMap>
#include <QPointer>
#include <QString>
#include <QJsonArray>
#include <QByteArray>
#include <QThread>

#include <vector>
#include <tuple>

#include "sql_engine.h"
#include "logger.h"

class thread_sheduler : public QObject
{
    Q_OBJECT

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

private:

    SQL_STATE sql_answer;

    QPointer<sql_engine> thread_connection_sql;
    QPointer<logger> thread_logger;

    quintptr thread_user_socket_desc;
    QByteArray user_request;

    //QPointer<QHash<QString,QPointer<QTcpSocket>>> login_and_socket_table;
    //QPointer<QMap<QPointer<QTcpSocket>,QString>> socket_and_login_table;
    QHash<QString,quintptr> login_and_socket_table;
    QMap<quintptr,QString> socket_and_login_table;

    std::vector<QString> list_of_users;
    std::vector<std::tuple<QString,QString,QString>>message_story;

public slots:

    void read_from_socket();

    //  SQL slots
    //  Registration slots
    void registration_answer_slot(const QString &login);
    void registration_fail_slot()noexcept;
    void registration_success_slot()noexcept;

    //  Login slots
    void login_answer_slot(const QString &login);
    void login_success_slot()noexcept;
    void login_fail_login_slot()noexcept;
    void login_fail_pass_slot()noexcept;

    //  Find Users slots
    void logins_list_answer_slot(const QString& login);
    void become_users_list_answer_slot(const std::vector<QString>logins_list);
    void users_not_found_slot()noexcept;
    void users_found_success_slot()noexcept;

    //  Message slots
    void send_message_slot(const QString &login_from,const QString &login_to,const QString &message);
    void send_message_history_slot();
    void become_message_history_slot(const std::vector<std::tuple<QString,QString,QString>>& data);

public:
    thread_sheduler(quintptr user_socket, const QByteArray& data,
                    QHash<QString,quintptr> login_and_socket_table_from,
                    QMap<quintptr,QString>socket_and_login_table_from);

signals:

    void finished();
    void answer_signal(quintptr,QByteArray);
    void logger_signal(logger::TypeError,QString,QString,QString);

};

