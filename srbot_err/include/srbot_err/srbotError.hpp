#ifndef _SRBOT_ERROR_
#define _SRBOT_ERROR_
#include <string>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <iostream>

static bool fileWriteSwitch = false;

class srbotError
{   
public:
    srbotError(const std::string pathFile_);
    ~srbotError();

    /*初始化日志文件*/
    int fileLoge_init();
    void fileWrite(Json::Value msgNode);

private:
    std::string pathFileLogo;
    std::ofstream logFile;
};


std::string getCalendar();
std::string getCalendar_Y_M_D();

#endif
