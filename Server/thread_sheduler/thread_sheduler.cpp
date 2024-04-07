#include "thread_sheduler.h"

thread_sheduler::thread_sheduler(quintptr user_socket, const QByteArray& data,
        QHash<QString,quintptr> login_and_socket_table_from,
        QMap<quintptr,QString>socket_and_login_table_from) : thread_user_socket_desc(user_socket),
        login_and_socket_table(login_and_socket_table_from),socket_and_login_table(socket_and_login_table_from),
        user_request(std::move(data))
{
    sql_answer = SQL_STATE::NONE;
}

void thread_sheduler::read_from_socket(){

    qDebug()<<"Fork thread: "<<QThread::currentThreadId();

    QJsonDocument user_data {QJsonDocument::fromJson(user_request)};
    QJsonObject user_json_object{user_data.object()};

    input_data =user_json_object["data"].toObject();

    if(user_json_object["type"]=="registration"){

        emit request_to_sql_signal({REQUEST_TYPES::REGISTER,QThread::currentThreadId()
                                    ,{input_data["login"].toString(),input_data["password"].toString()}});

    }
    else if(user_json_object["type"]=="login"){

        emit request_to_sql_signal({REQUEST_TYPES::LOGIN,QThread::currentThreadId()
                                    ,{input_data["login"].toString(),input_data["password"].toString()}});

    }
    else if(user_json_object["type"]=="users_list"){

        emit request_to_sql_signal({REQUEST_TYPES::USERS_LIST,QThread::currentThreadId()
                                    ,{input_data["login"].toString(),input_data["login_part"].toString()}});

    }
    else if(user_json_object["type"]=="message"){

        QDateTime current = QDateTime::currentDateTime();

        emit request_to_sql_signal({REQUEST_TYPES::INSERT_MESSAGE,QThread::currentThreadId()
            ,{input_data["login_from"].toString(),input_data["login_to"].toString(),
              current.toString("yyyy-MM-dd  HH:mm:ss"),input_data["message"].toString()}});

    }
    else if(user_json_object["type"]=="message_list"){

        emit request_to_sql_signal({REQUEST_TYPES::MESSAGE_HISTORY,QThread::currentThreadId(),
            {input_data["login_from"].toString(),input_data["login_to"].toString(),input_data["last_datetime"].toString()}});

    }else{
        qDebug()<<"empty data";

        emit logger_signal(TypeError::WARNING,"<thread_sheduler>",
                           "read_from_socket","another type of JSON: "+user_json_object["type"].toString());
    }
}

void thread_sheduler::request_answer_slot(const Answer_to_thread& request){

    if(request.thread_id!=QThread::currentThreadId()){
        return;
    }

    sql_answer=request.sql_answer;

    switch(request.sql_answer){
    //  Login
    case SQL_STATE::LOGIN_SUCCESS:
    case SQL_STATE::LOGIN_FAIL_LOGIN:
    case SQL_STATE::LOGIN_FAIL_PASSWORD:

        login_answer_slot(input_data["login"].toString());

        break;
    //  Register
    case SQL_STATE::REGISTRATION_FAIL:
    case SQL_STATE::REGISTRATION_SUCCESS:

        registration_answer_slot(input_data["login"].toString());

        break;
    //  Find User List
    case SQL_STATE::USERS_NOT_FOUND:
    case SQL_STATE::USERS_FOUND_SUCCESS:

        become_users_list_answer_slot(std::move(request.logins_list));
        logins_list_answer_slot(input_data["login"].toString());

        break;
    //  Insert Message
    case SQL_STATE::MESSAGE_NOT_INSERTED:
        break;
    case SQL_STATE::MESSAGE_INSERTED:

        if(login_and_socket_table.find(input_data["login_to"].toString())!=login_and_socket_table.end()){
            send_message_slot(input_data["login_from"].toString(),
            input_data["login_to"].toString(),input_data["message"].toString());
        }
        break;
    //  Message history
    case SQL_STATE::CHAT_HISTORY_FAIL:
    case SQL_STATE::CHAT_HISTORY_SUCCESS:

        become_message_history_slot(std::move(request.message_history));
        send_message_history_slot();

        break;

    }
    emit finished();
}

void thread_sheduler::send_message_history_slot(){
    QJsonObject answer_to_client,answer_data;
    QJsonArray data_array;

    answer_to_client["type"]="message_list";

    for(const auto&w:message_story){
        QJsonObject message_object;
        message_object["sender"]=w.sender;
        message_object["date_time"]=w.datetime;
        message_object["message"]=w.message;

        data_array.append(message_object);
    }

    message_story.clear();

    answer_data["messages"]=data_array;
    answer_to_client["data"]=answer_data;

    QJsonDocument doc(answer_to_client);
    QByteArray json_byte_array = doc.toJson();

    emit answer_signal(std::move(thread_user_socket_desc),std::move(json_byte_array));
}

void thread_sheduler::become_message_history_slot(const QVector<Message_from_DB>& data){

    if(message_story.empty()){
        message_story = std::move(data);
    }else{
        emit logger_signal(TypeError::WARNING,"<thread_sheduler>",
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

        emit logger_signal(TypeError::INFO,"<thread_sheduler>",
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

        emit logger_signal(TypeError::INFO,"<thread_sheduler>",
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

void thread_sheduler::become_users_list_answer_slot(const QVector<QString>logins_list){

    if(list_of_users.empty()){
        list_of_users=std::move(logins_list);
    }else{
        emit logger_signal(TypeError::WARNING,"<thread_sheduler>",
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
