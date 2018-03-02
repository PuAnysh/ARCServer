#ifndef ARCCOMPARESERVER_H
#define ARCCOMPARESERVER_H

#include <src/CompareServer/comparecodec.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/ThreadLocalSingleton.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <arcfaceengines.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>



#include <set>
#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;
class ARCCompareServer : boost::noncopyable
{
public:
    ARCCompareServer(EventLoop* loop, const InetAddress& listenAddr);
    void setDatabase(ARCFaceEngines* _engines);
    void setThreadNum(int numThreads);
    void start();

private:
    typedef std::set<TcpConnectionPtr> ConnectionList;
    typedef boost::shared_ptr<ConnectionList> ConnectionListPtr;
    void onConnection(const TcpConnectionPtr& conn);
    void onStringMessage_comp(const TcpConnectionPtr& conn,
                         char *data,
                         int len);
    void onStringMessage_addu(const TcpConnectionPtr &conn ,
                             char *data,
                             int len,
                             int id);
    void onStringMessage_addg(const TcpConnectionPtr &conn ,
                             char *data,
                             int len,
                             int id);
    ARCFaceEngines* engines;
    TcpServer server_;
    CompareCodec codec_;
    MutexLock mutex_;
    ConnectionListPtr connections_;
};

#endif // ARCCOMPARESERVER_H
