//===================================================
// resource_manager.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//===================================================
#include "resource_manager.h"

// リソースマネージャの初期化
void ResourceManager::Initialize() 
{
    m_modelRepository.Initialize();
}

// リソースマネージャの終了処理
void ResourceManager::Finalize() 
{
    m_modelRepository.Finalize();
}