#include "sql_engine.h"

sql_engine::sql_engine(QObject * parrent):QObject(parrent)
{
    sql_connection = QSqlDatabase::addDatabase("QSQLITE");
    data_base_query = new QSqlQuery;
    data_base_directory = QFileInfo(QCoreApplication::applicationFilePath()).path();

    open_data_bases();
}

void sql_engine::open_data_bases(){

    data_base_query->clear();

    sql_connection.setDatabaseName(data_base_directory+"/chat_logins.sqlite");

    if(!sql_connection.open()){
        qDebug()<<"Chat_logins DB connection  FAIL!";
        return;
    }

    data_base_query->clear();

    QString query_text{"CREATE TABLE IF NOT EXISTS chat_logins("
                       "login TEXT PRIMARY KEY UNIQUE"
                       ");"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Chat_logins DB open FAIL!";
        return;
    }
    data_base_query->clear();

}
