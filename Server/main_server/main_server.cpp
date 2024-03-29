
#include "main_server.h"

main_server::main_server(QObject * parent) : QObject(parent)
{
    sql_answer=SQL_STATE::NONE;

    user_server = new QTcpServer(this);

    logger_api = new logger;

    data_base = new sql_engine(logger_api);


    if(!user_server->listen(QHostAddress::Any, 2323)){
        qDebug()<<" Server working is "<<user_server->isListening();
        logger_api->message_handler(logger::TypeError::FATAL,"<main_server>"
                                    ,"main_server",
                                    "main_server constructor launching fail");
        QCoreApplication::exit(1);
        return;
    }

    qDebug()<<" Server working is "<<user_server->isListening();
    logger_api->message_handler(logger::TypeError::INFO,"<main_server>","main_server","Server is started");

    //  New socket connection
    connect(user_server,SIGNAL(newConnection()),this,SLOT(new_connection_slot()));

    //  SQL connections
    connect(data_base,SIGNAL(stop_server_signal()),this,SLOT(stop_server_slot()));

    //  Registration connections
    connect(data_base,SIGNAL(user_already_exists_signal()),this,SLOT(registration_fail_slot()));
    connect(data_base,SIGNAL(registration_success_signal()),this,SLOT(registration_success_slot()));

    //  Login connections
    connect(data_base,SIGNAL(login_not_exists_signal()),this,SLOT(login_fail_login_slot()));
    connect(data_base,SIGNAL(login_incorrect_pass_signal()),this,SLOT(login_fail_pass_slot()));
    connect(data_base,SIGNAL(login_success_signal()),this,SLOT(login_success_slot()));

    //  Found users connections
    connect(data_base,SIGNAL(users_not_found_signal()),this,SLOT(users_not_found_slot()));
    connect(data_base,SIGNAL(list_of_logins_signal(std::vector<QString>)),
            this,SLOT(become_users_list_answer_slot(std::vector<QString>)));
    connect(data_base,SIGNAL(list_of_logins_signal(std::vector<QString>)),
            this,SLOT(users_found_success_slot()));

    //  Message history connections
    connect(data_base,SIGNAL(messeges_list_signal(std::vector<std::tuple<QString,QString,QString>>))
            ,this,SLOT(become_message_history_slot(std::vector<std::tuple<QString,QString,QString>>)));
}

main_server::~main_server(){
    logger_api->message_handler(logger::TypeError::INFO,"<main_server>","~main_server","server stoped");
}

void main_server::stop_server_slot(){
    this->~main_server();
}

void main_server::new_connection_slot(){
    QPointer<QTcpSocket> new_user_socket = user_server->nextPendingConnection();

    //  Socket connections
    connect(new_user_socket,SIGNAL(disconnected()),new_user_socket,SLOT(deleteLater()));
    connect(new_user_socket,SIGNAL(disconnected()),this,SLOT(socket_disconnect_slot()));
    connect(new_user_socket,SIGNAL(readyRead()),this,SLOT(ready_read_slot()));

    socket_list.insert(new_user_socket);

    qDebug()<<"New connected user: "<<new_user_socket->peerAddress();
    logger_api->message_handler(logger::TypeError::INFO,"<main_server>"
                                ,"new_connection_slot"
                                ,"new socket "+new_user_socket->peerAddress().toString());

}

void main_server::ready_read_slot(){
    QPointer<QTcpSocket>user_socket_data = qobject_cast<QTcpSocket*>(sender());

    if(!user_socket_data){
        qDebug()<<"Fail by reading data from socket!";
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>"
                                    ,"ready_read_slot"
                                    ,"error reading socket: "+socket_and_login_table[user_socket_data]);
        return;
    }

    QByteArray data = user_socket_data->readAll();

    QJsonDocument user_data {QJsonDocument::fromJson(data)};
    QJsonObject user_json_object{user_data.object()},
        input_data {user_json_object["data"].toObject()};

    if(user_json_object["type"]=="registration"){

        data_base->user_registration(input_data["login"].toString(),input_data["password"].toString());

        registration_answer_slot(user_socket_data,input_data["login"].toString());

        sql_answer=SQL_STATE::NONE;
    }
    else if(user_json_object["type"]=="login"){

        data_base->user_login(input_data["login"].toString(),input_data["password"].toString());

        login_answer_slot(user_socket_data,input_data["login"].toString());

        sql_answer=SQL_STATE::NONE;
    }
    else if(user_json_object["type"]=="users_list"){

        data_base->find_users(input_data["login"].toString(),input_data["login_part"].toString());

        logins_list_answer_slot(user_socket_data,input_data["login"].toString());

        sql_answer=SQL_STATE::NONE;
    }
    else if(user_json_object["type"]=="message"){

        QDateTime current = QDateTime::currentDateTime();

        data_base->insert_message(input_data["login_from"].toString(),input_data["login_to"].toString(),
               input_data["message"].toString(), current.toString("yyyy-MM-dd  HH:mm:ss"));

        if(login_and_socket_table.find(input_data["login_to"].toString())!=login_and_socket_table.end()){
            send_message_slot(input_data["login_from"].toString(),
                              input_data["login_to"].toString(),input_data["message"].toString());
        }

        sql_answer=SQL_STATE::NONE;

    }
    else if(user_json_object["type"]=="message_list"){

        data_base->become_message_history(input_data["login_from"].toString()
                ,input_data["login_to"].toString(),input_data["last_datetime"].toString());

        send_message_history_slot(user_socket_data);

        sql_answer=SQL_STATE::NONE;
    }else{
        logger_api->message_handler(logger::TypeError::WARNING,"<main_server>"
                                    ,"ready_read_slot"
                                    ,"another type of JSON: "+user_json_object["type"].toString());
    }
}

void main_server::send_message_history_slot(QTcpSocket * user_socket){
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

    if(!user_socket->write(json_byte_array)){
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>"
                                    ,"send_message_history_slot","sending answer fail");
    }
}

void main_server::become_message_history_slot(const std::vector<std::tuple<QString,QString,QString>>& data){
    if(message_story.empty()){
        message_story = std::move(data);
    }else{
        logger_api->message_handler(logger::TypeError::WARNING,"<main_server>"
                                    ,"become_message_history_slot"
                                    ,"message_story is not empty");
    }
}

void main_server::send_message_slot(const QString &login_from,const QString &login_to,const QString &message){
    QJsonObject message_to_client,message_data;
    message_to_client["type"]="message";

    message_data["login_from"]=login_from;
    message_data["login_to"]=login_to;
    message_data["message"]=message;

    message_to_client["data"]=message_data;

    QJsonDocument doc(message_to_client);
    QByteArray json_byte_array = doc.toJson();

    if(!login_and_socket_table[login_to]->write(json_byte_array)){
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>"
                                    ,"send_message_slot"
                                    ,"sending message from "+login_from+" to "+login_to+" fail");
    }
}

void main_server::registration_answer_slot(QTcpSocket* user_socket,const QString& login){
    QJsonObject answer_to_client,answer_data;

    answer_to_client["type"]="registration";

    switch(sql_answer){
    case SQL_STATE::REGISTRATION_SUCCESS:
        answer_data["answer"]="success";
        answer_data["problem"]="none";

        login_and_socket_table[login]=user_socket;
        socket_and_login_table[user_socket]=login;

        logger_api->message_handler(logger::TypeError::INFO,"<main_server>"
                                    ,"registration_answer_slot"
                                    ,"register OK: "+login);

        break;
    case SQL_STATE::REGISTRATION_FAIL:
        answer_data["answer"]="fail";
        answer_data["problem"]="User already exists!";
        break;
    }

    answer_to_client["data"]=answer_data;
    QJsonDocument doc(answer_to_client);

    QByteArray json_byte_array = doc.toJson();
    if(!user_socket->write(json_byte_array)){
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>"
                                    ,"registration_answer_slot"
                                    ,"sending register answer  to "+login+" fail");
    }
}

void main_server::login_answer_slot(QTcpSocket * user_socket,const QString& login){
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

        logger_api->message_handler(logger::TypeError::INFO,"<main_server>"
                                    ,"login_answer_slot"
                                    ,"login OK: "+login);

        login_and_socket_table[login]=user_socket;
        socket_and_login_table[user_socket]=login;

        break;
    }

    answer_to_client["data"]=answer_data;
    QJsonDocument doc(answer_to_client);

    QByteArray json_byte_array = doc.toJson();
    if(!user_socket->write(json_byte_array)){
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>"
                                    ,"login_answer_slot"
                                    ,"sending login answer to "+login+" fail");
    }
}

void main_server::become_users_list_answer_slot(const std::vector<QString>logins_list){
    if(list_of_users.empty()){
        list_of_users=std::move(logins_list);
    }else{
        logger_api->message_handler(logger::TypeError::WARNING,"<main_server>"
                                    ,"become_users_list_answer_slot"
                                    ,"list_of_users is not empty");
    }
}

void main_server::logins_list_answer_slot(QTcpSocket * user_socket,const QString& login){
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
    if(!user_socket->write(json_byte_array)){
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>"
                                    ,"logins_list_answer_slot"
                                    ,"sending answer to "+login+" fail");
    }
}

void main_server::socket_disconnect_slot(){
    QPointer<QTcpSocket>disconnected_socket = qobject_cast<QTcpSocket*>(sender());

    logger_api->message_handler(logger::TypeError::INFO,"<main_server>"
                                ,"socket_disconnect_slot"
                                ,"disconnected: "+socket_and_login_table[disconnected_socket]);

    login_and_socket_table.remove(socket_and_login_table[disconnected_socket]);
    socket_and_login_table.remove(disconnected_socket);

    socket_list.remove(disconnected_socket);
    qDebug()<<"User disconnected"<<disconnected_socket->peerAddress();
}

void main_server::registration_fail_slot()noexcept{
    sql_answer=SQL_STATE::REGISTRATION_FAIL;
}

void main_server::registration_success_slot()noexcept{
    sql_answer=SQL_STATE::REGISTRATION_SUCCESS;
}

void main_server::users_not_found_slot()noexcept{
    sql_answer=SQL_STATE::USERS_NOT_FOUND;
}

void main_server::users_found_success_slot()noexcept{
    sql_answer=SQL_STATE::USERS_FOUND_SUCCESS;
}

void main_server::login_success_slot()noexcept{
    sql_answer=SQL_STATE::LOGIN_SUCCESS;

}

void main_server::login_fail_login_slot()noexcept{
    sql_answer = SQL_STATE::LOGIN_FAIL_LOGIN;
}

void main_server::login_fail_pass_slot()noexcept{
    sql_answer=SQL_STATE::LOGIN_FAIL_PASSWORD;
}
