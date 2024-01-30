#include "client/SraiRobot.hpp"
#include <thread>


int main(int argc, char *argv[])
{
    ros::init(argc,argv,"clientTcp");
    MqttContext MyContext;
    MyContext.topicUpdate =  false;
    MyContext.topicName = nullptr;
    MyContext.payloadlen = 0;
    MyContext.payload =  nullptr;
    MqttContext* _Context_ = &MyContext;

    SraiRobot robot(_Context_);
    std::thread MqttPubMsg(&SraiRobot::getRobotPosition, &robot);
    if (MqttPubMsg.joinable())
        MqttPubMsg.detach();

    std::thread MqttMsgToROS_Msg(&SraiRobot::PubMqttTopic, &robot);
    if (MqttMsgToROS_Msg.joinable())
        MqttMsgToROS_Msg.detach();
    
    //robot.PubMqttTopic();
    ros::spin();
    MqttPubMsg.join();
    MqttMsgToROS_Msg.join();
    return 0;
}