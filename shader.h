/*==============================================================================

   シェーダー [shader.h]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SHADER_H
#define	SHADER_H

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

/// @brief 頂点構造体
struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;		// 頂点カラー（R,G,B,A）
	XMFLOAT2 texCoord;	// テクスチャ座標
};
struct InstanceData
{
	XMFLOAT4 world0;
    XMFLOAT4 world1;
    XMFLOAT4 world2;
    XMFLOAT4 world3; // ワールド行列(4行分)
	XMFLOAT4 color; // インスタンスカラー（R,G,B,A）
    XMFLOAT4 uvRect; // UV矩形(x,y,width,height)

	InstanceData()
		: world0(1.0f, 0.0f, 0.0f, 0.0f)
		, world1(0.0f, 1.0f, 0.0f, 0.0f)
		, world2(0.0f, 0.0f, 1.0f, 0.0f)
		, world3(0.0f, 0.0f, 0.0f, 1.0f)
		, color(1.0f, 1.0f, 1.0f, 1.0f)
		, uvRect(0.0f, 0.0f, 1.0f, 1.0f)
    {
    }

};

enum ShaderBeginMode {
    Default = 0,
	TrueTypeFont,

	MAX,
};

bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader_Finalize();

void Shader_SetMatrix(const XMMATRIX& matrix);
void Shader_SetWorldMatrix(const XMMATRIX& world);

void Shader_Begin(ShaderBeginMode mode = ShaderBeginMode::Default);

void Shader_SetPixelOption(const XMFLOAT4& colorRate, float grayRate);

void Shader_BindVsConstantBuffer(UINT slot, ID3D11Buffer* pBuffer);
void Shader_BindPsConstantBuffer(UINT slot, ID3D11Buffer* pBuffer);

#endif // SHADER_H
