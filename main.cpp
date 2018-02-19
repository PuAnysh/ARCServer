#include <QCoreApplication>
#include <src/CompareServer/arccompareserver.h>
#include <muduo/net/EventLoop.h>
#include <arcfaceengines.h>
int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid();
    QCoreApplication a(argc, argv);
    EventLoop loop;
    uint16_t port = static_cast<uint16_t>(2018);
    InetAddress serverAddr(port);
    ARCCompareServer compareserver(&loop, serverAddr);
    compareserver.start();
    loop.loop();
    return a.exec();
}
