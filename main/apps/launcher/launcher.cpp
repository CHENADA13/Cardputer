/**
 * @file launcher.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-09-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "launcher.h"
#include "../../hal/hal.h"
#include "mc_conf_internal.h"
#include "spdlog/spdlog.h"
#include "../utils/theme/theme_define.h"
#include "../utils/anim/anim_define.h"


using namespace MOONCAKE::APPS;


void Launcher::onCreate()
{
    spdlog::info("{} onCreate", getAppName());
    
    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();

    // Init
    _boot_anim();
    _start_menu();
    _start_system_bar();
    _start_keyboard_bar();
    

    // Allow background running 
    setAllowBgRunning(true);

    // Auto start 
    startApp();
}


void Launcher::onRunning()
{
    _update_menu();
    _update_system_bar();
    _update_keyboard_bar();
}


void Launcher::onRunningBG()
{
    // spdlog::info("{}", mcAppGetFramework()->getAppManager().getCreatedAppNum());

    // Count created apps that are NOT allowed to run in background
    // （launcher 与常驻服务都允许后台运行，不计入，否则常驻服务会顶掉这个判断）
    int running_app_num = 0;
    for (const auto& lifecycle : *mcAppGetFramework()->getAppManager().getAppLifecycleList())
    {
        if (!lifecycle.app->isAllowBgRunning())
            running_app_num++;
    }

    // If no regular app standing in front
    if (running_app_num == 0)
    {
        spdlog::info("back to launcher");

        // Close anim 
        ANIM_APP_CLOSE();
        
        // Back to business
        mcAppGetFramework()->startApp(this);
    }

    _update_system_bar();
    _update_keyboard_bar();
}
