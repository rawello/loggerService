#pragma once
#include <string>
#include <trantor/utils/Date.h>

struct LogEntry
{
    std::string level;
    std::string message;
    trantor::Date timestamp;
    std::string serviceName;
};