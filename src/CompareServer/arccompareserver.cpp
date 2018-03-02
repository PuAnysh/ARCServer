#include "arccompareserver.h"

ARCCompareServer::ARCCompareServer(EventLoop* loop, const InetAddress& listenAddr):
    server_(loop, listenAddr, "ARCCompareServer"),
    codec_(boost::bind(&ARCCompareServer::onStringMessage_comp, this, _1, _2,_3) ,
           boost::bind(&ARCCompareServer::onStringMessage_addu , this , _1,_2,_3,_4),
           boost::bind(&ARCCompareServer::onStringMessage_addg , this , _1,_2,_3,_4)),
    connections_(new ConnectionList)
{
    server_.setConnectionCallback(
                boost::bind(&ARCCompareServer::onConnection, this, _1));
    server_.setMessageCallback(
                boost::bind(&CompareCodec::onMessage, &codec_, _1, _2, _3));
    engines = nullptr;
    engines = new ARCFaceEngines();
    engines->startEngines();
}

void ARCCompareServer::setDatabase(ARCFaceEngines * _engines)
{
    this->engines = _engines;
}

void ARCCompareServer::setThreadNum(int numThreads)
{
    server_.setThreadNum(numThreads);
}

void ARCCompareServer::start()
{
    server_.start();
}

void ARCCompareServer::onConnection(const TcpConnectionPtr &conn)
{
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
             << conn->peerAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

    MutexLockGuard lock(mutex_);
    if (!connections_.unique())
    {
        connections_.reset(new ConnectionList(*connections_));
    }
    assert(connections_.unique());

    if (conn->connected())
    {
        connections_->insert(conn);
    }
    else
    {
        connections_->erase(conn);
    }
}

void ARCCompareServer::onStringMessage_comp(const TcpConnectionPtr& conn, char* data, int len)
{
    LOG_INFO<<"compare message";
    if(engines != nullptr){
        //LOG_INFO<<conn->peerAddress().toIp();
        std::string str;
        muduo::string ip = conn->peerAddress().toIp();
        for(int i = 0 ; i < ip.size() ; i++) str+=ip[i];
        if(engines->compareFace(data , len, str) == 0){
            muduo::string message("ok" , 2  );
            conn->send(message);
        }
        else{
            muduo::string message("no" , 2);
            conn->send(message);
        }
    }
    else{
        muduo::string message("no" , 2);
        conn->send(message);
        LOG_INFO<<"Unset Engines\r\n";
    }

    //codec_.send(conn , message);
}

void ARCCompareServer::onStringMessage_addu(const TcpConnectionPtr &conn, char* data, int len , int id)
{
    if(engines != nullptr){
        if(engines->addFeature(data , len , id) == 0){
            muduo::string message("ok" , 2);
            conn->send(message);
        }
        else{
            muduo::string message("no" , 2);
            conn->send(message);
        }
    }
    else{
        muduo::string message("no" , 2);
        conn->send(message);
        LOG_INFO<<"Unset Engines\r\n";
    }
}

void ARCCompareServer::onStringMessage_addg(const TcpConnectionPtr &conn, char *data, int len, int id)
{
    if(engines != nullptr){
        if(engines->addFeature_g(data , len , id) == 0){
            muduo::string message("ok" , 2);
            conn->send(message);
        }
        else{
            muduo::string message("no" , 2);
            conn->send(message);
        }
    }
    else{
        muduo::string message("no" , 2);
        conn->send(message);
        LOG_INFO<<"Unset Engines\r\n";
    }
}
