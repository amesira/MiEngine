//---------------------------------------------------
// transform_constraint_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/06/02
//---------------------------------------------------
#ifndef TRANSFORM_CONSTRAINT_BEHAVIOR_H
#define TRANSFORM_CONSTRAINT_BEHAVIOR_H
#include "Engine/Framework/Component/behavior_component.h"
#include "Engine/Device/direct3d.h"

using namespace DirectX;

class TransformComponent;

class TransformConstraintBehavior : public BehaviorComponent {
private:
    TransformComponent* m_transform = nullptr;

    // 制約の対象
    TransformComponent* m_target = nullptr;

    // 対象からのオフセット
    XMFLOAT3 m_offset = { 0.0f, 0.0f, 0.0f };

    bool m_considerRotation = true; // 回転を考慮するかどうか
    bool m_considerScaling = false; // スケーリングを考慮するかどうか

public:
    void Start() override;
    void Update() override;
    void DrawComponentInspector() override;

    // 制約の対象の設定・取得
    void SetTarget(TransformComponent* target) { m_target = target; }
    TransformComponent* GetTarget() const { return m_target; }

    // オフセットの設定・取得
    void SetOffset(const XMFLOAT3& offset) { m_offset = offset; }
    const XMFLOAT3& GetOffset() const { return m_offset; }

    // 回転を考慮するかどうかの設定・取得
    void SetConsiderRotation(bool consider) { m_considerRotation = consider; }
    bool GetConsiderRotation() const { return m_considerRotation; }
    // スケーリングを考慮するかどうかの設定・取得
    void SetConsiderScaling(bool consider) { m_considerScaling = consider; }
    bool GetConsiderScaling() const { return m_considerScaling; }

private:
    // 制約された位置の計算
    XMFLOAT3 CalculateConstrainedPosition() const;
};

#endif // TRANSFORM_CONSTRAINT_BEHAVIOR_H
