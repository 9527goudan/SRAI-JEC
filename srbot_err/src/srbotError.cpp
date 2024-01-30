#include "srbot_err/srbotError.hpp"
#include <ctime>


srbotError::srbotError(const std::string pathFile_)
{
    pathFileLogo = pathFile_;
}

srbotError::~srbotError()
{
    if(logFile.is_open())
        logFile.close();
}

/*分割上一次启动记录的内容*/
int srbotError::fileLoge_init()
{
    int ret = 0;
    logFile.open(pathFileLogo, std::ios::app);
    if(logFile.is_open())
        logFile << "\n---------------- " + getCalendar() + " ----------------\n";
    else
    {
        std::cout << "日志文件打开失败" << std::endl;
        ret = -1;
    }
    logFile.close();
    return ret;
}


/*获取当前时间[年/月/日：时-分-秒]*/
std::string getCalendar()
{
    time_t timeNew;
    time(&timeNew);
    tm *pm = localtime(&timeNew);
    std::string calendar = std::to_string(pm->tm_year + 1900) + "-" + std::to_string(pm->tm_mon + 1) + "-" + 
                           std::to_string(pm->tm_mday) + " / " + std::to_string(pm->tm_hour) + ":" + std::to_string(pm->tm_min) + 
                           ":" + std::to_string(pm->tm_sec);
                           
    return calendar;
}

/*获取当前时间[年/月/日]*/
std::string getCalendar_Y_M_D()
{
    time_t timeNew;
    time(&timeNew);
    tm *pm = localtime(&timeNew);
    std::string calendar = std::to_string(pm->tm_year + 1900) + "-" + std::to_string(pm->tm_mon + 1) + "-" + 
                           std::to_string(pm->tm_mday);
    return calendar;
}

/*将日志消息写入日志文件中*/
void srbotError::fileWrite(Json::Value msgNode)
{
    logFile.clear();
    time_t timeNew;
    time(&timeNew);
    tm *pm = localtime(&timeNew);
    msgNode["time"] = std::to_string(pm->tm_hour) + ":" + std::to_string(pm->tm_min) + ":" + std::to_string(pm->tm_sec);
    std::cout << msgNode;

    logFile.open(pathFileLogo, std::ios::app);
    if(logFile.is_open())
        logFile << msgNode << '\n';
    else
        std::cerr << "日志写入失败" << std::endl;

    logFile.close();
}