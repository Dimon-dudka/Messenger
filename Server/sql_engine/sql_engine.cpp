
#include "sql_engine.h"

sql_engine::sql_engine(QObject *parrent):QObject(parrent)
{
    sql_connection = QSqlDatabase::addDatabase("QSQLITE");
    data_base_query = new QSqlQuery;
    data_base_directory = QFileInfo(QCoreApplication::applicationFilePath()).path();

    open_data_bases();

}

void sql_engine::change_connection_to_user_info(){
    data_base_query->clear();

    sql_connection.setDatabaseName(data_base_directory+"/user_info.sqlite");

    if(!sql_connection.open()){
        qDebug()<<"User info DB connection  FAIL!";
        return;
    }

    data_base_query->clear();
}

void sql_engine::change_connection_to_messages(){
    data_base_query->clear();

    sql_connection.setDatabaseName(data_base_directory+"/messages_info.sqlite");

    if(!sql_connection.open()){
        qDebug()<<"User messages DB connection  FAIL!";
        return;
    }

    data_base_query->clear();

}

void sql_engine::open_data_bases(){

    change_connection_to_user_info();

    QString query_text{"CREATE TABLE IF NOT EXISTS user_info("
            "login TEXT PRIMARY KEY UNIQUE,"
            "password TEXT"
            ");"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"User info DB open FAIL!";
        return;
    }

    change_connection_to_messages();

    query_text= "CREATE TABLE IF NOT EXISTS messages("
                 "messege_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "sender TEXT,"
                 "getter TEXT,"
                 "date_time TEXT,"
                 "message TEXT"
                 ");";

    if(!data_base_query->exec(query_text)){
        qDebug()<<"User messages DB open FAIL!";
        return;
    }


}

void sql_engine::user_registration(const QString &login,const QString &password){
    change_connection_to_user_info();

    QString query_text{"INSERT INTO user_info (login, password)"
                       "VALUES ( '"+login+"', '"+password+"' )"};

    if(!data_base_query->exec(query_text)){
        emit user_already_exists_signal();
        return;
    }
    emit registration_success_signal();
}

void sql_engine::user_login(const QString &login,const QString &password){
    change_connection_to_user_info();

    QString query_text{"SELECT password FROM user_info "
                       "WHERE login == '"+login+"';"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query!";
        emit login_not_exists_signal();
        return;
    }

    QString tmp_pass;
    while(data_base_query->next()){
        tmp_pass = data_base_query->value(0).toString();
    }

    if(tmp_pass.isEmpty()){
        qDebug()<<"login not exists";
        emit login_not_exists_signal();
        return;
    }

    if(password!=tmp_pass){
        qDebug()<<"incorrect pass";
        emit login_incorrect_pass_signal();
        return;
    }

    qDebug()<<"login success";
    emit login_success_signal();
}

void sql_engine::find_users(const QString &login,const QString& part_of_login){
    change_connection_to_user_info();

    QString query_text{"SELECT login FROM user_info "
                       " WHERE login LIKE '"+part_of_login+"%' AND login != '"+login+"' ;"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query!";
        emit users_not_found_signal();
        return;
    }

    std::vector<QString>users_vector;
    while(data_base_query->next()){
        users_vector.push_back(data_base_query->value(0).toString());
    }

    if(users_vector.empty()){
        emit users_not_found_signal();
        return;
    }

    emit list_of_logins_signal(std::move(users_vector));
}
