#include "../include/LogController.h"
#include <drogon/utils/Utilities.h>
#include <json/json.h>

using namespace drogon;

void LogController::handleLog(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback)
{
    if (req->method() != Post)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k405MethodNotAllowed);
        resp->setBody("Invalid request method");
        callback(resp);
        return;
    }

    try
    {
        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errs;

        std::istringstream bodyStream(std::string(req->getBody()));
        if (!parseFromStream(reader, bodyStream, &root, &errs))
        {
            throw std::runtime_error("Failed to parse JSON: " + errs);
        }

        LogEntry entry;
        entry.level = root["level"].asString();
        entry.message = root["message"].asString();
        entry.serviceName = root["serviceName"].asString();
        entry.timestamp = trantor::Date::now();

        LogStorage::addLogEntry(entry);

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k201Created);
        callback(resp);
    }
    catch (const std::exception &e)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Invalid request body");
        callback(resp);
    }
}

void LogController::getLogs(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback)
{
    if (req->method() != Get)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k405MethodNotAllowed);
        resp->setBody("Invalid request method");
        callback(resp);
        return;
    }

    auto serviceName = req->getParameter("service");
    if (serviceName.empty())
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k400BadRequest);
        resp->setBody("Service name is required");
        callback(resp);
        return;
    }

    auto logs = LogStorage::getLogsByService(serviceName);

    Json::Value jsonResponse(Json::arrayValue);
    for (const auto &entry : logs)
    {
        Json::Value logEntry;
        logEntry["level"] = entry.level;
        logEntry["message"] = entry.message;
        logEntry["timestamp"] = entry.timestamp.toFormattedString(false);
        logEntry["serviceName"] = entry.serviceName;
        jsonResponse.append(logEntry);
    }

    auto resp = HttpResponse::newHttpJsonResponse(jsonResponse);
    callback(resp);
}