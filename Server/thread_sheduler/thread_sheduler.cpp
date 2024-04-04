#include "thread_sheduler.h"

thread_sheduler::thread_sheduler(quintptr user_socket, const QByteArray& data,
        QHash<QString,quintptr> login_and_socket_table_from,
        QMap<quintptr,QString>socket_and_login_table_from) : thread_user_socket_desc(user_socket),
        login_and_socket_table(login_and_socket_table_from),socket_and_login_table(socket_and_login_table_from),
        user_request(std::move(data))
{
    thread_connection_sql = new sql_engine;

    sql_answer = SQL_STATE::NONE;

    //  SQL connections
    //connect(thread_connection_sql,SIGNAL(stop_server_signal()),this,SLOT(stop_server_slot()));

    //  Registration connections
    connect(thread_connection_sql,SIGNAL(user_already_exists_signal()),this,SLOT(registration_fail_slot()));
    connect(thread_connection_sql,SIGNAL(registration_success_signal()),this,SLOT(registration_success_slot()));

    //  Login connections
    connect(thread_connection_sql,SIGNAL(login_not_exists_signal()),this,SLOT(login_fail_login_slot()));
    connect(thread_connection_sql,SIGNAL(login_incorrect_pass_signal()),this,SLOT(login_fail_pass_slot()));
    connect(thread_connection_sql,SIGNAL(login_success_signal()),this,SLOT(login_success_slot()));

    //  Found users connections
    connect(thread_connection_sql,SIGNAL(users_not_found_signal()),this,SLOT(users_not_found_slot()));
    connect(thread_connection_sql,SIGNAL(list_of_logins_signal(std::vector<QString>)),
                this,SLOT(become_users_list_answer_slot(std::vector<QString>)));
    connect(thread_connection_sql,SIGNAL(list_of_logins_signal(std::vector<QString>)),
                this,SLOT(users_found_success_slot()));

    //  Message history connections
    connect(thread_connection_sql,SIGNAL(messeges_list_signal(std::vector<std::tuple<QString,QString,QString>>))
               ,this,SLOT(become_message_history_slot(std::vector<std::tuple<QString,QString,QString>>)));

}

void thread_sheduler::read_from_socket(){

    qDebug()<<"Fork thread: "<<QThread::currentThreadId();

    QJsonDocument user_data {QJsonDocument::fromJson(user_request)};
    QJsonObject user_json_object{user_data.object()},
        input_data {user_json_object["data"].toObject()};

    if(user_json_object["type"]=="registration"){

        thread_connection_sql->user_registration(input_data["login"].toString(),input_data["password"].toString());

        registration_answer_slot(input_data["login"].toString());

        sql_answer=SQL_STATE::NONE;
    }
    else if(user_json_object["type"]=="login"){

        thread_connection_sql->user_login(input_data["login"].toString(),input_data["password"].toString());

        login_answer_slot(input_data["login"].toString());

        sql_answer=SQL_STATE::NONE;
    }
    else if(user_json_object["type"]=="users_list"){

        thread_connection_sql->find_users(input_data["login"].toString(),input_data["login_part"].toString());

        logins_list_answer_slot(input_data["login"].toString());

        sql_answer=SQL_STATE::NONE;
    }
    else if(user_json_object["type"]=="message"){

        QDateTime current = QDateTime::currentDateTime();

        thread_connection_sql->insert_message(input_data["login_from"].toString(),input_data["login_to"].toString(),
                                  input_data["message"].toString(), current.toString("yyyy-MM-dd  HH:mm:ss"));

        if(login_and_socket_table.find(input_data["login_to"].toString())!=login_and_socket_table.end()){
            send_message_slot(input_data["login_from"].toString(),
                              input_data["login_to"].toString(),input_data["message"].toString());
        }

        sql_answer=SQL_STATE::NONE;

    }
    else if(user_json_object["type"]=="message_list"){

        thread_connection_sql->become_message_history(input_data["login_from"].toString()
                                          ,input_data["login_to"].toString(),input_data["last_datetime"].toString());

        send_message_history_slot();

        sql_answer=SQL_STATE::NONE;
    }else{
        qDebug()<<"empty data";

        emit logger_signal(logger::TypeError::WARNING,"<thread_sheduler>",
                           "read_from_socket","another type of JSON: "+user_json_object["type"].toString());
    }

    thread_connection_sql->deleteLater();

    emit finished();
}

void thread_sheduler::send_message_history_slot(){
    QJsonObject answer_to_client,answer_data;
    QJsonArray data_array;

    answer_to_client["type"]="message_list";

    for(const auto&w:message_story){
        QJsonObject message_object;
        message_object["sender"]=get<0>(w);
        message_object["date_time"]=get<1>(w);
        message_object["message"]=get<2>(w);

        data_array.append(message_object);
    }

    message_story.clear();

    answer_data["messages"]=data_array;
    answer_to_client["data"]=answer_data;

    QJsonDocument doc(answer_to_client);
    QByteArray json_byte_array = doc.toJson();

    emit answer_signal(std::move(thread_user_socket_desc),std::move(json_byte_array));
}

void thread_sheduler::become_message_history_slot(const std::vector<std::tuple<QString,QString,QString>>& data){
    if(message_story.empty()){
        message_story = std::move(data);
    }else{
        emit logger_signal(logger::TypeError::WARNING,"<thread_sheduler>",
                           "become_message_history_slot","message_story is not empty");
    }
}

void thread_sheduler::send_message_slot(const QString &login_from,const QString &login_to
                                        ,const QString &message){
    QJsonObject message_to_client,message_data;
    message_to_client["type"]="message";

    message_data["login_from"]=login_from;
    message_data["login_to"]=login_to;
    message_data["message"]=message;

    message_to_client["data"]=message_data;

    QJsonDocument doc(message_to_client);
    QByteArray json_byte_array = doc.toJson();

    emit answer_signal(std::move(thread_user_socket_desc),std::move(json_byte_array));
}
void thread_sheduler::registration_answer_slot(const QString& login){
    QJsonObject answer_to_client,answer_data;

    answer_to_client["type"]="registration";

    switch(sql_answer){
    case SQL_STATE::REGISTRATION_SUCCESS:
        answer_data["answer"]="success";
        answer_data["problem"]="none";

        login_and_socket_table[login]=thread_user_socket_desc;
        socket_and_login_table[thread_user_socket_desc]=login;

        emit logger_signal(logger::TypeError::INFO,"<thread_sheduler>",
                           "registration_answer_slot","register OK: "+login);

        break;
    case SQL_STATE::REGISTRATION_FAIL:
        answer_data["answer"]="fail";
        answer_data["problem"]="User already exists!";
        break;
    }

    answer_to_client["data"]=answer_data;
    QJsonDocument doc(answer_to_client);

    QByteArray json_byte_array = doc.toJson();

    emit answer_signal(std::move(thread_user_socket_desc),std::move(json_byte_array));
}

void thread_sheduler::login_answer_slot(const QString& login){
    QJsonObject answer_to_client,answer_data;

    answer_to_client["type"]="login";

    switch(sql_answer){
    case SQL_STATE::LOGIN_FAIL_LOGIN:
        answer_data["answer"]="fail";
        answer_data["problem"]="User does not exists!";
        break;
    case SQL_STATE::LOGIN_FAIL_PASSWORD:
        answer_data["answer"]="fail";
        answer_data["problem"]="Incorrect password!";
        break;
    case SQL_STATE::LOGIN_SUCCESS:
        answer_data["answer"]="success";
        answer_data["problem"]="none";

        emit logger_signal(logger::TypeError::INFO,"<thread_sheduler>",
                           "login_answer_slot","login OK: "+login);

        login_and_socket_table[login]=thread_user_socket_desc;
        socket_and_login_table[thread_user_socket_desc]=login;

        break;
    }

    answer_to_client["data"]=answer_data;
    QJsonDocument doc(answer_to_client);

    QByteArray json_byte_array = doc.toJson();

    emit answer_signal(std::move(thread_user_socket_desc),std::move(json_byte_array));
}

void thread_sheduler::become_users_list_answer_slot(const std::vector<QString>logins_list){
    if(list_of_users.empty()){
        list_of_users=std::move(logins_list);
    }else{
        emit logger_signal(logger::TypeError::WARNING,"<thread_sheduler>",
                           "become_users_list_answer_slot","list_of_users is not empty");
    }
}

void thread_sheduler::logins_list_answer_slot(const QString& login){
    QJsonObject answer_to_client,answer_data;
    QJsonArray array_of_logins;

    answer_to_client["type"]="users_list";

    switch(sql_answer){
    case SQL_STATE::USERS_FOUND_SUCCESS:
        answer_data["answer"]="success";

        for(const QString&w:list_of_users){
            array_of_logins.append(w);
        }
        list_of_users.clear();

        answer_data["users"] = array_of_logins;

        break;
    case SQL_STATE::USERS_NOT_FOUND:
        answer_data["answer"]="fail";
        answer_data["problem"]="No users found";
        break;
    }

    answer_to_client["data"]=answer_data;
    QJsonDocument doc(answer_to_client);

    QByteArray json_byte_array = doc.toJson();

    emit answer_signal(std::move(thread_user_socket_desc),std::move(json_byte_array));

}

void thread_sheduler::registration_fail_slot()noexcept{
    sql_answer=SQL_STATE::REGISTRATION_FAIL;
}

void thread_sheduler::registration_success_slot()noexcept{
    sql_answer=SQL_STATE::REGISTRATION_SUCCESS;
}

void thread_sheduler::users_not_found_slot()noexcept{
    sql_answer=SQL_STATE::USERS_NOT_FOUND;
}

void thread_sheduler::users_found_success_slot()noexcept{
    sql_answer=SQL_STATE::USERS_FOUND_SUCCESS;
}

void thread_sheduler::login_success_slot()noexcept{
    sql_answer=SQL_STATE::LOGIN_SUCCESS;
}

void thread_sheduler::login_fail_login_slot()noexcept{
    sql_answer = SQL_STATE::LOGIN_FAIL_LOGIN;
}

void thread_sheduler::login_fail_pass_slot()noexcept{
    sql_answer=SQL_STATE::LOGIN_FAIL_PASSWORD;
}
