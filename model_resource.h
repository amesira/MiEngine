//---------------------------------------------------
// model_resource.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/19
//---------------------------------------------------
#ifndef MODEL_RESOURCE_H
#define MODEL_RESOURCE_H
#include <string>

#include "d3d11.h"
#include "DirectXMath.h"
using namespace DirectX;
#include "direct3d.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include "texture_resource.h"
#include "material_resource.h"

class aiScene;

// モデルのメッシュデータ
struct ModelMesh {
	std::string name;

    // 頂点バッファとインデックスバッファ
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	unsigned int numVertices;
	unsigned int numIndices;

    // マテリアルインデックス
	uint32_t materialIndex;
};

// ボーンのデータ
struct ModelBone {
	std::string name;
    unsigned int index;

    // ボーンのオフセット行列（モデル空間からボーン空間への変換行列）
	XMMATRIX offsetMatrix;
    XMMATRIX globalTransform;
    XMMATRIX finalTransform;
};

// アニメーションクリップのデータ
struct AnimationClip {
    std::string name;

    float duration;         // アニメーションの長さ（秒）
    float ticksPerSecond;   // 1秒あたりのティック数

    // ボーンごとのキーフレームデータ
    struct Keyframe {
        float time;           // キーフレームの時間（秒）
        XMFLOAT4 keyValue;    // キーフレームの値（位置、回転、スケールなど）
    };
    struct AnimationChannel {
        std::string boneName;            // 対象のボーン名
        unsigned int boneIndex;          // 対象のボーンインデックス

        std::vector<Keyframe> positionKeyframes;   // 位置のキーフレーム
        std::vector<Keyframe> rotationKeyframes;   // 回転のキーフレーム
        std::vector<Keyframe> scalingKeyframes;      // スケールのキーフレーム
    };
    std::vector<AnimationChannel> channels;
};

// モデルリソース
class ModelResource {
private: friend class ModelRepository;
    const aiScene* AiScene = nullptr;

public:
	std::string name;

    // 頂点の種類
    enum class VertexType {
        Lit,
        SkinnedLit
    } vertexType = VertexType::Lit;

    // メッシュのリスト
	std::vector<ModelMesh> meshes;

    // ボーンのリスト
    std::vector<ModelBone> bones;
    std::unordered_map<std::string, unsigned int> boneNameToIndex;

    // アニメーションクリップ
    std::vector<AnimationClip> animationClips;

    // モデル標準となるマテリアルリソースのリスト
    std::vector<MaterialResource*> materialResources;

};
#endif // MODEL_RESOURCE_H