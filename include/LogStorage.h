#pragma once
#include "LogEntry.h"
#include <fstream>
#include <json/json.h>
#include <mutex>
#include <vector>

class LogStorage
{
public:
    static void init();
    static void addLogEntry(const LogEntry &entry);
    static std::vector<LogEntry> getLogsByService(const std::string &serviceName);

private:
    static std::vector<LogEntry> logs;
    static std::mutex mutex_;
    static void saveLogToFile(const LogEntry &entry);
};