#include "messages_info_thread.h"

messages_info_thread::messages_info_thread(QObject * parrent):QObject(parrent) {
    flag_of_work = true;

    data_base_directory = QFileInfo(QCoreApplication::applicationFilePath()).path();

}

void messages_info_thread::open_data_base(){

    sql_connection = QSqlDatabase::addDatabase("QSQLITE",data_base_directory+"/messages_info.sqlite");
    sql_connection.setDatabaseName(data_base_directory+"/messages_info.sqlite");
    data_base_query = new QSqlQuery(sql_connection);
    sql_connection.setConnectOptions("QSQLITE_ENABLE_SHARED_CACHE");

    if(!sql_connection.open()){
        qDebug()<<"User messages DB connection FAIL!";

        emit logger_signal({TypeError::FATAL,"<messages_info_thread>","open_data_bases",
                            "unable to open messages_info database!"});
        emit stop_server_signal();
        emit finished();
        return;
    }

    data_base_query->clear();

    QString query_text{"CREATE TABLE IF NOT EXISTS messages("
                 "messege_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "sender TEXT,"
                 "getter TEXT,"
                 "date_time TEXT,"
                 "message TEXT"
                       ");"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"User messages DB open FAIL!";

        emit logger_signal({TypeError::FATAL,"<messages_info_thread>","open_data_bases",
                            "unable to create messages database: "+data_base_query->lastError().text()});
        emit stop_server_signal();
        emit finished();
        return;
    }
    emit logger_signal({TypeError::INFO,"<messages_info_thread>","open_data_bases",
                        "messages_info.sqlite is open"});
}

void messages_info_thread::work_slot(){

    open_data_base();

    while(flag_of_work){

        if(requests_queue.empty()){
            QThread::sleep(1);
            continue;
        }

        switch(requests_queue.front().type){
        case REQUEST_TYPES::INSERT_MESSAGE:
            insert_message(requests_queue.front());
            break;

        case REQUEST_TYPES::MESSAGE_HISTORY:
            become_message_history(requests_queue.front());
            break;
        }

        requests_queue.pop_front();
    }

    emit logger_signal({TypeError::INFO,"<messages_info_thread>"
                       ,"work_slot"
                        ,"Completion of the thread execution"});

    emit finished();
}

void messages_info_thread::become_message_history(const Request_struct& user_request){
    QString query_text{"SELECT sender, date_time, message FROM messages "
            "WHERE ((sender = '"+user_request.values.at(0)+"' AND getter = '"+user_request.values.at(1)+"' ) "
            "OR    (sender = '"+user_request.values.at(1)+"' AND getter = '"+user_request.values.at(0)+"' )) "
            "AND date_time < '"+user_request.values.at(2)+"' "
            "ORDER BY date_time DESC ;"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query in becoming message story!";

        emit answer_request({user_request.thread_id,SQL_STATE::CHAT_HISTORY_FAIL,{},{}});
        emit logger_signal({TypeError::ERROR,"<messages_info_thread>","become_message_history",
                            "executing sql query fail: "+data_base_query->lastError().text()});
        return;
    }

    QVector<Message_from_DB>data_from_sql;

    unsigned int count{0};

    while(data_base_query->next()&&count<20){
        data_from_sql.push_back({data_base_query->value(0).toString(),
                                data_base_query->value(1).toString(),
                                data_base_query->value(2).toString()});
        count+=1;
    }

    //if(data_from_sql.empty()){
    //    return;
    //}

    emit answer_request({user_request.thread_id,SQL_STATE::CHAT_HISTORY_SUCCESS,
                         {},std::move(data_from_sql)});
}

void messages_info_thread::insert_message(const Request_struct& user_request){
    QString query_text{"INSERT INTO messages ( sender , getter , date_time , message ) "
    " VALUES ('"+user_request.values.at(0)+"' , '"+user_request.values.at(1)
    +"' , '"+user_request.values.at(2)+"' , '"+user_request.values.at(3)+"');"};

    if(!data_base_query->exec(query_text)){
        qDebug()<<"Fail by executing sql query in insert_message!";

        emit answer_request({user_request.thread_id,SQL_STATE::MESSAGE_NOT_INSERTED,{},{}});

        emit logger_signal({TypeError::ERROR,"<messages_info_thread>","insert_message",
                            "executing sql query fail: "+data_base_query->lastError().text()});
        return;
    }

    emit answer_request({user_request.thread_id,SQL_STATE::MESSAGE_INSERTED,{},{}});
}

void messages_info_thread::stop_work()noexcept{
    emit logger_signal({TypeError::INFO,"<messages_info_thread>","stop_work",
                        "Stoped work of request answering cycle"});
    flag_of_work = false;
}

void messages_info_thread::add_to_queue_slot(const Request_struct& user_request){
    if(user_request.values.empty()){
        emit logger_signal({TypeError::INFO,"<messages_info_thread>"
                           ,"add_to_queue_slot","Empty incoming data"});
        return;
    }
    requests_queue.push_back(user_request);
}
