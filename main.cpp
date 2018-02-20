#include <QCoreApplication>
#include <src/CompareServer/arccompareserver.h>
#include <src/GuestServer/arcguestserver.h>
#include <muduo/net/EventLoop.h>
#include <arcfaceengines.h>
int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid();
    QCoreApplication a(argc, argv);
    EventLoop loop;
    /*
     * abort feature
    */
    uint16_t port = static_cast<uint16_t>(2018);
    InetAddress serverAddr(port);
    ARCCompareServer compareserver(&loop, serverAddr);
    compareserver.start();
    /*
     * abort Guest
    */
    uint16_t port_guest = static_cast<uint16_t>(2019);
    InetAddress serverAddr_Guest(port_guest);
    ARCGuestServer guestServer(&loop , serverAddr_Guest);
    guestServer.start();
    loop.loop();
    return a.exec();
}
