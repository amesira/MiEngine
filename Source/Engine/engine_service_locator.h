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
    static TextureRepository* GetTextureRepository() {
        return s_engineInstance ? s_engineInstance->GetResourceManager().GetTextureRepository() : nullptr;
    }
    static MaterialRepository* GetMaterialRepository() {
        return s_engineInstance ? s_engineInstance->GetResourceManager().GetMaterialRepository() : nullptr;
    }
    static ShaderRepository* GetShaderRepository() {
        return s_engineInstance ? s_engineInstance->GetResourceManager().GetShaderRepository() : nullptr;
    }

    // ShaderManagerへのアクセス
    static ShaderManager* GetShaderManager() {
        return s_engineInstance ? &s_engineInstance->GetShaderManager() : nullptr;
    }
    static void BindShader(ShaderBase shaderBase) {
        if (s_engineInstance) {
            s_engineInstance->GetShaderManager().BindShader(shaderBase);
        }
    }
    static void BindShader(const std::string& shaderName) {
        if (s_engineInstance) {
            s_engineInstance->GetShaderManager().BindShader(shaderName);
        }
    }
    static void BindShader(const ShaderProgramResource* shaderProgram) {
        if (s_engineInstance) {
            s_engineInstance->GetShaderManager().BindShader(shaderProgram);
        }
    }
    static void UpdateTransformCB(const ShaderManager::TransformBuffer& transformData) {
        if (s_engineInstance) {
            s_engineInstance->GetShaderManager().BindTransformCB(transformData);
        }
    }
    static void UpdateCameraCB(const ShaderManager::CameraBuffer& cameraData) {
        if (s_engineInstance) {
            s_engineInstance->GetShaderManager().BindCameraCB(cameraData);
        }
    }
};

#endif // ENGINE_SERVICE_LOCATOR_H