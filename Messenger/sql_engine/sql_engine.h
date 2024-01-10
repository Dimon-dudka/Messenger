//  Class describes sql engine for table
//  that contains logins with them
//  user already chatted

#pragma once

#include <QtSql>
#include <QObject>
#include <QString>
#include <QPointer>
#include <QCoreApplication>

class sql_engine: public QObject
{
    Q_OBJECT

private:

    QSqlDatabase sql_connection;
    QSqlQuery* data_base_query;
    QString data_base_directory;

    void open_data_bases();

public:
    sql_engine(QObject * parrent=0);
};
