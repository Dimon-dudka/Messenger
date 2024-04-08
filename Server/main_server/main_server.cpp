
#include "main_server.h"

main_server::main_server(QObject * parent) : QObject(parent)
{
    is_server_stoped = 0;

    QPointer<QThread> logger_thr = new QThread;

    logger_api = new logger;

    logger_api->moveToThread(logger_thr);

    //  Thread-object connections
    connect(logger_thr,SIGNAL(started()),logger_api,SLOT(work()));
    connect(logger_thr,SIGNAL(finished()),logger_thr,SLOT(deleteLater()));

    connect(logger_api,SIGNAL(finished()),logger_thr,SLOT(quit()));
    connect(logger_api,SIGNAL(finished()),logger_api,SLOT(deleteLater()));

    logger_thr->start();

    //  Server part
    user_server = new QTcpServer(this);
    if(!user_server->listen(QHostAddress::Any, 2323)){
        qDebug()<<" Server working is "<<user_server->isListening();
        logger_api->message_handler({TypeError::FATAL,"<main_server>"
                                    ,"main_server",
                                     "main_server constructor launching fail"});
        QCoreApplication::exit(1);
        return;
    }

    qDebug()<<" Server working is "<<user_server->isListening();
    logger_api->message_handler({TypeError::INFO,"<main_server>","main_server","Server is started"});

    //  New socket connection
    connect(user_server,SIGNAL(newConnection()),this,SLOT(new_connection_slot()));

    sql_engine_api = new sql_engine;

    connect(sql_engine_api,SIGNAL(logger_signal(Logger_message))
            ,logger_api,SLOT(message_handler(Logger_message)));
    connect(sql_engine_api,SIGNAL(stop_server_signal()),this,SLOT(stop_server_slot()));

}

void main_server::stop_server_slot(){

    if(is_server_stoped){
        return;
    }
    is_server_stoped=1;

    emit stop_threads();

    for(auto [key,value]:desc_and_socket.asKeyValueRange()){
        value->disconnect();
    }

    desc_and_socket.clear();
    login_and_socket_table.clear();
    socket_and_login_table.clear();

    logger_api->message_handler({TypeError::INFO,"<main_server>","stop_server_slot","server stoped"});

    logger_api->stop_work();

    this->~main_server();
    QCoreApplication::exit(1);
}

void main_server::new_connection_slot(){
    QPointer<QTcpSocket> new_user_socket = user_server->nextPendingConnection();

    //  Socket connections
    connect(new_user_socket,SIGNAL(disconnected()),new_user_socket,SLOT(deleteLater()));
    connect(new_user_socket,SIGNAL(disconnected()),this,SLOT(socket_disconnect_slot()));
    connect(new_user_socket,SIGNAL(readyRead()),this,SLOT(ready_read_slot()));

    desc_and_socket[new_user_socket->socketDescriptor()]=new_user_socket;

    qDebug()<<"New connected user: "<<new_user_socket->peerAddress();
    logger_api->message_handler({TypeError::INFO,"<main_server>"
                                ,"new_connection_slot"
                                 ,"new socket "+new_user_socket->peerAddress().toString()});

}

void main_server::ready_read_slot(){

    if(!qobject_cast<QTcpSocket*>(sender())){
        qDebug()<<"Fail by reading data from socket!";
        logger_api->message_handler({TypeError::ERROR,"<main_server>"
            ,"ready_read_slot","error reading socket: "
            +socket_and_login_table[qobject_cast<QTcpSocket*>(sender())->socketDescriptor()]});
        return;
    }

    quintptr socket_desc{static_cast<quintptr>(qobject_cast<QTcpSocket*>(sender())->socketDescriptor())};

    QPointer<thread_sheduler> worker = new thread_sheduler
        (socket_desc,std::move(qobject_cast<QTcpSocket*>(sender())->readAll()),login_and_socket_table);

    QPointer<QThread> new_thread = new QThread;
    worker->moveToThread(new_thread.get());

    //  Thread-object connections
    connect(new_thread,SIGNAL(started()),worker,SLOT(read_from_socket()));
    connect(new_thread,SIGNAL(finished()),new_thread,SLOT(deleteLater()));

    connect(worker,SIGNAL(finished()),new_thread,SLOT(quit()));
    connect(worker,SIGNAL(finished()),worker,SLOT(deleteLater()));

    connect(this,SIGNAL(stop_threads()),worker,SLOT(stop_thread()));

    //  Object-SQL connections
    //  Send request connection
    connect(worker,SIGNAL(request_to_sql_signal(Request_struct))
            ,sql_engine_api,SLOT(get_user_request(Request_struct)));

    //Become request from sql connection
    connect(sql_engine_api,SIGNAL(answer_request(Answer_to_thread))
            ,worker,SLOT(request_answer_slot(Answer_to_thread)));

    connect(worker,SIGNAL(insert_login_desc_signal(QString,quintptr))
            ,this,SLOT(become_table_insert_slot(QString,quintptr)));

    //  Answer to client connections
    connect(worker,SIGNAL(answer_signal(quintptr,QByteArray)),this,SLOT(send_answer_slot(quintptr,QByteArray)));

    //  Logger signal
    connect(worker,SIGNAL(logger_signal(Logger_message))
            ,logger_api,SLOT(message_handler(Logger_message)));

    new_thread->start();

    return;
}

void main_server::send_answer_slot(const quintptr &user_desc,const QByteArray &user_answer){
    if(desc_and_socket.find(user_desc)==desc_and_socket.end()){
        qDebug()<<"No socket!";
    }

    if(!desc_and_socket[user_desc]->write(std::move(user_answer))){
        qDebug()<<"Answer not send";
        logger_api->message_handler({TypeError::ERROR,"<main_server>","send_answer_slot",
                                     "Answer to "+socket_and_login_table[user_desc]+" does not send"});
    }
}

void main_server::socket_disconnect_slot(){
    QPointer<QTcpSocket>disconnected_socket = qobject_cast<QTcpSocket*>(sender());

    logger_api->message_handler({TypeError::INFO,"<main_server>"
            ,"socket_disconnect_slot"
            ,"disconnected: "+socket_and_login_table[disconnected_socket->socketDescriptor()]});

    login_and_socket_table.remove(socket_and_login_table[disconnected_socket->socketDescriptor()]);
    socket_and_login_table.remove(disconnected_socket->socketDescriptor());

    desc_and_socket.remove(disconnected_socket->socketDescriptor());

    qDebug()<<"User disconnected"<<disconnected_socket->peerAddress();
}

void main_server::become_table_insert_slot(QString login,quintptr desc){
    login_and_socket_table[login]=desc;
    socket_and_login_table[desc]=login;
}
