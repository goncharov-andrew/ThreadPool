#ifndef LOGGING_H
#define LOGGING_H

#include <QDebug>
#include <chrono>

#include "fstream"
#include "string"
#include <ctime>

class LoggerSingleton
{
public:
    static LoggerSingleton & instance()
    {
        static LoggerSingleton retVal;

        return retVal;
    }

    void writeToFile(std::string str, ...)
    {
        std::chrono::time_point<std::chrono::system_clock> p = std::chrono::system_clock::now();

        std::time_t today_time = std::chrono::system_clock::to_time_t(p);
        std::string timeDate = std::ctime(&today_time);

        //std::string strLog = format
    }
private:
    LoggerSingleton()
    {
        std::chrono::time_point<std::chrono::system_clock> t;

        t = std::chrono::system_clock::now();

        fileName = std::to_string(std::chrono::duration_cast<std::chrono::minutes>(t.time_since_epoch()).count());

        std::ofstream fout("./" + fileName + ".txt");
    }
    LoggerSingleton(LoggerSingleton const &);
    LoggerSingleton & operator=(LoggerSingleton const &);
    std::string fileName;
};

#ifndef NDEBUG

#define LOG_INFO(str, ...){ \
    qDebug(str, ##__VA_ARGS__); \
    LoggerSingleton& l = LoggerSingleton::instance(); \
}

#define LOG_WARNING(str, ...) qDebug(str, ##__VA_ARGS__)

#endif

#define LOG_ERROR(str, ...) qDebug(str, ##__VA_ARGS__)

#endif // LOGGING_H
