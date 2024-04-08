//  SQL Engine provides an API to send request into DB

#pragma once

#include <QThread>
#include <QPointer>

#include "messages_info_thread.h"
#include "user_info_thread.h"

#include "request_types.h"

class sql_engine : public QObject
{
    Q_OBJECT
private:
    QPointer<QThread>messages_thr,user_info_thr;

    QPointer<messages_info_thread> messages_DB;
    QPointer<user_info_thread> user_info_DB;

public:
    sql_engine(QObject *parrent = 0);

public slots:
    void get_user_request(const Request_struct& user_request);

private slots:
    void become_result_and_send_slot(const Answer_to_thread& user_answer);
    void become_logger_message_slot(const Logger_message& message);
    void stop_server_slot();

signals:
    void answer_request(Answer_to_thread);
    void logger_signal(Logger_message);
    void stop_server_signal();
};
