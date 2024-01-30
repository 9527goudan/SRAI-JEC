#ifndef _STANDALONE_
#define _STANDALONE_

#include<sys/socket.h>
#include<netinet/in.h>
#include<string>
#include<iostream>
#include<arpa/inet.h>
#include <jsoncpp/json/json.h>

class standAloneUdp
{
private:
    int socketFd, ClientNewFd;
    int portUdp;
    struct sockaddr_in server_addr{}, client_addr{}; 
    std::string serverUdpIp;


public:
    standAloneUdp(std::string _IP,int _PORT);
    ~standAloneUdp();
    int8_t standAloneBindSocket();
    Json::Value UdpRecvfore();
};

standAloneUdp::standAloneUdp(std::string _IP,int _PORT)
{
    serverUdpIp = _IP;
    portUdp = (uint16_t)_PORT;
}

standAloneUdp::~standAloneUdp()
{
    close(socketFd);
}


/*功能：创建UDP网络服务器
* 参数：无
* 返回值：0：创建成功；-1：socket_fd创建失败；-2：bind（）绑定失败。
*/
int8_t standAloneUdp::standAloneBindSocket()
{
    socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //SOCK_STREAM:流式套接字
    //int socketFd = socket(AF_INET, SOCK_DGRAM, 0); //SOCK_DGRAM:数据报套接字
    if(0 < socketFd)
    {
        //server_addr.sin_addr.s_addr = inet_addr(serverUdpIp.c_str());
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(portUdp);

        std::cout << "IP = " << serverUdpIp << '\n' << "port = " << portUdp << std::endl;

        int ret = bind(socketFd, (const sockaddr*)&server_addr, sizeof(server_addr));
        if(0 > ret)
        {
            printf("bind() 绑定失败  ret = %d\n",ret);
            close(socketFd);
            return -2;
        }
    }
    else
    {
         printf("UDP socket_fd 创建失败\n");
         return -1;
    }
    return 0;  
}

/*功能：Udp读取数据，并返回json对象
* 参数：
* 返回值：成功：readRoot["code"] = 1; 失败： readRoot["code"] = 0;
*/
Json::Value standAloneUdp::UdpRecvfore()
{
    Json::Value readRoot;
    
    timeval timeout = {3,0};//设置recvfrom超时时间：3秒，0微秒
    setsockopt(socketFd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));

    char *recvBuff = new char[2048]{};
    socklen_t len = sizeof(client_addr);
    int buffSize = recvfrom(socketFd, recvBuff, 2048, 0, (sockaddr *)&client_addr, &len);
    if(0 > buffSize)
    {
        //printf("数据接受失败 buffSize = %d\n",buffSize);
        readRoot["code"] = 0;
        return readRoot;
    }
    //std::cout << recvBuff << std::endl;
    Json::Reader readBuffToJosn;
   
    if(!readBuffToJosn.parse(recvBuff,readRoot))
    {
        printf("json格式解析失败\n");
        readRoot["code"] = 0;
    }
    delete [] recvBuff;

    return readRoot;
}



#endif