#include "logger.h"

logger::logger(QObject * parrent):QObject(parrent)
{
    direction=QFileInfo(QCoreApplication::applicationFilePath()).path();
    log_file = new QFile(direction+"/log_file.log");
    log_file.data()->open(QFile::Append|QFile::Text);

}

void logger::message_handler(TypeError type,const QString &file,const QString &whereAccident,const QString &message){
    QTextStream out(log_file.data());
    out<<"<";

    switch(type){
    case TypeError::INFO:
        out<<"INFO";
        break;
    case TypeError::WARNING:
        out<<"WARNING";
        break;
    case TypeError::ERROR:
        out<<"ERROR";
        break;
    case TypeError::FATAL:
        out<<"FATAL";
        break;
    }

    out<<"> "<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    out<<" "<<file<<" "<<whereAccident<<": "<<message<<"\n";
    out.flush();

}

void logger::clear_the_file(){
    log_file.data()->remove();
    log_file.data()->open(QFile::Append|QFile::Text);
}
