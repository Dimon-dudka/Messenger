#include "main_server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    main_server server_app;

    return a.exec();
}
