#ifndef _CLIENT_TCP_
#define _CLIENT_TCP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <jsoncpp/json/json.h>
#include <unistd.h>
#include <ros/ros.h>
#include "srbot_err/Error.h"

class client_tcp
{
private:
    std::string TcpAddressIP{};
    int TcpAddressPort{}, socketfd{};
    struct sockaddr_in clientSockaddr_in;
    ros::NodeHandle TcpClient_NH;
    ros::Publisher Pub_read, Pub_errorOrLog;
    srbot_err::Error TcpError;

public:
    client_tcp(/* args */);
    ~client_tcp();
    void setSocketAddres(const std::string &_IP, const int &_Port);
    bool TcpSocketInit();
    bool clientTcpSend(const std::string sendBuff);//发送数据
    bool clientTcpRead();//接受数据
};

client_tcp::client_tcp(/* args */)
{
    TcpAddressIP = "127.0.0.1";
    TcpAddressPort = 8083;

    //Pub_read = TcpClient_NH.advertise<>("/TcpRead", 10);
    Pub_errorOrLog = TcpClient_NH.advertise<srbot_err::Error>("srbotError", 10);
    TcpError.nodeName = "Pose_converter";
}

client_tcp::~client_tcp()
{
    close(socketfd);
}

//设置ip及端口
void client_tcp::setSocketAddres(const std::string &_IP, const int &_Port)
{
    TcpAddressIP.clear();
    TcpAddressIP = _IP;
    TcpAddressPort = _Port;
}


//初始化socket并连接服务端
bool client_tcp::TcpSocketInit()
{
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > socketfd)
    {
        TcpError.msgLog = "socket 套接字初始化失败  socketfd=" + std::to_string(socketfd);
        TcpError.Error = 1;
        TcpError.ErrorMsg = "Pose converter failed to start. Reload";
        close(socketfd);
        return false;
    }
    else
    {
        clientSockaddr_in.sin_family = AF_INET;
        clientSockaddr_in.sin_port = htons(TcpAddressPort);
        clientSockaddr_in.sin_addr.s_addr = inet_addr(TcpAddressIP.c_str());
        
        int connect_fig = connect(socketfd, (struct sockaddr*)&clientSockaddr_in, sizeof(clientSockaddr_in));
        if(0 > connect_fig)
        {
            printf("err: connect()  socketfd=%d\n", socketfd);
            TcpError.msgLog = "err: connect()  socketfd=" + std::to_string(socketfd);
            TcpError.Error = 1;
            TcpError.ErrorMsg = "Failed to connect to the server and reconnect...";
            close(socketfd);
            return false;
        }
        else
        {
            printf("connect server: [ IP:%s  Port:%d ]", TcpAddressIP.c_str(), TcpAddressPort);
            TcpError.msgLog = "connect server[ IP:" + TcpAddressIP + " , Prot:" + std::to_string(TcpAddressPort) + " ]  OK";
            TcpError.Error = 0;
            TcpError.ErrorMsg = nullptr;
        }
    }
    Pub_errorOrLog.publish(TcpError);
    return true;
}


//发送数据
bool client_tcp::clientTcpSend(const std::string sendBuff)
{


}



//接受数据
bool client_tcp::clientTcpRead()
{

}


#endif