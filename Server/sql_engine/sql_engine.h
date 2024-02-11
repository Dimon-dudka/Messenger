//  SQL Engine provides an interface to answer on server requests
//  insert,update and read data from databases

#pragma once

//  STL containers
#include <vector>
#include <tuple>

#include <QtSql>
#include <QObject>
#include <QString>
#include <QPointer>
#include <QCoreApplication>

#include "logger.h"

class sql_engine : public QObject
{
    Q_OBJECT
private:

    QSqlDatabase sql_connection;
    QSqlQuery* data_base_query;
    QString data_base_directory;

    QPointer<logger>logger_api;

    void open_data_bases();

    //  Change connection to another table
    void change_connection_to_user_info();
    void change_connection_to_messages();

public:
    sql_engine(QObject *parrent = 0,logger* log = 0);

public slots:

    void user_registration(const QString &login,const QString &password);
    void user_login(const QString &login,const QString &password);
    void find_users(const QString &login,const QString& part_of_login);
    void become_message_history(const QString &login_first,const QString &login_second,const QString &date);
    void insert_message(const QString &login_from,
                        const QString &login_to,const QString &message,const QString &datetime);


signals:
    //  Registration signals
    void user_already_exists_signal();
    void registration_success_signal();

    //  Login signals
    void login_not_exists_signal();
    void login_incorrect_pass_signal();
    void login_success_signal();

    //  become users signals
    void list_of_logins_signal(const std::vector<QString>);
    void users_not_found_signal();

    //  become messeges list signals
    void messeges_list_signal(std::vector<std::tuple<QString,QString,QString>>);

    void stop_server_signal();

};
