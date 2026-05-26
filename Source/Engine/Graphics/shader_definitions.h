// shader_definitions.h
#pragma once

#include "Engine/Device/direct3d.h"
using namespace DirectX;

namespace ShaderDefinitions {
    // 頂点の種類
    enum class VertexType {
        Model,          // モデル用頂点
        SkinnedModel,   // スキンメッシュ用頂点
        Sprite,         // スプライト用頂点

        None,           // 頂点情報なし

        MAX,
    };
#pragma region 頂点構造体
    struct ModelVertex {
        XMFLOAT3 position;  // 頂点の位置
        XMFLOAT3 normal;    // 頂点の法線
        XMFLOAT3 tangent;   // 頂点の接線
        XMFLOAT3 binormal;  // 頂点の副接線

        XMFLOAT4 color;     // 頂点の色
        XMFLOAT2 texCoord;  // 頂点のテクスチャ座標
    };
    struct SkinnedModelVertex {
        XMFLOAT3 position;  // 頂点の位置
        XMFLOAT3 normal;    // 頂点の法線
        XMFLOAT3 tangent;   // 頂点の接線
        XMFLOAT3 binormal;  // 頂点の副接線

        XMFLOAT4 color;     // 頂点の色
        XMFLOAT2 texCoord;  // 頂点のテクスチャ座標

        XMUINT4  boneIndices; // ボーンのインデックス
        XMFLOAT4 boneWeights; // ボーンの重み
    };
    //struct UnlitVertex {
    //    XMFLOAT3 position;  // 頂点の位置
    //    XMFLOAT3 normal;    // 頂点の法線
    //    XMFLOAT4 color;     // 頂点の色
    //    XMFLOAT2 texCoord;  // 頂点のテクスチャ座標
    //};
    struct SpriteVertex {
        XMFLOAT3 position;  // 頂点の位置
        XMFLOAT4 color;     // 頂点の色
        XMFLOAT2 texCoord;  // 頂点のテクスチャ座標
    };
#pragma endregion
    
    // シェーダーベース
    enum class ShaderBase {
        None,
        Lit,        // ライト付きシェーダー
        SkinnedLit, // スキンメッシュ用ライト付きシェーダー
        Unlit,      // ライトなしシェーダー
        Ui,         // UIシェーダー
        FullScreen, // フルスクリーンクワッドシェーダー
        MAX
    };

    static const std::string SHADER_BASE_NAMES[static_cast<size_t>(ShaderBase::MAX)] = {
        "None",
        "Lit",
        "SkinnedLit",
        "Unlit",
        "Ui",
        "FullScreen",
    };

}
