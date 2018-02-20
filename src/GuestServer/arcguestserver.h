#ifndef ARCGUESTSERVER_H
#define ARCGUESTSERVER_H


#include <src/GuestServer/guestcodec.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/ThreadLocalSingleton.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <QTime>
#include <QReadWriteLock>

#include <bits/stdc++.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>



#include <set>
#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;
using namespace std;
class ARCGuestServer  : boost::noncopyable
{
public:
    ARCGuestServer(EventLoop* loop, const InetAddress& listenAddr);
    void setThreadNum(int numThreads);
    void start();

private:
    struct GuestPic{
        int len_pic;
        char* data;
        int id;
        QTime addTime;
    };
    typedef std::set<TcpConnectionPtr> ConnectionList;
    typedef boost::shared_ptr<ConnectionList> ConnectionListPtr;
    void onConnection(const TcpConnectionPtr& conn);
    void onStringMessage_add(const TcpConnectionPtr& conn,
                         char *data,
                         int len,
                         int id);
    void onStringMessage_query(const TcpConnectionPtr &conn ,
                             int id);
    void clearList();
    const int RetainTime = (30*60);
    list<GuestPic> guestList;
    TcpServer server_;
    GuestCodec codec_;
    MutexLock mutex_;
    QReadWriteLock lock;
    ConnectionListPtr connections_;
};

#endif // ARCGUESTSERVER_H
