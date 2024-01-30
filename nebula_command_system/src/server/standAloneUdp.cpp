#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include "server/standAlone.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "standAloneUdpServer");
    ros::NodeHandle nh;
    ros::Publisher standAloneJoyPub = nh.advertise<geometry_msgs::Twist>("/UDP_cmd_vel",50);

    std::string UdpServerIp{};
    int port{};
    ros::param::get("/standAloneUdp_node/IP", UdpServerIp = "172.26.123.249");
    ros::param::get("/standAloneUdp_node/PORT", port = 6000);

    standAloneUdp udpServer{UdpServerIp, port};
    switch (udpServer.standAloneBindSocket())
    {
    case 0:  printf("UDP网络服务器创建成功\n"); break;
    case -1: printf("socket_fd创建失败\n"); return -1; 
    case -2: printf("bind()绑定失败\n"); return -2; 
    }

    while (ros::ok())
    {
       Json::Value root = udpServer.UdpRecvfore();
       if(root["code"].asBool() && !root["msg"].isNull() && root["msg"].isArray())
       {
        //"msg" : [ 右，下，左，上]；
        geometry_msgs::Twist joyMsg;
        joyMsg.linear.x = (stod(root["msg"][3].asString()) - stod(root["msg"][1].asString())) * 0.3;
        joyMsg.angular.z = (stod(root["msg"][2].asString()) - stod(root["msg"][0].asString())) * 0.6;
        standAloneJoyPub.publish(joyMsg);  
       }
       //usleep(10*1000);
    }
    return 0;
}