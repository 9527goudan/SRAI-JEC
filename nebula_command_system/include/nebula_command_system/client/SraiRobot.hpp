#ifndef _SRAIROBOT_
#define _SRAIROBOT_

#include <ros/ros.h>
#include "tf/transform_listener.h"
#include "tf/tf.h"
#include <jsoncpp/json/json.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <fstream>
#include <cmath>

#include <std_msgs/String.h>
#include "client/client_mqtt.hpp"
#include "msg/Battory.h"
#include <rosgraph_msgs/Log.h>
#include <std_msgs/Bool.h>


class SraiRobot
{
private:
    std::string SraiRobotID{}, RobotClass{}, pathFile{}, RobotError{};
    ros::NodeHandle robot_NH;
    ros::Subscriber subState, subError;
    ros::Publisher MqttPubTopic;
    client_mqtt Client;
    Json::Value mqttState{}, mqttError{};
    MqttContext* My_context_;
public:
    SraiRobot(MqttContext* My_Context_);
    ~SraiRobot();

    std::string getSraiRobotID();
    std::string getRobotClass();
    void robotId_UUID(); 
    void getRobotPosition();
    void ErrorCallback(const std_msgs::String &_Error); 
    void StateCallback(const std_msgs::String &_State);
    void PubMqttTopic();
      
};

std::string WeKeep_N_DecimalPlaces(double Decimal, std::size_t _N);



#endif