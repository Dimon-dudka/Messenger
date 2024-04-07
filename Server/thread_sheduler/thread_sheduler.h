#pragma once

#include <QTcpSocket>
#include <QMutex>
#include <QHash>
#include <QMap>
#include <QPointer>
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QThread>

#include "request_types.h"

class thread_sheduler : public QObject
{
    Q_OBJECT

private:

    QJsonObject input_data;

    SQL_STATE sql_answer;
    quintptr thread_user_socket_desc;
    QByteArray user_request;

    QHash<QString,quintptr> login_and_socket_table;
    QMap<quintptr,QString> socket_and_login_table;

    QVector<QString> list_of_users;
    QVector<Message_from_DB>message_story;

    //  Registration slots
    void registration_answer_slot(const QString &login);

    //  Login slots
    void login_answer_slot(const QString &login);

    //  Find Users slots
    void logins_list_answer_slot(const QString& login);

    //  Message slots
    void send_message_slot(const QString &login_from,const QString &login_to,const QString &message);

    //  Message history slot
    void send_message_history_slot();

public slots:

    void read_from_socket();

    //  SQL slots

    void request_answer_slot(const Answer_to_thread& request);

    void become_users_list_answer_slot(const QVector<QString>logins_list);

    void become_message_history_slot(const QVector<Message_from_DB>& data);

public:
    thread_sheduler(quintptr user_socket, const QByteArray& data,
                    QHash<QString,quintptr> login_and_socket_table_from,
                    QMap<quintptr,QString>socket_and_login_table_from);

    QMutex g_mutex;

signals:

    void finished();
    void answer_signal(quintptr,QByteArray);
    void logger_signal(TypeError,QString,QString,QString);

    //  Signal to sql_engine
    void request_to_sql_signal(Request_struct);

};

