/**
 * @file app_service.cpp
 * @author Forairaaaaa
 * @brief 常驻后台服务 App 的最小实现
 * @version 0.1
 * @date 2026-11-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <Arduino.h>
#include "app_service.h"
#include "mc_conf_internal.h"
#include "spdlog/spdlog.h"


using namespace MOONCAKE::APPS;


/* database key：其他 App 靠它拿到本服务对象指针（App 间通信） */
#define APP_SERVICE_DB_KEY  "BG_SERVICE"


void AppService::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal*>();

    // 关键：允许被 closeApp() 之后继续在后台跑
    setAllowBgRunning(true);

    // 把自己注册进 database，供其他 App 查找
    if (mcAppGetDatabase()->Add<AppService*>(APP_SERVICE_DB_KEY, this))
        spdlog::info("service registered to database");
    else
        spdlog::info("service key already exist");

    // 启动自己：会走 onResume -> onRunning 一次
    startApp();
}


void AppService::onResume()
{
    spdlog::info("{} onResume", getAppName());
}


void AppService::onRunning()
{
    // 只跑一次：请求关闭自己
    // 因为 setAllowBgRunning(true)，框架会走到 onPause() 后停在 ON_RUNNING_BG
    // 之后每帧回调的就是 onRunningBG()
    closeApp();
}


void AppService::onPause()
{
    spdlog::info("{} onPause -> 进入后台常驻", getAppName());
}


void AppService::onRunningBG()
{
    // 常驻服务的主场：每帧都被回调，在这里做周期任务
    if (millis() - _data.last_update > 500)
    {
        _data.last_update = millis();
        _data.tick++;

        spdlog::info("service tick {}", _data.tick);
    }
}


void AppService::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // 清理注册到 database 的 key
    mcAppGetDatabase()->Delete(APP_SERVICE_DB_KEY);
}
