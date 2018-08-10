#ifndef LOGGING_H
#define LOGGING_H

#include <chrono>
#include "string"
#include <ctime>
#include <stdio.h>

std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    return ltrim(rtrim(str, chars), chars);
}

class LoggerSingleton
{
public:
    static LoggerSingleton & instance()
    {
        static LoggerSingleton retVal;

        return retVal;
    }

    template<typename... Args>
    void writeToFile(std::string str, Args... args)
    {
        std::chrono::time_point<std::chrono::system_clock> p = std::chrono::system_clock::now();

        std::time_t today_time = std::chrono::system_clock::to_time_t(p);
        std::string strTimeDate = std::ctime(&today_time);

        //pFile = fopen(fileName.c_str(),"w+");

        //fseek(pFile, 0, SEEK_END);

        strTimeDate = trim(strTimeDate);

        std::string strWriteToFile = strTimeDate + " " + str + "\n";

        fprintf (pFile, strWriteToFile.c_str(), args...);

        //fclose (pFile);
    }
private:

    FILE * pFile;
    std::string fileName;

    LoggerSingleton()
    {
        std::chrono::time_point<std::chrono::system_clock> t;

        t = std::chrono::system_clock::now();

        std:: string strMinutes = std::to_string(std::chrono::duration_cast<std::chrono::minutes>(t.time_since_epoch()).count());

        fileName = "./" + strMinutes + ".txt";

        pFile = fopen(fileName.c_str(),"w");

        //fclose (pFile);
    }
    LoggerSingleton(LoggerSingleton const &);
    LoggerSingleton & operator=(LoggerSingleton const &);
    ~LoggerSingleton()
    {
        fclose (pFile);
    }
};

//qDebug(str, ##__VA_ARGS__);

#ifndef NDEBUG

#define LOG_INFO(str, ...){ \
    LoggerSingleton& ls = LoggerSingleton::instance(); \
    ls.writeToFile(str, ##__VA_ARGS__); \
}

#define LOG_WARNING(str, ...) { \
    LoggerSingleton& ls = LoggerSingleton::instance(); \
    ls.writeToFile(str, ##__VA_ARGS__); \
}
#endif

#define LOG_ERROR(str, ...) { \
    LoggerSingleton& ls = LoggerSingleton::instance(); \
    ls.writeToFile(str, ##__VA_ARGS__); \
}

#endif // LOGGING_H
