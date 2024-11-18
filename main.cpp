#include "tcpfilesender.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpFileSender w;
    w.setWindowTitle("send file(Git Mod)");
    w.show();
    return a.exec();
}
