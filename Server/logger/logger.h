//  Class describes logger, that write
//  problems, warnings etc in log_file

#pragma once

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QCoreApplication>
#include <QString>
#include <QPointer>

class logger:public QObject
{
    Q_OBJECT
private:

    QPointer<QFile>log_file;
    QString direction;

public:

    enum class TypeError{
        INFO,
        WARNING,
        ERROR,
        FATAL,
    };

    logger(QObject * parrent = 0);

    void clear_the_file();

public slots:

    void message_handler(TypeError type,const QString &file,const QString &whereAccident,const QString &message);

};

