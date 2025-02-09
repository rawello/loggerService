#pragma once
#include "LogStorage.h"
#include <drogon/HttpController.h>

class LogController : public drogon::HttpController<LogController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(LogController::handleLog, "/log", drogon::Post);
    ADD_METHOD_TO(LogController::getLogs, "/logs", drogon::Get);
    METHOD_LIST_END

    void handleLog(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void getLogs(const drogon::HttpRequestPtr &req,
                 std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};