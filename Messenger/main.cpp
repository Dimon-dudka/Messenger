//--------------------------------------------------------------------------------------------
//  Made by Dmitrii Dudin
//
//  Client of Messenger
//  It is client server app
//
//  Started on 28.12.2023
//  v 0.1 - 10.01.2024 Added register/login, choise an interlocutor, general error menu
//  v 0.9 - 11.02.2024 Implemented main logic and basic functions
//  v 1.0 - 12.02.2024 Realese version
//--------------------------------------------------------------------------------------------

#include "widget_manager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    widget_manager wgt_manager;
    wgt_manager.show();

    return a.exec();
}
