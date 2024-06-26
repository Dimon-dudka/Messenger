#include "widget_manager.h"

widget_manager::widget_manager(QStackedWidget *parrent):QStackedWidget(parrent)
{
    QPixmap pix(":images/logo.png");
    setWindowIcon(QIcon(pix));

    current_date_time=QDateTime::currentDateTime();

    logger_api = new logger;

    fail_type = WHAT_FAIL::NONE;
    login_status = LOGIN_STATUS::FAIL;

    local_DB= new sql_engine;

    login_menu_widget = new login_menu;
    fails_menu_widget = new fails_menu;
    registration_menu_widget = new registration_menu;
    messenger_menu_widget = new messenger_main_menu;
    find_users_widget = new find_users_menu;

    addWidget(registration_menu_widget);
    addWidget(fails_menu_widget);
    addWidget(login_menu_widget);
    addWidget(messenger_menu_widget);
    addWidget(find_users_widget);

    resize(400,200);

    //  Socket logic
    user_socket = new QTcpSocket(this);
    connect_to_host();

    connect(user_socket,SIGNAL(readyRead()),this,SLOT(on_socket_ready_read()));
    connect(user_socket,SIGNAL(connected()),this,SLOT(on_socket_connected()));
    connect(user_socket,SIGNAL(connected()),this,SLOT(choise_slot_after_problem_menu()));
    connect(user_socket,SIGNAL(disconnected()),this,SLOT(on_socket_disconnected()));
    connect(user_socket,SIGNAL(disconnected()),this,SLOT(set_current_fails_menu_slot()));
    connect(user_socket,SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
            this,SLOT(on_socket_display_error(QAbstractSocket::SocketError)));

    //  login_menu connections
    connect(login_menu_widget,SIGNAL(registration_button_signal())
            ,this,SLOT(set_current_registration_menu_slot()));
    connect(login_menu_widget,SIGNAL(login_button_signal(QString,QString,QString))
            ,this,SLOT(register_login_to_server_slot(QString,QString,QString)));

    connect(login_menu_widget,SIGNAL(login_frontend_problem_signal(QString)),
            this,SLOT(login_frontend_fail_slot()));
    connect(login_menu_widget,SIGNAL(login_frontend_problem_signal(QString)),
            fails_menu_widget,SLOT(open_fails_menu_slot(QString)));
    connect(login_menu_widget,SIGNAL(login_frontend_problem_signal(QString)),
            this,SLOT(set_current_fails_menu_slot()));

    //  network_fail_menu connections
    connect(fails_menu_widget,SIGNAL(try_again_signal()),this,SLOT(choise_slot_after_problem_menu()));

    //  registration menu connections
    connect(registration_menu_widget,SIGNAL(register_button_signal(QString,QString,QString))
            ,this,SLOT(register_login_to_server_slot(QString,QString,QString)));
    connect(registration_menu_widget,SIGNAL(back_button_signal()),this,SLOT(set_current_login_menu_slot()));
    connect(registration_menu_widget,SIGNAL(register_frontend_problem_signal(QString))
            ,this,SLOT(registration_frontend_fail_slot()));
    connect(registration_menu_widget,SIGNAL(register_frontend_problem_signal(QString))
            ,fails_menu_widget,SLOT(open_fails_menu_slot(QString)));
    connect(registration_menu_widget,SIGNAL(register_frontend_problem_signal(QString))
            ,this,SLOT(set_current_fails_menu_slot()));

    //  messenger main menu connections
    connect(messenger_menu_widget,SIGNAL(find_users_signal()),this,SLOT(set_current_find_users_menu_slot()));
    connect(messenger_menu_widget,SIGNAL(send_message_signal(QString,QString)),
            this,SLOT(send_message_slot(QString,QString)));
    connect(messenger_menu_widget,SIGNAL(become_message_history(QString,QString)),
            this,SLOT(send_message_history_request_slot(QString,QString)));

    //  find users menu connections
    connect(find_users_widget,SIGNAL(back_button_signal()),this,SLOT(set_current_messenger_main_slot()));
    connect(find_users_widget,SIGNAL(find_user_signal(QString)),this,SLOT(find_users_to_server_slot(QString)));

    connect(find_users_widget,SIGNAL(empty_line_edit_signal(QString)),
            this,SLOT(find_users_frontend_fail_slot()));
    connect(find_users_widget,SIGNAL(empty_line_edit_signal(QString)),
            fails_menu_widget,SLOT(open_fails_menu_slot(QString)));
    connect(find_users_widget,SIGNAL(empty_line_edit_signal(QString)),
            this,SLOT(set_current_fails_menu_slot()));

    connect(find_users_widget,SIGNAL(choise_is_done_signal(QString))
            ,messenger_menu_widget,SLOT(change_another_user_label_slot(QString)));
    connect(find_users_widget,SIGNAL(choise_is_done_signal(QString)),
            this,SLOT(set_current_messenger_main_slot()));
    connect(find_users_widget,SIGNAL(choise_is_done_signal(QString)),
            this,SLOT(proof_add_logins_hash(QString)));

    connect(find_users_widget,SIGNAL(print_basic_logins_signal()),this,SLOT(print_basic_users_slot()));

    //  DB connections
    connect(local_DB,SIGNAL(logins_vector_signal(std::vector<QString>)),
            find_users_widget,SLOT(update_user_list_tree(std::vector<QString>)));

    set_current_login_menu_slot();
}

void widget_manager::choise_slot_after_problem_menu()noexcept{
    switch(fail_type){
    case WHAT_FAIL::NONE:
    case WHAT_FAIL::NETWORK_FAIL:
        if(connect_to_host()){
            if(login_status==LOGIN_STATUS::OK){
                set_current_messenger_main_slot();
            }else{
                set_current_login_menu_slot();
            }
        }
        break;
    case WHAT_FAIL::REGISTRATION_FRONTEND_FAIL:
    case WHAT_FAIL::REGISTRATION_BACKEND_FAIL:
        set_current_registration_menu_slot();
        break;

    case WHAT_FAIL::LOGIN_BACKEND_FAIL:
    case WHAT_FAIL::LOGIN_FRONTEND_FAIL:
        set_current_login_menu_slot();
        break;
    case WHAT_FAIL::FIND_USERS_BACKEND_FAIL:
    case WHAT_FAIL::FIND_USERS_FRONTEND_FAIL:
        set_current_find_users_menu_slot();
        break;
    }
    fail_type=WHAT_FAIL::NONE;
}

void widget_manager::set_current_fails_menu_slot()noexcept{
    setCurrentWidget(fails_menu_widget);
}

void widget_manager::set_current_login_menu_slot()noexcept{
    setCurrentWidget(login_menu_widget);
}

void widget_manager::set_current_registration_menu_slot()noexcept{
    setCurrentWidget(registration_menu_widget);
}

void widget_manager::set_current_messenger_main_slot()noexcept{
    setCurrentWidget(messenger_menu_widget);
}

void widget_manager::set_current_find_users_menu_slot()noexcept{
    local_DB->select_logins_already_chatted_slot(user_login);
    setCurrentWidget(find_users_widget);
}

void widget_manager::registration_frontend_fail_slot()noexcept{
    fail_type = WHAT_FAIL::REGISTRATION_FRONTEND_FAIL;
}

void widget_manager::registration_backend_fail_slot()noexcept{
    fail_type = WHAT_FAIL::REGISTRATION_BACKEND_FAIL;
}

void widget_manager::login_backend_fail_slot()noexcept{
    fail_type = WHAT_FAIL::LOGIN_BACKEND_FAIL;
}

void widget_manager::login_frontend_fail_slot()noexcept{
    fail_type = WHAT_FAIL::LOGIN_FRONTEND_FAIL;
}

void widget_manager::find_users_backend_fail_slot()noexcept{
    fail_type = WHAT_FAIL::FIND_USERS_BACKEND_FAIL;
}

void widget_manager::find_users_frontend_fail_slot()noexcept{
    fail_type = WHAT_FAIL::FIND_USERS_FRONTEND_FAIL;
}

void widget_manager::on_socket_ready_read(){
    QPointer<QTcpSocket>server_answer_socket = qobject_cast<QTcpSocket*>(sender());

    if(!server_answer_socket){
        logger_api->message_handler(logger::TypeError::ERROR,"<widget_manager>"
                                    ,"on_socket_ready_read"
                                    ,"reading server answer fail!");

        return;
    }

    QByteArray json_data_byte_array = server_answer_socket->readAll();
    QJsonDocument json_doc_type_proof = QJsonDocument::fromJson(json_data_byte_array);

    QJsonObject json_type_message = json_doc_type_proof.object();
    QJsonObject message_info=json_type_message["data"].toObject();

    if(json_type_message["type"]=="registration"){
        if(message_info["answer"]=="success"){
            login_status=LOGIN_STATUS::OK;
            set_current_messenger_main_slot();
            return;
        }else{
            registration_backend_fail_slot();
            fails_menu_widget->open_fails_menu_slot(message_info["problem"].toString());
            set_current_fails_menu_slot();
        }
    }
    else if (json_type_message["type"]=="login"){
        if(message_info["answer"]=="success"){
            login_status=LOGIN_STATUS::OK;
            set_current_messenger_main_slot();
            return;
        }else{
            login_backend_fail_slot();
            fails_menu_widget->open_fails_menu_slot(message_info["problem"].toString());
            set_current_fails_menu_slot();
        }
    }
    else if(json_type_message["type"]=="users_list"){
        if(message_info["answer"]=="success"){
            std::vector<QString>logins_array_from_json;
            QJsonArray logins_json_array = message_info["users"].toArray();

            for(const auto &w:logins_json_array){
                logins_array_from_json.push_back(w.toString());
            }
            find_users_widget->update_user_list_tree(logins_array_from_json);
        }
        else{
            find_users_backend_fail_slot();
            fails_menu_widget->open_fails_menu_slot(message_info["problem"].toString());
            set_current_fails_menu_slot();
        }
    }
    else if(json_type_message["type"]=="message"){
        if(message_info["login_to"].toString()!=user_login){
            return;
        }
        messenger_menu_widget->push_new_message_slot(message_info["login_from"].toString(),message_info["message"].toString());
    }
    else if(json_type_message["type"]=="message_list"){
        std::vector<std::tuple<QString,QString,QString>>server_data;
        QJsonArray message_array = message_info["messages"].toArray();

        for(const auto&w:message_array){
           QJsonObject tmp= w.toObject();
           server_data.push_back({tmp["sender"].toString(),
                                  tmp["date_time"].toString()
                                 ,tmp["message"].toString()});
        }
        messenger_menu_widget->print_message_story_slot(std::move(server_data));
    }
    else{
        qDebug()<<"Fuck";
    }
}

void widget_manager::on_socket_connected()noexcept{
    fail_type = WHAT_FAIL::NONE;
}

void widget_manager::on_socket_disconnected()noexcept{
    fail_type = WHAT_FAIL::NETWORK_FAIL;
}

void widget_manager::on_socket_display_error(QAbstractSocket::SocketError socket_error)noexcept{
    fail_type = WHAT_FAIL::NETWORK_FAIL;
    fails_menu_widget->open_fails_menu_slot("Network fail!");
    set_current_fails_menu_slot();
}

bool widget_manager::connect_to_host(){
    if(user_socket->state()!=QAbstractSocket::ConnectedState){
        user_socket->connectToHost("127.0.0.1",2323);
    }
    if(user_socket->state()==QAbstractSocket::ConnectedState){
        fail_type=WHAT_FAIL::NONE;
        logger_api->message_handler(logger::TypeError::INFO,"<widget_manager>"
                                    ,"connect_to_host"
                                    ,"connected to host");
        return 1;
    }
    return 0;
}

void widget_manager::register_login_to_server_slot(const QString& login,
                                        const QString &password,const QString &type){
    QJsonObject message_to_server,data_object;

    if(type=="register"){
        message_to_server["type"]="registration";
    }else{
        message_to_server["type"]="login";
    }
    data_object["login"]=login;
    data_object["password"]=password;
    message_to_server["data"]=data_object;

    QJsonDocument doc(message_to_server);
    QByteArray jsonData = doc.toJson();

    user_login=std::move(login);
    user_password=std::move(password);

    if(!user_socket->write(jsonData)){
        logger_api->message_handler(logger::TypeError::ERROR,"<widget_manager>"
                                    ,"register_login_to_server_slot"
                                    ,"response to server sending fail!");
    }
}

void widget_manager::find_users_to_server_slot(const QString &part_of_login){
    QJsonObject message_to_server,data_object;

    message_to_server["type"]="users_list";
    data_object["login"]=user_login;
    data_object["login_part"]=part_of_login;
    message_to_server["data"]=data_object;

    QJsonDocument doc(message_to_server);
    QByteArray jsonData = doc.toJson();

    if(!user_socket->write(jsonData)){
        logger_api->message_handler(logger::TypeError::ERROR,"<widget_manager>"
                                    ,"find_users_to_server_slot"
                                    ,"response to server sending fail!");
    }
}

void widget_manager::send_message_slot(const QString &login_to,const QString &message){
    QJsonObject message_to_server,data_object;

    message_to_server["type"]="message";

    data_object["login_from"]=user_login;
    data_object["login_to"]=login_to;
    data_object["message"]=message;
    message_to_server["data"]=data_object;

    QJsonDocument doc(message_to_server);
    QByteArray jsonData = doc.toJson();

    if(!user_socket->write(jsonData)){
        logger_api->message_handler(logger::TypeError::ERROR,"<widget_manager>"
                                    ,"send_message_slot"
                                    ,"sending message fail!");
    }

    local_DB->update_frequency_slot(user_login,login_to);
}

void widget_manager::update_login_frequency_hash_slot(const std::vector<QString>& logins)noexcept{
    if(!login_frequency.empty())return;
    for(const auto&w:logins){
        login_frequency[w]=0;
    }
}

void widget_manager::proof_add_logins_hash(const QString & login_to){
    if(login_frequency.find(login_to)==login_frequency.end()){
        local_DB->insert_new_chat_login_slot(user_login,login_to);
        login_frequency[login_to]=0;
    }
}

void widget_manager::print_basic_users_slot(){
    local_DB->select_logins_already_chatted_slot(user_login);
}

void widget_manager::send_message_history_request_slot(const QString &last_date,const QString &login_to){
    QJsonObject message_to_server,data_object;

    message_to_server["type"]="message_list";

    data_object["login_from"]=user_login;
    data_object["login_to"]=login_to;
    data_object["last_datetime"]=last_date;
    message_to_server["data"]=data_object;

    QJsonDocument doc(message_to_server);
    QByteArray jsonData = doc.toJson();

    if(!user_socket->write(jsonData)){
        logger_api->message_handler(logger::TypeError::ERROR,"<widget_manager>"
                                    ,"send_message_history_request_slot"
                                    ,"sending response fail!");
    }
}
