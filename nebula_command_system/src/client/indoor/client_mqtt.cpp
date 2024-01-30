#include "client/client_mqtt.hpp"


/*
功能：移动底盘mqtt客户端类构造函数
param1：mqtt服务器地址
param2：mqtt客户端ID标识前缀
*/
client_mqtt::client_mqtt(std::string _addRess, std::string _client_id_prefix ,std::string _username, std::string _userword)
{
    addRess = _addRess;
    username = _username;
    userword = _userword;
    _deliveredtoken = 0;
    if(!_client_id_prefix.empty())
        client_id = _client_id_prefix;
        
    else
        client_id = "SRAI_";

    time_t newTime;
    time(&newTime);
    client_id = _client_id_prefix + "_" + std::to_string(newTime);

    std::cout << "client_id:" << client_id << std::endl;

    rec = MQTTCLIENT_SUCCESS;
}


//默认构造函数
client_mqtt::client_mqtt()
{
    _deliveredtoken = 0;
    username = userword = "srai_client";
    addRess = "mqtt://120.55.164.135:1883";
    time_t newTime;
    time(&newTime);
    client_id = "SRAI_" + std::to_string(newTime);
    rec = MQTTCLIENT_SUCCESS;
}

client_mqtt::~client_mqtt()
{
    MQTTClient_disconnect(mqttClient, 10000);
    MQTTClient_destroy(&mqttClient);
}


/**********   异步订阅  *********/
//连接时触发
void connlostSub(void *context, char * cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

//连接断开时触发
void deliveredSub(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    //_deliveredtoken = dt;
}
 

 //接收订阅消息时触发
int msgarrvdSub(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    MqttContext* _context =(MqttContext*) context;
    _context->topicName = topicName;
    _context->topicUpdate = true;
    _context->payloadlen = message->payloadlen;
    char * msg;
    memset(msg, 0, strlen(msg) + 1);
    strncat(msg, (char*)message->payload, message->payloadlen);
    _context->payload = msg;
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}


/*功能：初始化mqtt客户端链接，设置订阅话题回调函数
return：
*/
int client_mqtt::mqttClient_init(MqttContext* Context_)
{
    _deliveredtoken = 0;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;  //20秒中断
    conn_opts.cleansession = 1;        //连接恢复时恢复上一次会话
    conn_opts.username = username.c_str();
    conn_opts.password = userword.c_str();


    //创建mqtt_clientSub 的handle
    if((rec = MQTTClient_create(&mqttClient, addRess.c_str(), client_id.c_str(), MQTTCLIENT_PERSISTENCE_NONE, nullptr)) != MQTTCLIENT_SUCCESS)
    {
        printf("\nMQTTClient_create() 失败   rec: %d\n", rec);
        rec = EXIT_FAILURE;
        goto exit;
    } 

    //设置异步订阅回调函数
    if((rec = MQTTClient_setCallbacks(mqttClient, static_cast<void*>(Context_), connlostSub, msgarrvdSub, deliveredSub)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set Callbacks, return code %d\n", rec);
        rec = EXIT_FAILURE;
        goto exit;
    }

    //连接mqtt_server  sub
    if((rec = MQTTClient_connect(mqttClient, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set connect, return code %d\n", rec);
        rec = EXIT_FAILURE;
        goto exit;
    }

exit:
    return rec;
}

//设置mqtt_client_ID
void client_mqtt::setMqttClientID(const std::string& _id_prefix)
{
    client_id.clear();
    
    time_t newTime;
    time(&newTime);
    client_id = _id_prefix + "_" + std::to_string(newTime);
        
    printf("\nclient_id:%s\n",client_id.c_str());
}

void client_mqtt::setServerAdderAndUser(std::string _adder, std::string _name, std::string _word)
{
    addRess.clear();
    addRess = _adder;
    username.clear();
    username = _name;
    userword.clear();
    userword = _word;
}


//订阅话题
int client_mqtt::SubTopic(std::string _Topic, int _Qos)
{
    if ((rec = MQTTClient_subscribe(mqttClient, _Topic.c_str(), _Qos)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to MQTTClient_subscribe, return code %d\n", rec);
        rec = EXIT_FAILURE;
        return rec;
    }
    return rec;
}


void client_mqtt::MqttPubMsg(const char* _topic, const std::string* _Msg)
{
    MQTTClient_message _pubMsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken _toKen = 0;

    rec = MQTTClient_publish(mqttClient, _topic, _Msg->size(), _Msg->c_str(), 2, 0, &_toKen);
    if(MQTTCLIENT_SUCCESS != rec)
    {
        _deliveredtoken = _toKen;
        printf("\n%s: 发布失败!!  toKen: %d \n", _topic, _deliveredtoken);
    }
}
