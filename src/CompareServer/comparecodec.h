#ifndef COMPARECODEC_H
#define COMPARECODEC_H
#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Endian.h>
#include <muduo/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class CompareCodec
{
public:
    typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
                                  char* data,
                                  int len)> StringMessageCallback_comp;
    typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
                                  char* data,
                                  int len,
                                  int id)> StringMessageCallback_add;
    explicit CompareCodec(const StringMessageCallback_comp& cb_comp , const StringMessageCallback_add& cb_addf ,
                          const StringMessageCallback_add& cb_addu)
        : messageCallback_comp(cb_comp),
          messageCallback_addf(cb_addf),
          messageCallback_addu(cb_addu)
    {
    }

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime)
    {
        if(buf->readableBytes() > kHeaderLen){
            char* data;
            const void* header=  buf->peek();
            int32_t len = *static_cast<const int32_t*>(header); // SIGBUS
            if(len <= buf->readableBytes()){
                buf->retrieve(kHeaderLen);
                const void* flag_head = buf->peek();
                int32_t flag_be32 = *static_cast<const int32_t*>(flag_head); // SIGBUS
                const int32_t flag = muduo::net::sockets::networkToHost32(flag_be32);
                buf->retrieve(kHeaderLen);
                if( flag == 0x636f6d70 ){
                    /* comp
                     * get the raw data
                    */

                    int imglen = len - 2*kHeaderLen;
                    char* imghead = const_cast<char*>(buf->peek());
                    data = (char*)malloc(sizeof(char)*imglen);
                    memcpy(data , imghead , imglen);
                    buf->retrieve(imglen);

                    messageCallback_comp(conn , data , imglen);
                }
                else if(flag == 0x61646466){
                    /*addu
                     * add feature
                    */
                    /*
                     * get the id
                    */
                    LOG_INFO<<"ADDU";
                    const void* id_header=  buf->peek();
                    int32_t id_be32 = *static_cast<const int32_t*>(id_header);
                    buf->retrieve(kHeaderLen);
                    /*
                     * get the raw data
                    */
                    int imglen = len - 3*kHeaderLen;
                    char* imghead = const_cast<char*>(buf->peek());
                    data = (char*)malloc(sizeof(char)*imglen);
                    memcpy(data , imghead , imglen);
                    buf->retrieve(imglen);
                    messageCallback_addf(conn , data , imglen , id_be32);

                }
                else if(flag == 0x61646467){
                    /*
                     * add feature of guest
                    */
                    const void* id_header=  buf->peek();
                    int32_t id_be32 = *static_cast<const int32_t*>(id_header);
                    buf->retrieve(kHeaderLen);
                    /*
                     * get the raw data
                    */
                    int imglen = len - 3*kHeaderLen;
                    char* imghead = const_cast<char*>(buf->peek());
                    data = (char*)malloc(sizeof(char)*imglen);
                    memcpy(data , imghead , imglen);
                    buf->retrieve(imglen);
                    messageCallback_addu(conn , data , imglen , id_be32);
                }
            }
        }

    }
    void send(muduo::net::TcpConnection* conn,
              const muduo::StringPiece& message)
    {
        muduo::net::Buffer buf;
        buf.append(message.data(), message.size());
        int32_t len = static_cast<int32_t>(message.size());
        int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof be32);
        conn->send(&buf);
    }
private:
    int min(int x , int y){
        return x<y? x : y;
    }
    StringMessageCallback_comp messageCallback_comp;
    StringMessageCallback_add messageCallback_addf , messageCallback_addu;
    const static size_t kHeaderLen = sizeof(int32_t);
};

#endif // COMPARECODEC_H
