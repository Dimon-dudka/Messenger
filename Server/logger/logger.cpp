#include "logger.h"

logger::logger(QObject * parrent):QObject(parrent)
{
    direction=QFileInfo(QCoreApplication::applicationFilePath()).path();
    log_file = new QFile(direction+"/log_file.log");
    log_file.data()->open(QFile::Append|QFile::Text);

    flag_of_work = 1;

}

void logger::work(){

    QTextStream out(log_file.data());

    while(flag_of_work){

        if(messages_queue.empty()){
            QThread::sleep(1);
            continue;
        }

        out<<"<";

        switch(messages_queue.front().type){
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
        out<<" "<<messages_queue.front().file<<" "<<messages_queue.front().whereAccident
            <<": "<<messages_queue.front().message<<"\n";
        out.flush();

        messages_queue.pop_front();
    }
}

void logger::stop_work() noexcept{
    flag_of_work = 0;
    emit finished();
}

void logger::message_handler(const Logger_message& message){

    if(message.file.size()==0||message.message.size()==0||message.whereAccident.size()==0){
        messages_queue.push_back({TypeError::WARNING,"<logger>","message_handler","empty data!"});
        return;
    }
    messages_queue.push_back(std::move(message));
}

void logger::clear_the_file(){
    log_file.data()->remove();
    log_file.data()->open(QFile::Append|QFile::Text);
}
