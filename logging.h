#ifndef LOGGING_H
#define LOGGING_H

#include <QDebug>

#ifndef NDEBUG

#define LOG_INFO(str, ...) qDebug(str, ##__VA_ARGS__)

#define LOG_WARNING(str, ...) qDebug(str, ##__VA_ARGS__)

#endif

#define LOG_ERROR(str, ...) qDebug(str, ##__VA_ARGS__)

#endif // LOGGING_H
