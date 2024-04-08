//  Header file with all request types

#pragma once

#include <QString>
#include <QVector>

//  Request to sql_engine
enum class REQUEST_TYPES{
    REGISTER,
    LOGIN,
    USERS_LIST,
    INSERT_MESSAGE,
    MESSAGE_HISTORY,
};

//  Types for logger
enum class TypeError{
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

//  From sql_engine answer types
enum class SQL_STATE{
    NONE,
    REGISTRATION_SUCCESS,
    REGISTRATION_FAIL,
    LOGIN_SUCCESS,
    LOGIN_FAIL_LOGIN,
    LOGIN_FAIL_PASSWORD,
    USERS_NOT_FOUND,
    USERS_FOUND_SUCCESS,
    MESSAGE_INSERTED,
    MESSAGE_NOT_INSERTED,
    CHAT_HISTORY_SUCCESS,
    CHAT_HISTORY_FAIL,
};

//  Request structure to sql_engine
struct Request_struct{

    REQUEST_TYPES type;

    Qt::HANDLE thread_id;

    QVector<QString> values;

    Request_struct(REQUEST_TYPES type_from_user,Qt::HANDLE id,const QVector<QString> &arg)
        :type(type_from_user),thread_id(id),values(arg){}
};

//  Structure that provides message structure to message_history request
struct Message_from_DB{
    QString sender,datetime,message;

    Message_from_DB(QString sender_from,QString datetime_from,QString message_from):
        sender(sender_from),datetime(datetime_from),message(message_from){}
};

//  Standart answer type from sql_engine to thread
struct Answer_to_thread{
    Qt::HANDLE thread_id;

    SQL_STATE sql_answer;

    QVector<QString> logins_list;

    QVector<Message_from_DB> message_history;

    Answer_to_thread(Qt::HANDLE thread_id_from,SQL_STATE sql_answer_from
        ,QVector<QString> logins_list_from, QVector<Message_from_DB> message_history_from ):
        thread_id(thread_id_from),sql_answer(sql_answer_from)
        ,logins_list(logins_list_from),message_history(message_history_from){}

};

struct Logger_message{
    TypeError type;

    QString file, whereAccident, message;

    Logger_message(TypeError type_from,const QString &file_from,
                   const QString &whereAccident_from,const QString &message_from):
        type(type_from),file(file_from),whereAccident(whereAccident_from),message(message_from){}
};
