#include "arcguestserver.h"

ARCGuestServer::ARCGuestServer(EventLoop* loop, const InetAddress& listenAddr):
    server_(loop, listenAddr, "ARCCompareServer"),
    codec_(boost::bind(&ARCGuestServer::onStringMessage_add, this, _1, _2,_3 , _4) ,
           boost::bind(&ARCGuestServer::onStringMessage_query , this , _1,_2)),
    connections_(new ConnectionList)
{
    server_.setConnectionCallback(
                boost::bind(&ARCGuestServer::onConnection, this, _1));
    server_.setMessageCallback(
                boost::bind(&GuestCodec::onMessage, &codec_, _1, _2, _3));
}

void ARCGuestServer::start()
{
    server_.start();
}


void ARCGuestServer::onConnection(const TcpConnectionPtr &conn)
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

void ARCGuestServer::onStringMessage_add(const TcpConnectionPtr &conn, char *data, int len, int id)
{
    GuestPic guestPic;
    guestPic.id = id;
    guestPic.addTime = QTime::currentTime();
    guestPic.len_pic = len;
    guestPic.data = data;

    lock.lockForWrite();

    guestList.push_back(guestPic);
    lock.unlock();
    clearList();
    muduo::string messages("ok" , 2);
    conn->send(messages);

}

void ARCGuestServer::onStringMessage_query(const TcpConnectionPtr &conn, int id)
{
    lock.lockForRead();
    int flag = 0;
    for(list<GuestPic>::iterator it = guestList.begin() ; it != guestList.end() ; it++){
        if(id == it->id){
            flag++;
            int32_t total_len = it->len_pic+4;
            /*
             * send data of image
             * the head is the total length of data in TCP/IP
             * follow by the raw data of jpg image
            */
            conn->send((char*)&total_len , 4);
            conn->send(it->data , it->len_pic);
        }
    }
    if(flag == 0){
        conn->send((char*)"empty" , 5);
    }
    lock.unlock();
}

void ARCGuestServer::clearList()
{
    /*
     * clear outime Guest information
    */
    if(!lock.tryLockForWrite()) return;
    QTime curTime = QTime::currentTime();
    curTime =  curTime.addSecs(-RetainTime);
    list<GuestPic>::iterator it = guestList.begin();
    while((it->addTime)<= curTime && it!= guestList.end()){
        list<GuestPic>::iterator it_ = it;
        free(it_->data);
        it++;
        guestList.erase(it_);
    }
    lock.unlock();
}
