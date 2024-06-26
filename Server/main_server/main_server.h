//  Class that describes server API
//  and main server logic

#pragma once

#include <QHash>
#include <QObject>
#include <QString>
#include <QThread>
#include <QVector>
#include <QPointer>
#include <QJsonArray>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

#include "sql_engine.h"
#include "logger.h"
#include "thread_sheduler.h"

class main_server : public QObject
{
    Q_OBJECT
private:

    bool is_server_stoped;

    QHash<QString,quintptr> login_and_socket_table;
    QMap<quintptr,QString> socket_and_login_table;

    QMap<quintptr,QTcpSocket*>desc_and_socket;

    QPointer<logger> logger_api;
    QPointer<sql_engine> sql_engine_api;

    QPointer<QTcpServer> user_server;
private slots:

    void stop_server_slot();

    void new_connection_slot();
    void ready_read_slot();

    void socket_disconnect_slot();

    void become_table_insert_slot(QString login,quintptr desc);

    //  Answer on thread signals
    void send_answer_slot(const quintptr &user_desc,const QByteArray &user_answer);

public:
    explicit main_server(QObject * parent = 0);
    ~main_server() = default;

signals:
    void stop_threads();
};
