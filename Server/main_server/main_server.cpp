
#include "main_server.h"

main_server::main_server(QObject * parent) : QObject(parent)
{
    sql_answer=SQL_STATE::NONE;

    user_server = new QTcpServer(this);

    data_base = new sql_engine;

    if(!user_server->listen(QHostAddress::Any, 2323)){
        qDebug()<<" Server working is "<<user_server->isListening();
        return;
    }

    qDebug()<<" Server working is "<<user_server->isListening();

    next_block_size=0;

    connect(user_server,SIGNAL(newConnection()),this,SLOT(new_connection_slot()));

    //  SQL Connections
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
}

void main_server::new_connection_slot(){
    QPointer<QTcpSocket> new_user_socket = user_server->nextPendingConnection();

    connect(new_user_socket,SIGNAL(disconnected()),new_user_socket,SLOT(deleteLater()));
    connect(new_user_socket,SIGNAL(readyRead()),this,SLOT(ready_read_slot()));

    qDebug()<<"New connected user: "<<new_user_socket->peerAddress();
}

void main_server::ready_read_slot(){
    QPointer<QTcpSocket>user_socket_data = qobject_cast<QTcpSocket*>(sender());

    if(!user_socket_data){
        qDebug()<<"Fail by reading data from socket!";
        return;
    }

    QByteArray data = user_socket_data->readAll();

    QJsonDocument user_data {QJsonDocument::fromJson(data)};
    QJsonObject user_json_object{user_data.object()};

    QJsonObject input_data {user_json_object["data"].toObject()};

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
}

void main_server::registration_fail_slot(){
    sql_answer=SQL_STATE::REGISTRATION_FAIL;
}

void main_server::registration_success_slot(){
    sql_answer=SQL_STATE::REGISTRATION_SUCCESS;
}


void main_server::registration_answer_slot(QTcpSocket* user_socket,const QString& login){
    QJsonObject answer_to_client,answer_data;

    answer_to_client["type"]="registration";

    switch(sql_answer){
    case SQL_STATE::REGISTRATION_SUCCESS:
        answer_data["answer"]="success";
        answer_data["problem"]="none";

        login_and_socket_table[login]=user_socket;

        break;
    case SQL_STATE::REGISTRATION_FAIL:
        answer_data["answer"]="fail";
        answer_data["problem"]="User already exists!";
        break;
    }

    answer_to_client["data"]=answer_data;
    QJsonDocument doc(answer_to_client);

    QByteArray json_byte_array = doc.toJson();
    user_socket->write(json_byte_array);
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

        login_and_socket_table[login]=user_socket;

        break;
    }

    answer_to_client["data"]=answer_data;
    QJsonDocument doc(answer_to_client);

    QByteArray json_byte_array = doc.toJson();
    user_socket->write(json_byte_array);
}

void main_server::become_users_list_answer_slot(const std::vector<QString>logins_list){
    if(list_of_users.empty()){
        list_of_users=std::move(logins_list);
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
    user_socket->write(json_byte_array);
}

void main_server::users_not_found_slot(){
    sql_answer=SQL_STATE::USERS_NOT_FOUND;
}

void main_server::users_found_success_slot(){
    sql_answer=SQL_STATE::USERS_FOUND_SUCCESS;
}

void main_server::login_success_slot(){
    sql_answer=SQL_STATE::LOGIN_SUCCESS;

}

void main_server::login_fail_login_slot(){
    sql_answer = SQL_STATE::LOGIN_FAIL_LOGIN;
}

void main_server::login_fail_pass_slot(){
    sql_answer=SQL_STATE::LOGIN_FAIL_PASSWORD;
}
