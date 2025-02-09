#include "../include/LogController.h"
#include "../include/LogStorage.h"
#include <drogon/drogon.h>

int main()
{
    LogStorage::init();

    LOG_INFO << "Logger service started on port 8080";
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .run();

    return 0;
}