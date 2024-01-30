#ifndef _CLIENT_MQTT_
#define _CLIENT_MQTT_


#include <iostream>
#include <MQTTClient.h>
#include <string>
#include <ctime>
#include <string.h>

struct MqttContext
{
    int 	    payloadlen;           //MQTT 消息负载的长度（以字节为单位）。
    char * 	    payload;              //指向 MQTT 消息有效负载的指针。
    bool        topicUpdate;          //话题是否有更新 0：没有； 1：有 
    char *      topicName;            //话题名称  
};

class client_mqtt
{
private:
    /* data */
    MQTTClient mqttClient;
    std::string addRess{}, client_id{}, username{}, userword{};
    MQTTClient_deliveryToken _deliveredtoken;
    int rec;

public:
    client_mqtt();
    client_mqtt(std::string _addRess, std::string _client_id_prefix, std::string _username, std::string _userword);
    ~client_mqtt();

    void MqttPubMsg(const char* _topic, const std::string* _Msg);
    int mqttClient_init(MqttContext* Context_);
    void setMqttClientID(const std::string& _id_prefix);
    void setServerAdderAndUser(std::string _adder, std::string _name, std::string _word);
    int SubTopic(std::string _Topic, int _Qos = 1);
};

extern  MqttContext* _Context_;

#endif

