#ifndef LOGGING_H
#define LOGGING_H

#include <chrono>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <atomic>

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
        while (m_Locked.test_and_set()) {}

        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);

        std::stringstream buffer;

        buffer << std::put_time(&tm, "%H:%M:%S %Y");

        std::string strTimeDate = buffer.str();

        std::string strWriteToFile = strTimeDate + " " + str + "\n";

        fprintf (pFile, strWriteToFile.c_str(), args...);

        m_Locked.clear();
    }
private:

    FILE* pFile;
    std::string fileName;
    std::atomic_flag m_Locked = ATOMIC_FLAG_INIT;

    LoggerSingleton()
    {
        std::chrono::time_point<std::chrono::system_clock> timeNow;

        timeNow = std::chrono::system_clock::now();

        std:: string strMinutes = std::to_string(std::chrono::duration_cast<std::chrono::minutes>(timeNow.time_since_epoch()).count());

        fileName = "./" + strMinutes + ".txt";

        pFile = fopen(fileName.c_str(),"w");
    }
    LoggerSingleton(LoggerSingleton const &);
    LoggerSingleton & operator=(LoggerSingleton const &);
    ~LoggerSingleton()
    {
        fclose (pFile);
    }
};

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
