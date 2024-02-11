
#include "sql_engine.h"

sql_engine::sql_engine(QObject *parrent,logger* log):QObject(parrent),logger_api(log)
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
        logger_api->message_handler(logger::TypeError::FATAL,"sql_engine"
                                    ,"change_connection_to_user_info"
                                    ,"unable to open user_info database!");
        emit stop_server_signal();
        return;
    }

    data_base_query->clear();
}

void sql_engine::change_connection_to_messages(){
    data_base_query->clear();

    sql_connection.setDatabaseName(data_base_directory+"/messages_info.sqlite");

    if(!sql_connection.open()){
        qDebug()<<"User messages DB connection  FAIL!";
        logger_api->message_handler(logger::TypeError::FATAL,"sql_engine"
                                    ,"change_connection_to_user_info"
                                    ,"unable to open messages_info database!");
        emit stop_server_signal();
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
        logger_api->message_handler(logger::TypeError::FATAL,"sql_engine"
                                    ,"open_data_bases"
                                    ,"unable to create user_info database: "+data_base_query->lastError().text());
        emit stop_server_signal();
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
        logger_api->message_handler(logger::TypeError::FATAL,"sql_engine"
                                    ,"open_data_bases"
                                    ,"unable to create messages database: "+data_base_query->lastError().text());
        emit stop_server_signal();
        return;
    }
}

void sql_engine::user_registration(const QString &login,const QString &password){
    change_connection_to_user_info();

    QString query_text{"INSERT INTO user_info (login, password) "
                       "VALUES ( '"+login+"', '"+password+"' );"};

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
        logger_api->message_handler(logger::TypeError::ERROR,"sql_engine"
                                    ,"user_login"
                                    ,"executing sql query fail: "+data_base_query->lastError().text());
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
        logger_api->message_handler(logger::TypeError::ERROR,"sql_engine"
                                    ,"find_users"
                                    ,"executing sql query fail: "+data_base_query->lastError().text());
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

void sql_engine::become_message_history(const QString &login_first,const QString &login_second,const QString &date){
    change_connection_to_messages();

    QString query_text{"SELECT sender, date_time, message FROM messages "
                       "WHERE ((sender = '"+login_first+"' AND getter = '"+login_second+"' ) "
                       "OR    (sender = '"+login_second+"' AND getter = '"+login_first+"' )) "
                       "AND date_time < '"+date+"' "
                       "ORDER BY date_time DESC ;"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query in becoming message story!";
        logger_api->message_handler(logger::TypeError::ERROR,"sql_engine"
                                    ,"become_message_history"
                                    ,"executing sql query fail: "+data_base_query->lastError().text());
        return;
    }

    std::vector<std::tuple<QString,QString,QString>>data_from_sql;

    unsigned int count{0};

    while(data_base_query->next()&&count<20){
        data_from_sql.push_back(std::make_tuple(data_base_query->value(0).toString(),
                                                data_base_query->value(1).toString(),
                                                data_base_query->value(2).toString()));
        count+=1;
    }

    if(data_from_sql.empty()){
        return;
    }

    emit messeges_list_signal(std::move(data_from_sql));
}

void sql_engine::insert_message(const QString &login_from,
                         const QString &login_to,const QString &message,const QString &datetime){
    change_connection_to_messages();

    QString query_text{"INSERT INTO messages ( sender , getter , date_time , message ) "
                " VALUES ('"+login_from+"' , '"+login_to+"' , '"+datetime+"' , '"+message+"');"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query!";
        logger_api->message_handler(logger::TypeError::ERROR,"sql_engine"
                                    ,"insert_message"
                                    ,"executing sql query fail: "+data_base_query->lastError().text());
        return;
    }
}
