#include "user_info_thread.h"

user_info_thread::user_info_thread(QObject * parrent):QObject(parrent) {
    flag_of_work = true;

    data_base_directory = QFileInfo(QCoreApplication::applicationFilePath()).path();

}

void user_info_thread::open_data_base(){

    sql_connection = QSqlDatabase::addDatabase("QSQLITE",data_base_directory+"/user_info.sqlite");
    sql_connection.setDatabaseName(data_base_directory+"/user_info.sqlite");
    data_base_query = new QSqlQuery(sql_connection);
    sql_connection.setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");

    if(!sql_connection.open()){
        qDebug()<<"User info DB connection FAIL!";

        emit logger_signal({TypeError::FATAL,"<user_info_thread>"
                            ,"open_data_base"
                            ,"unable to open user_info database!"});

        emit stop_server_signal();
        emit finished();
        return;
    }

    data_base_query->clear();

    QString query_text{"CREATE TABLE IF NOT EXISTS user_info("
                       "login TEXT PRIMARY KEY UNIQUE,"
                       "password TEXT"
                       ");"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"User info DB open FAIL!";

        emit logger_signal({TypeError::FATAL,"<user_info_thread>"
            ,"open_data_base"
            ,"unable to create user_info database: "+data_base_query->lastError().text()});

        emit stop_server_signal();
        emit finished();
        return;
    }
    emit logger_signal({TypeError::INFO,"<user_info_thread>","open_data_bases",
                        "user_info.sqlite is open"});
}

void user_info_thread::work_slot(){

    open_data_base();

    while(flag_of_work){

        if(requests_queue.empty()){
            QThread::sleep(1);
            continue;
        }

        switch(requests_queue.front().type){
        case REQUEST_TYPES::REGISTER:
            user_registration(requests_queue.front());
            break;

        case REQUEST_TYPES::LOGIN:
            user_login(requests_queue.front());
            break;

        case REQUEST_TYPES::USERS_LIST:
            find_users(requests_queue.front());
            break;
        }

        requests_queue.pop_front();
    }

    emit logger_signal({TypeError::INFO,"<user_info_thread>"
                       ,"work_slot","Completion of the thread execution"});

    emit finished();

}

void user_info_thread::user_registration(const Request_struct& user_request){
    QString query_text{"INSERT INTO user_info (login, password) "
                       "VALUES ( '"+user_request.values.at(0)+"', '"+user_request.values.at(1)+"' );"};

    if(!data_base_query->exec(query_text)){
        emit answer_request({user_request.thread_id,SQL_STATE::REGISTRATION_FAIL,{},{}});
        return;
    }

    emit answer_request({user_request.thread_id,SQL_STATE::REGISTRATION_SUCCESS,{},{}});
}

void user_info_thread::user_login(const Request_struct& user_request){
    QString query_text{"SELECT password FROM user_info "
                       "WHERE login == '"+user_request.values.at(0)+"';"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query in user_login!";

        emit logger_signal({TypeError::ERROR,"<user_info_thread>"
                ,"user_login","executing sql query fail: "+data_base_query->lastError().text()});

        emit answer_request({user_request.thread_id,SQL_STATE::LOGIN_FAIL_LOGIN,{},{}});
        return;
    }

    QString tmp_pass;
    while(data_base_query->next()){
        tmp_pass = data_base_query->value(0).toString();
    }

    if(tmp_pass.isEmpty()){
        emit answer_request({user_request.thread_id,SQL_STATE::LOGIN_FAIL_LOGIN,{},{}});
        return;
    }

    if(user_request.values.at(1)!=tmp_pass){
        emit answer_request({user_request.thread_id,SQL_STATE::LOGIN_FAIL_PASSWORD,{},{}});
        return;
    }

    emit answer_request({user_request.thread_id,SQL_STATE::LOGIN_SUCCESS,{},{}});
}

void user_info_thread::find_users(const Request_struct& user_request){
    QString query_text{"SELECT login FROM user_info "
        " WHERE login LIKE '"+user_request.values.at(1)+"%' AND login != '"+user_request.values.at(0)+"' ;"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query in find_users!";

        emit logger_signal({TypeError::ERROR,"<user_info_thread>"
                ,"find_users","executing sql query fail: "+data_base_query->lastError().text()});

        emit answer_request({user_request.thread_id,SQL_STATE::USERS_NOT_FOUND,{},{}});
        return;
    }

    QVector<QString>users_vector;
    while(data_base_query->next()){
        users_vector.push_back(data_base_query->value(0).toString());
    }

    if(users_vector.empty()){
        emit answer_request({user_request.thread_id,SQL_STATE::USERS_NOT_FOUND,{},{}});
        return;
    }
    emit answer_request({user_request.thread_id,SQL_STATE::USERS_FOUND_SUCCESS,
                         std::move(users_vector),{}});
}

void user_info_thread::stop_work()noexcept{
    emit logger_signal({TypeError::INFO,"<user_info_thread>","stop_work",
                        "Stoped work of request answering cycle"});
    flag_of_work = false;
}

void user_info_thread::add_to_queue_slot(const Request_struct& user_request){
    if(user_request.values.empty()){
        emit logger_signal({TypeError::INFO,"<user_info_thread>"
                            ,"add_to_queue_slot", "Empty incoming data"});
        return;
    }
    requests_queue.push_back(user_request);
}

