//  Class describes sql engine for table
//  that contains logins with them
//  user already chatted

#pragma once

#include <QtSql>
#include <vector>
#include <QObject>
#include <QString>
#include <QPointer>
#include <QCoreApplication>

//  logger
#include "logger.h"

class sql_engine: public QObject
{
    Q_OBJECT

private:

    QSqlDatabase sql_connection;
    QSqlQuery* data_base_query;
    QString data_base_directory;

    QPointer<logger>logger_api;

    void open_data_bases();
    void open_and_proof_DB_connection();

public:
    sql_engine(QObject * parrent=0,logger* log=0);

public slots:

    void insert_new_chat_login_slot(const QString& user_login,const QString& login_to);
    void update_frequency_slot(const QString& user_login,const QString& login_to);
    void select_logins_already_chatted_slot(const QString& user_login);

signals:
    //  select_logins signals
    void logins_vector_signal(std::vector<QString>);
};
