//--------------------------------------------------------------------------------------------
//  Made by Dmitrii Dudin
//
//  Client of Messenger
//  It is client server app
//
//  Started on 28.12.2023
//  v 0.1 - 10.01.2024 Added register/login, choise an interlocutor, general error menu
//
//--------------------------------------------------------------------------------------------

#include "widget_manager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    widget_manager wgt_manager;
    wgt_manager.show();

    return a.exec();
}
