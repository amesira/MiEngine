//---------------------------------------------------
// engine_service_locator.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/29
//---------------------------------------------------
#ifndef ENGINE_SERVICE_LOCATOR_H
#define ENGINE_SERVICE_LOCATOR_H
#include "engine.h"

// エンジンのサービス（ResourceManagerなど）へのアクセスを提供するクラス
class EngineServiceLocator {
private: friend class MiEngine;
    // エンジンのインスタンスへのポインタ
    static inline MiEngine* s_engineInstance = nullptr;

public:
    // デバッグログの追加
    static void AddLogMessage(const std::string& message) {
        if (s_engineInstance) {
            s_engineInstance->AddLogMessage(message);
        }
    }

    // ResourceManagerへのアクセス
    static ResourceManager* GetResourceManager() {
        return s_engineInstance ? &s_engineInstance->GetResourceManager() : nullptr;
    }
    static ModelRepository* GetModelRepository() {
        return s_engineInstance ? s_engineInstance->GetResourceManager().GetModelRepository() : nullptr;
    }

};

#endif // ENGINE_SERVICE_LOCATOR_H