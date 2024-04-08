//--------------------------------------------------------------------------------------------
//  Made by Dmitrii Dudin
//
//  Server app
//
//  Started on 28.12.2023
//  v 0.1 - 10.01.2024
//  v 0.9 - 11.02.2024
//  v 1.0 - 12.02.2024
//  v 1.1 - 08.04.2024
//--------------------------------------------------------------------------------------------

#include "main_server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    main_server server_app;

    return a.exec();
}
