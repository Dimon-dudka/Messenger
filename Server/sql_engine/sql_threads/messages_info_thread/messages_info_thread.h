#pragma once

#include <QtSql>
#include <QQueue>
#include <QObject>
#include <QVector>
#include <QString>
#include <QPointer>
#include <QCoreApplication>

#include <tuple>

#include "request_types.h"

class messages_info_thread : public QObject
{
    Q_OBJECT

private:
    bool flag_of_work;

    QString data_base_directory;
    QSqlDatabase sql_connection;
    QSqlQuery* data_base_query;

    QQueue<Request_struct> requests_queue;

    void open_data_base();

    void become_message_history(const Request_struct& user_request);
    void insert_message(const Request_struct& user_request);

public slots:

    void work_slot();
    void stop_work()noexcept;
    void add_to_queue_slot(const Request_struct& user_request);

public:
    messages_info_thread(QObject * parrent = 0);

signals:

    void logger_signal(TypeError,QString,QString,QString);

    //  become messeges list signals
    //void messeges_list_signal(Qt::HANDLE,QVector<Message_from_DB>);
    //void messages_list_fail_signal(Qt::HANDLE);

    //  Message insert signals
    //void message_insert_success_signal(Qt::HANDLE);
    //void message_insert_fail_signal(Qt::HANDLE);

    void answer_request(Answer_to_thread);

    //  Thread control
    void finished();
    void stop_server_signal();

};
