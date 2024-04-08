//  Class describes logger, that write
//  problems, warnings etc in log_file

#pragma once

#include <QFile>
#include <QDir>
#include <QThread>
#include <QQueue>
#include <QTextStream>
#include <QDateTime>
#include <QCoreApplication>
#include <QString>
#include <QPointer>

#include "request_types.h"

class logger:public QObject
{
    Q_OBJECT
private:

    bool flag_of_work;

    QQueue<Logger_message> messages_queue;

    QPointer<QFile>log_file;
    QString direction;

public:

    logger(QObject * parrent = 0);

    void clear_the_file();

public slots:

    void message_handler(const Logger_message& message);

    void stop_work()noexcept;
    void work();

signals:

    void finished();

};

