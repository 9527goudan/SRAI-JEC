#include <ros/ros.h>
#include <std_msgs/String.h>
#include "srbot_err/Error.h"
#include "srbot_err/srbotError.hpp"
#include "srbot_err/State.h"


static Json::Value nodeMsg{};
static Json::Value stateMsg{};
static ros::Publisher msgErr;
static ros::Publisher msgState;


//订阅错误消息
void subSrbotErrorCall(const srbot_err::Error &err)
{
    nodeMsg["nodeName"] = err.nodeName;
    nodeMsg["msgLog"] = err.msgLog;
    nodeMsg["Error"] = err.Error;
    nodeMsg["ErrorMsg"] = err.ErrorMsg;
    fileWriteSwitch = true;
    if(err.Error)
    {
        Json::Value errorValue{};
        errorValue[err.nodeName] = err.ErrorMsg;
        Json::FastWriter writerToString;
        std_msgs::String errorMsg{};
        errorMsg.data = writerToString.write(errorValue);
        msgErr.publish(errorMsg);
        errorMsg.data.clear();
        //std::cout << "异常：" << errorMsg <<std::endl;
    }
}

//订阅状态信息
void subSrbotStateCall(const srbot_err::State &_State)
{
    Json::Value Data;
    if(_State.dataType)
        for (size_t i = 0; i < 4; i++)
            Data.append(_State.array[i]);
    else
        Data["State"] = _State.data;

    stateMsg[_State.sensingElement.c_str()] = Data;
    //std::cout << "状态：" << stateMsg << std::endl;

    Json::FastWriter writerToString;
    std_msgs::String StateMsg{};
    StateMsg.data = writerToString.write(stateMsg);
    msgState.publish(StateMsg);
    StateMsg.data.clear();
    //std::cout << "信息：" << StateMsg << std::endl;
}


int main(int argc, char **argv)
{
    ros::init(argc,argv,"srbotErrorNode");
    ros::NodeHandle nh_;

   // ros::Subscriber subRosOut = nh_.subscribe("/rosout", 100, );
    ros::Subscriber subSrbotError = nh_.subscribe("/srbotError", 10, &subSrbotErrorCall);//错误异常
    ros::Subscriber subSrbotState = nh_.subscribe("/srbotState", 10, &subSrbotStateCall);//状态信息

    msgErr = nh_.advertise<std_msgs::String>("/RobotStateError",10);
    msgState = nh_.advertise<std_msgs::String>("/RobotState",10);
    
    std::string logPath{};
    ros::param::get("/srbot_err_node/LogPath", logPath = "/home/tom/srbot_ws-C/src/srbot_err/log/SRBOT.log");
    logPath = logPath + getCalendar_Y_M_D() + ".log";
    srbotError srbot(logPath);
    int ret = srbot.fileLoge_init();
    if(-1 == ret)
    {
        std::cerr << "日志文件初始化失败" << std::endl;
        return -1;
    }
    std::cout << "日志文件初始化完成" << std::endl;

    ros::Rate Sleep(20);
    while (ros::ok())
    {
        ros::spinOnce();
        Sleep.sleep();
        if(fileWriteSwitch)
        {
            srbot.fileWrite(nodeMsg);
            fileWriteSwitch = false;
        }
    }
    return 0;
}