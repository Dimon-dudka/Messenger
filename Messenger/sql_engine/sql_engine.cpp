#include "sql_engine.h"

sql_engine::sql_engine(QObject * parrent,logger* log):QObject(parrent),logger_api(log)
{
    sql_connection = QSqlDatabase::addDatabase("QSQLITE");
    data_base_query = new QSqlQuery;
    data_base_directory = QFileInfo(QCoreApplication::applicationFilePath()).path();

    open_data_bases();
}

void sql_engine::open_data_bases(){

    data_base_query->clear();

    sql_connection.setDatabaseName(data_base_directory+"/chat_logins.sqlite");

    open_and_proof_DB_connection();

    QString query_text{"CREATE TABLE IF NOT EXISTS chat_logins("
                       "chat_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "login_own TEXT,"
                       "login_to TEXT,"
                       "frequency INTEGER"
                       ");"};

    if(!data_base_query->exec(query_text)){
        logger_api->message_handler(logger::TypeError::ERROR,"<sql_engine>"
                                    ,"open_data_bases"
                                    ,"creating/open database fail: "+data_base_query->lastError().text());
        return;
    }
    data_base_query->clear();

}

void sql_engine::open_and_proof_DB_connection(){
    data_base_query->clear();

    if(!sql_connection.open()){
        logger_api->message_handler(logger::TypeError::ERROR,"<sql_engine>"
                                    ,"open_and_proof_DB_connection"
                                    ,"database connection fail!");
        return;
    }
}

void sql_engine::select_logins_already_chatted_slot(const QString& user_login){

    open_and_proof_DB_connection();

    QString query_text{"SELECT login_to FROM chat_logins "
                       "WHERE login_own = '"+user_login+"' "
                       "ORDER BY frequency DESC;"};

    if(!data_base_query->exec(query_text)){
        logger_api->message_handler(logger::TypeError::ERROR,"<sql_engine>"
                                    ,"select_logins_already_chatted_slot"
                                    ,"sql query fail: "+data_base_query->lastError().text());
        return;
    }

    std::vector<QString>logins;

    while(data_base_query->next()){
        logins.push_back(data_base_query->value(0).toString());
    }

    if(!logins.empty()){
        emit logins_vector_signal(logins);
    }

}

void sql_engine::insert_new_chat_login_slot(const QString& user_login,const QString& login_to){

    open_and_proof_DB_connection();

    QString query_text{"SELECT COUNT(*) FROM chat_logins"
    " WHERE login_own ='"+user_login+"' AND login_to = '"+login_to+"' ;"};

    if(!data_base_query->exec(query_text)){
        logger_api->message_handler(logger::TypeError::ERROR,"<sql_engine>"
                                    ,"insert_new_chat_login_slot"
                                    ,"sql select query fail: "+data_base_query->lastError().text());
        return;
    }

    QString counter_tmp{"0"};
    while(data_base_query->next()){
        counter_tmp=data_base_query->value(0).toString();
    }

    data_base_query->clear();

    if(counter_tmp!="0"){
        return;
    }

    query_text="INSERT INTO chat_logins(login_own, login_to, frequency)"
                       "VALUES ('"+user_login+"' , '"+login_to+"' , '0');";

    if(!data_base_query->exec(query_text)){
        logger_api->message_handler(logger::TypeError::ERROR,"<sql_engine>"
                                    ,"insert_new_chat_login_slot"
                                    ,"sql select fail: "+data_base_query->lastError().text());
        return;
    }
    data_base_query->clear();
}

void sql_engine::update_frequency_slot(const QString& user_login,const QString& login_to){
    open_and_proof_DB_connection();

    QString query_text{"UPDATE chat_logins SET frequency = frequency + 1 "
                       "WHERE login_own = '"+user_login+"' AND login_to = '"+login_to+"' ;"};

    if(!data_base_query->exec(query_text)){
        logger_api->message_handler(logger::TypeError::ERROR,"<sql_engine>"
                                    ,"update_frequency_slot"
                                    ,"sql query fail: "+data_base_query->lastError().text());
        return;
    }
    data_base_query->clear();
}
