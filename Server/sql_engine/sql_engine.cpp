#include "sql_engine.h"

sql_engine::sql_engine(QObject *parrent):QObject(parrent)
{

    messages_DB = new messages_info_thread;
    user_info_DB = new user_info_thread;

    messages_thr = new QThread;
    user_info_thr = new QThread;

    messages_DB->moveToThread(messages_thr.data());
    user_info_DB->moveToThread(user_info_thr.data());

    //  Messages_DB connections
    connect(messages_thr,SIGNAL(started()),messages_DB,SLOT(work_slot()));
    connect(messages_thr,SIGNAL(finished()),messages_thr,SLOT(deleteLater()));

    connect(messages_DB,SIGNAL(finished()),messages_thr,SLOT(quit()));
    connect(messages_DB,SIGNAL(finished()),messages_DB,SLOT(deleteLater()));

    connect(messages_DB,SIGNAL(stop_server_signal()),this,SLOT(stop_server_slot()));

    //  User_info_DB connections
    connect(user_info_thr,SIGNAL(started()),user_info_DB,SLOT(work_slot()));
    connect(user_info_thr,SIGNAL(finished()),user_info_thr,SLOT(deleteLater()));

    connect(user_info_DB,SIGNAL(finished()),user_info_thr,SLOT(quit()));
    connect(user_info_DB,SIGNAL(finished()),user_info_DB,SLOT(deleteLater()));

    connect(user_info_DB,SIGNAL(stop_server_signal()),this,SLOT(stop_server_slot()));

    //  Answer connections
    connect(messages_DB,SIGNAL(answer_request(Answer_to_thread)),this,SLOT(become_result_and_send_slot(Answer_to_thread)));
    connect(user_info_DB,SIGNAL(answer_request(Answer_to_thread)),this,SLOT(become_result_and_send_slot(Answer_to_thread)));

    messages_thr->start();
    user_info_thr->start();
}

void sql_engine::get_user_request(const Request_struct& user_request){
    switch(user_request.type){
    case REQUEST_TYPES::LOGIN:
    case REQUEST_TYPES::REGISTER:
    case REQUEST_TYPES::USERS_LIST:
        user_info_DB.data()->add_to_queue_slot(user_request);
        break;

    case REQUEST_TYPES::INSERT_MESSAGE:
    case REQUEST_TYPES::MESSAGE_HISTORY:
        messages_DB.data()->add_to_queue_slot(user_request);
        break;
    }
}

void sql_engine::stop_server_slot(){
    emit stop_server_signal();
}

void sql_engine::become_logger_message_slot(const Logger_message& message){
    emit logger_signal(std::move(message));
}

void sql_engine::become_result_and_send_slot(const Answer_to_thread& user_answer){
    emit answer_request(std::move(user_answer));
}
