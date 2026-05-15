//---------------------------------------------------
// shader_repository.h
// 
// Author：Miu Kitamura
// Date  ：2026/05/15
//---------------------------------------------------
#ifndef SHADER_REPOSITORY_H
#define SHADER_REPOSITORY_H
#include "shader_resource.h"

#include <unordered_map>
#include <memory>

class ShaderRepository {
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // シェーダーリソースのキャッシュ
    std::unordered_map<std::string, std::unique_ptr<ShaderProgramResource>> m_shaderCache;

    // 頂点シェーダキャッシュ
    std::unordered_map<std::string, std::unique_ptr<VertexShaderResource>> m_vertexShaderCache;
    // ピクセルシェーダーキャッシュ
    std::unordered_map<std::string, std::unique_ptr<PixelShaderResource>> m_pixelShaderCache;
    // 定数バッファキャッシュ
    std::unordered_map<std::string, std::unique_ptr<ConstantBufferResource>> m_constantBufferCache;

    // 頂点バイナリデータ構造体
    struct VsBinaryData {
        unsigned char* vsBinaryPointer;  // バイナリデータへのポインタ
        std::streamsize	fileSize;        // バイナリデータのサイズ
    };

public:
    // 初期化
    void Initialize();
    // 終了
    void Finalize();

    // シェーダープログラムリソースの生成・取得
    ShaderProgramResource* GenerateShaderProgramResource(const ShaderProgramResource& shader);
    ShaderProgramResource* GetShaderProgramResource(const std::string& name) {
        auto it = m_shaderCache.find(name);
        if (it != m_shaderCache.end()) return it->second.get();
        return nullptr;
    }
    // シェーダープログラムリソースへの定数バッファの追加
    bool AddConstantBufferToShaderProgram(const std::string& shaderName, ConstantBufferResource* cbResource) {
        auto shaderIt = m_shaderCache.find(shaderName);
        if (shaderIt != m_shaderCache.end()) {
            shaderIt->second->constantBuffers.push_back(cbResource);
            return true;
        }
        return false;
    }

    // 頂点シェーダーリソースの生成・取得
    VertexShaderResource* GenerateVertexShaderResource(const std::string& filePath, VertexType vertexType);
    VertexShaderResource* GetVertexShaderResource(const std::string& filePath) {
        auto it = m_vertexShaderCache.find(filePath);
        if (it != m_vertexShaderCache.end()) return it->second.get();
        return nullptr;
    }
    // ピクセルシェーダーリソースの生成・取得
    PixelShaderResource* GeneratePixelShaderResource(const std::string& filePath);
    PixelShaderResource* GetPixelShaderResource(const std::string& filePath) {
        auto it = m_pixelShaderCache.find(filePath);
        if (it != m_pixelShaderCache.end()) return it->second.get();
        return nullptr;
    }

    // 定数バッファリソースの生成・取得
    ConstantBufferResource* GenerateConstantBufferResource(const std::string& name,
        UINT slot, UINT byteWidth,
        bool bindVs = true, bool bindPs = true, ConstantBufferUsage usage = ConstantBufferUsage::Dynamic);
    ConstantBufferResource* GetConstantBufferResource(const std::string& name) {
        auto it = m_constantBufferCache.find(name);
        if (it != m_constantBufferCache.end()) return it->second.get();
        return nullptr;
    }

private:
    // 頂点シェーダーの読み込み
    bool LoadVertexShader(ID3D11VertexShader** outVs, const std::string& filePath, VsBinaryData& vbData);
    // 頂点レイアウトの作成
    bool CreateInputLayout(ID3D11InputLayout** outInputLayout, VertexType vertexType, const VsBinaryData& vbData);
    // ピクセルシェーダーの読み込み
    bool LoadPixelShader(ID3D11PixelShader** outPs, const std::string& filePath);


};

#endif // SHADER_REPOSITORY_H