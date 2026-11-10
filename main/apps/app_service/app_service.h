/**
 * @file app_service.h
 * @author Forairaaaaa
 * @brief 常驻后台服务 App 的最小实现
 * @version 0.1
 * @date 2026-11-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <mooncake.h>
#include "../../hal/hal.h"

#include <cstdint>


namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief 常驻后台服务
         *
         * 生命周期约定：
         * - onCreate()     注入 HAL、允许后台运行、把自己注册进 database
         * - onRunning()    起来后立刻 closeApp() 自降级为后台（只跑一次）
         * - onPause()      被关闭的那一刻回调（此处就是降级为后台）
         * - onRunningBG()  之后每帧都在跑，在这里做周期任务（本例子是 500ms 心跳）
         * - onDestroy()    清理注册到 database 的 key
         */
        class AppService : public APP_BASE
        {
        public:
            /* 对外接口：其他 App 通过 database 拿到本对象指针后调用 */
            uint32_t getTick() { return _data.tick; }

        private:
            struct Data_t
            {
                HAL::Hal* hal = nullptr;
                uint32_t tick = 0;          // 心跳计数，每 500ms +1
                uint32_t last_update = 0;   // 上次心跳时间
            };
            Data_t _data;

        public:
            void onCreate() override;
            void onResume() override;
            void onRunning() override;
            void onRunningBG() override;
            void onPause() override;
            void onDestroy() override;
        };


        class AppService_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "BG Service"; }
            void* getAppIcon() override { return nullptr; }
            void* newApp() override { return new AppService; }
            void deleteApp(void* app) override { delete (AppService*)app; }
        };
    }
}
