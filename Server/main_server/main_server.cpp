
#include "main_server.h"

main_server::main_server(QObject * parent) : QObject(parent)
{
    user_server = new QTcpServer(this);

    logger_api = new logger;

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
    //connect(data_base,SIGNAL(stop_server_signal()),this,SLOT(stop_server_slot()));
}

main_server::~main_server(){
    logger_api->message_handler(logger::TypeError::INFO,"<main_server>","~main_server","server stoped");
}

void main_server::stop_server_slot(){
    for(auto [key,value]:desc_and_socket.asKeyValueRange()){
        value->disconnect();
    }

    desc_and_socket.clear();
    login_and_socket_table.clear();
    socket_and_login_table.clear();

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
    logger_api->message_handler(logger::TypeError::INFO,"<main_server>"
                                ,"new_connection_slot"
                                ,"new socket "+new_user_socket->peerAddress().toString());

}

void main_server::ready_read_slot(){
    qDebug()<<"Main thread: "<<QThread::currentThreadId();

    if(!qobject_cast<QTcpSocket*>(sender())){
        qDebug()<<"Fail by reading data from socket!";
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>"
            ,"ready_read_slot"
            ,"error reading socket: "+socket_and_login_table[qobject_cast<QTcpSocket*>(sender())->socketDescriptor()]);
        return;
    }

    quintptr socket_desc{static_cast<quintptr>(qobject_cast<QTcpSocket*>(sender())->socketDescriptor())};

    QPointer<thread_sheduler> worker = new thread_sheduler
        (socket_desc,std::move(qobject_cast<QTcpSocket*>(sender())->readAll()),login_and_socket_table,socket_and_login_table);

    QPointer<QThread> new_thread = new QThread;
    worker->moveToThread(new_thread.get());

    connect(new_thread,SIGNAL(started()),worker,SLOT(read_from_socket()));
    connect(worker,SIGNAL(finished()),new_thread,SLOT(quit()));
    connect(worker,SIGNAL(finished()),worker,SLOT(deleteLater()));
    connect(new_thread,SIGNAL(finished()),new_thread,SLOT(deleteLater()));

    connect(worker,SIGNAL(answer_signal(quintptr,QByteArray)),this,SLOT(send_answer_slot(quintptr,QByteArray)));
    connect(worker,SIGNAL(logger_signal(logger::TypeError,QString,QString,QString))
            ,this,SLOT(write_into_logger_slot(logger::TypeError,QString,QString,QString)));

    new_thread->start();

    return;
}

void main_server::send_answer_slot(const quintptr &user_desc,const QByteArray &user_answer){
    if(!desc_and_socket[user_desc]->write(std::move(user_answer))){
        qDebug()<<"Answer not send";
        logger_api->message_handler(logger::TypeError::ERROR,"<main_server>","send_answer_slot",
                    "Answer to "+socket_and_login_table[user_desc]+" does not send");
    }
}

void main_server::write_into_logger_slot(const logger::TypeError& error_type,
                                         const QString &file,const QString &function_fail,const QString &what){
    logger_api->message_handler(error_type,file,function_fail,what);
}

void main_server::socket_disconnect_slot(){
    QPointer<QTcpSocket>disconnected_socket = qobject_cast<QTcpSocket*>(sender());

    logger_api->message_handler(logger::TypeError::INFO,"<main_server>"
                                ,"socket_disconnect_slot"
                                ,"disconnected: "+socket_and_login_table[disconnected_socket->socketDescriptor()]);

    login_and_socket_table.remove(socket_and_login_table[disconnected_socket->socketDescriptor()]);
    socket_and_login_table.remove(disconnected_socket->socketDescriptor());

    desc_and_socket.remove(disconnected_socket->socketDescriptor());

    qDebug()<<"User disconnected"<<disconnected_socket->peerAddress();
}
