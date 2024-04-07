#pragma once

#include <QString>
#include <QVector>

enum class REQUEST_TYPES{
    REGISTER,
    LOGIN,
    USERS_LIST,
    INSERT_MESSAGE,
    MESSAGE_HISTORY,
};

enum class TypeError{
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

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

struct Request_struct{

    REQUEST_TYPES type;

    Qt::HANDLE thread_id;

    QVector<QString> values;

    Request_struct(REQUEST_TYPES type_from_user,Qt::HANDLE id,const QVector<QString> &arg)
        :type(type_from_user),thread_id(id),values(arg){}
};

struct Message_from_DB{
    QString sender,datetime,message;

    Message_from_DB(QString sender_from,QString datetime_from,QString message_from):
        sender(sender_from),datetime(datetime_from),message(message_from){}
};

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
