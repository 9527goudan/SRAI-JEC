#include "client/SraiRobot.hpp"

//异常信息回调
void SraiRobot::ErrorCallback(const std_msgs::String &_Error)
{
    Json::Reader ErrorReader{};
    Json::Value ErrorValue{};
    Json::FastWriter ErrorToString{};
    std::string ErrorString{};

    if(ErrorReader.parse(_Error.data, ErrorValue))
    {
        mqttError["MSG"] = ErrorValue;
        ErrorString = ErrorToString.write(mqttError);
        std::cout << "Error:" << ErrorString <<std::endl;
        Client.MqttPubMsg("RobotStateError", &ErrorString);
    }
}


//状态检测
void SraiRobot::StateCallback(const std_msgs::String &_State)
{
    Json::Reader StateReader{};
    Json::Value StateValue{};
    Json::FastWriter StateToString{};
    std::string StateString = _State.data;
    if(StateReader.parse(StateString, StateValue))
    {
        mqttState["MSG"] = StateValue;
        StateString.clear();
        StateString = StateToString.write(mqttState);
        std::cout << "State:" << mqttState <<std::endl;
        Client.MqttPubMsg("RobotState", &StateString);
    }
}

void SraiRobot::PubMqttTopic()
{
    std_msgs::String MqttTopicMsgs{};
    while (ros::ok())
    {
        if(My_context_->topicUpdate)
        {
            MqttPubTopic = robot_NH.advertise<std_msgs::String>(My_context_->topicName, 10);
            MqttTopicMsgs.data = My_context_->payload;
            My_context_->topicUpdate  = false;
            MqttPubTopic.publish(MqttTopicMsgs);
        }
        usleep(200* 1000);
    }
}

SraiRobot::SraiRobot(MqttContext* My_Context_)
{
    std::string _serveraddres{}, userName{}, userWord{}, TopicPrefix{};
    ros::param::get("client_mqtt_node/FilePath", pathFile = "/home/tom/srbot_ws-C/src/nebula_command_system/param/robot_ID.json");
    ros::param::get("client_mqtt_node/MqttServerAddress", _serveraddres = "mqtt://120.55.164.135:1883");
    ros::param::get("client_mqtt_node/MqttUserName", userName = "srai_client");
    ros::param::get("client_mqtt_node/MqttUserword", userWord = "srai_client");
    ros::param::get("client_mqtt_node/RobotClass", RobotClass = "YSH-CIR01");
    ros::param::get("client_mqtt_node/MqttTopicPrefix", TopicPrefix = "MQTT");

    Client.setServerAdderAndUser(_serveraddres, userName, userWord);
    Client.mqttClient_init(My_Context_);
    My_context_ = My_Context_;
    TopicPrefix = TopicPrefix + "/#";
    Client.SubTopic(TopicPrefix);

    robotId_UUID();
    mqttError["robotID"] = mqttState["robotID"] = SraiRobotID;
    mqttError["robotClass"] = mqttState["robotClass"] = RobotClass;

    subError = robot_NH.subscribe("/RobotStateError",10,&SraiRobot::ErrorCallback,this);
    subState = robot_NH.subscribe("/RobotState", 10, &SraiRobot::StateCallback, this);
}

SraiRobot::~SraiRobot()
{
    Client.~client_mqtt();
}

//获取机器人uuid号
std::string SraiRobot::getSraiRobotID()
{
    std::string _SraiRobotID{};
    if(SraiRobotID.empty())
        _SraiRobotID = "NULL";
    else
        _SraiRobotID = SraiRobotID;
    return _SraiRobotID;
}   

//获取机器人型号
std::string SraiRobot::getRobotClass()
{
    std::string _RobotClass{};
    if(RobotClass.empty())
        _RobotClass = "NULL";
    else
        _RobotClass = RobotClass;
    return _RobotClass;
}

//生成发布机器人唯一ID 识别码
void SraiRobot::robotId_UUID()
{
    std::fstream RobotIDFile;
    Json::Value fileJson{};
    Json::Value MqttVale{};

    //读取文件获取ID
    RobotIDFile.open(pathFile, std::ios::in);
    if(RobotIDFile.is_open())
    {
        RobotIDFile >> fileJson;
        if(fileJson.isMember("RobotID") && !fileJson["RobotID"].isNull())
        {
            SraiRobotID = fileJson["RobotID"].asString();
            RobotClass = fileJson["RobotClass"].asString();
            ros::param::set("/RobotUUID", SraiRobotID);
            ros::param::set("/RobotClass", RobotClass);
        }
    }
    else
    {
        RobotIDFile.close();
        int8_t ii_ = 10;
        while(ii_)
        {
            RobotIDFile.open(pathFile, std::ios::out);
            if(RobotIDFile.is_open())
            {
                MqttVale.clear();
                MqttVale["Path"] = pathFile;
                MqttVale["RobotClass"] = fileJson["RobotClass"] = RobotClass;

                time_t nowtime;
                time(&nowtime);
                boost::uuids::uuid robotUuid = boost::uuids::random_generator()();
                MqttVale["RobotID"] = fileJson["RobotID"] = std::to_string(nowtime) + "-" + boost::uuids::to_string(robotUuid);
                RobotIDFile << MqttVale << std::endl;
                SraiRobotID = fileJson["RobotID"].asString();
                ros::param::set("/RobotUUID", SraiRobotID);
                ros::param::set("/RobotClass", RobotClass);
                ii_ = 0;
            }
            else
            {
                printf("err:创建失败,再次尝试\n");
                RobotIDFile.close();
                ii_--;
                continue;
            }
            RobotIDFile.close();
        }
    }
}

//保留小数点后n位  ---默认保留2位
std::string WeKeep_N_DecimalPlaces(double Decimal, std::size_t _N = 2)
{
    std::string Places_,Decimal_;
    Places_ = ".";
    Decimal_ = std::to_string(Decimal);
    std::size_t found = 0;
    if(0 == _N)
         found = Decimal_.find(Places_) - 1;
    else
        found = Decimal_.find(Places_) + 1;
    Decimal_ = Decimal_.substr(0,found+_N);
    return Decimal_;
}

//获取机器人位置(x,y)以及绕z轴转动角度，同时将信息发布到mqtt服务器
void SraiRobot::getRobotPosition()
{
    tf::TransformListener listener;
    tf::StampedTransform transform;
    Json::Value positionValue{};
    Json::FastWriter writerJsonToString;
    std::string PubBuff{};
    double resolution{}, origin_x{}, origin_y{}, origin_z{};
    
    while (ros::ok())
    {
        ros::param::get("", resolution = 0.00);
        ros::param::get("", origin_x = 0.00);
        ros::param::get("", origin_y = 0.00);
        ros::param::get("", origin_z = 0.00);
        PubBuff.clear();

        if(listener.waitForTransform("/map","/base_link",ros::Time(0),ros::Duration(0.2)))
        {
          try
          {
            listener.lookupTransform("/map", "/base_link",ros::Time(0), transform);
          }
          catch (tf::TransformException &ex)
          {
            ROS_ERROR("%s",ex.what());

            positionValue["x"] = (int8_t)0.0;
            positionValue["y"] = (int8_t)0.0;
            positionValue["w"] = (int8_t)0.0;
            mqttState["Position"] = positionValue;
            PubBuff=writerJsonToString.write(mqttState);
            Client.MqttPubMsg("RobotState", &PubBuff);
            positionValue.clear();
            continue;
          }
        }
        else
        {
          positionValue["x"] = (int8_t)0.0;
          positionValue["y"] = (int8_t)0.0;
          positionValue["w"] = (int8_t)0.0;
          mqttState["Position"] = positionValue;
          PubBuff=writerJsonToString.write(mqttState);
          Client.MqttPubMsg("RobotState", &PubBuff);
          positionValue.clear();
          usleep(800*1000);
          continue;
        }
       //获取偏移量（x,y）
        positionValue["x"]=WeKeep_N_DecimalPlaces((transform.getOrigin().x() - origin_x) / resolution);
        positionValue["y"]=WeKeep_N_DecimalPlaces((transform.getOrigin().y() - origin_y) / resolution);

       //通过四元数计算欧拉角RPY
        tf::Quaternion RQ2;
        geometry_msgs::Quaternion msg;
        msg.w = transform.getRotation().getW();
        msg.x = transform.getRotation().getX();
        msg.y = transform.getRotation().getY();
        msg.z = transform.getRotation().getZ();
        tf::quaternionMsgToTF(msg, RQ2);
        double roll{}, pitch{}, yaw{};
        tf::Matrix3x3(RQ2).getRPY(roll, pitch, yaw);//四元数转欧拉角；以世界坐标系为基坐标系
        yaw = yaw * 180 / M_PI;
        if(90.0 <= yaw && yaw <= 180.0)
            yaw = 360.0 - (yaw - 90.0);
        else
            yaw= -(yaw - 90.0);
        positionValue["w"] = WeKeep_N_DecimalPlaces(yaw);
        mqttState["Position"] = positionValue;

        PubBuff=writerJsonToString.write(mqttState);
        Client.MqttPubMsg("RobotState", &PubBuff);
        usleep(800*1000);
    }
    
}
