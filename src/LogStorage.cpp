#include "../include/LogStorage.h"
#include <filesystem>
#include <trantor/utils/Logger.h>

std::vector<LogEntry> LogStorage::logs;
std::mutex LogStorage::mutex_;

void LogStorage::init()
{
    logs.clear();
}

void LogStorage::addLogEntry(const LogEntry &entry)
{
    std::lock_guard<std::mutex> lock(mutex_);
    logs.push_back(entry);
    saveLogToFile(entry);
}

void LogStorage::saveLogToFile(const LogEntry &entry)
{
    std::string fileName = entry.serviceName + ".json";
    std::ofstream file(fileName, std::ios::app);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    Json::Value jsonData;
    jsonData["level"] = entry.level;
    jsonData["message"] = entry.message;
    jsonData["timestamp"] = entry.timestamp.toFormattedString(false);
    jsonData["serviceName"] = entry.serviceName;

    Json::StreamWriterBuilder writer;
    std::string jsonString = Json::writeString(writer, jsonData);

    file << jsonString << std::endl;
    file.close();
}

std::vector<LogEntry> LogStorage::getLogsByService(const std::string &serviceName)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::string fileName = serviceName + ".json";
    if (!std::filesystem::exists(fileName))
    {
        return {};
    }

    std::ifstream file(fileName);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    std::vector<LogEntry> logs;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        try
        {
            Json::Value jsonLog;
            Json::CharReaderBuilder reader;
            std::string errs;

            std::istringstream lineStream(line);
            if (!parseFromStream(reader, lineStream, &jsonLog, &errs))
            {
                LOG_ERROR << "Failed to parse log entry: " << errs;
                continue;
            }

            LogEntry entry;
            entry.level = jsonLog["level"].asString();
            entry.message = jsonLog["message"].asString();
            entry.timestamp = trantor::Date::fromDbString(jsonLog["timestamp"].asString());
            entry.serviceName = jsonLog["serviceName"].asString();
            logs.push_back(entry);
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << "Failed to parse log entry: " << e.what();
        }
    }

    file.close();
    return logs;
}