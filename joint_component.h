//---------------------------------------------------
// joint_component.h
// 
// Author：Miu Kitamura
// Date  ：2026/03/25
//---------------------------------------------------
#ifndef JOINT_COMPONENT_H
#define JOINT_COMPONENT_H
#include "component.h"

class JointComponent : public Component {
private:
    unsigned int m_groupID = 0;         // ジョイントグループのID

    unsigned int m_connectedBodyID = 0; // 接続された剛体のID
    float   m_restLength = 1.0f;        // 目標の距離

public:
    JointComponent() {}
    ~JointComponent() {}

    // ジョイントグループのIDを設定・取得
    void SetGroupID(unsigned int groupID) { m_groupID = groupID; }
    unsigned int GetGroupID() const { return m_groupID; }

    // 接続された剛体のIDを設定・取得
    void SetConnectedBodyID(unsigned int bodyID) { m_connectedBodyID = bodyID; }
    unsigned int GetConnectedBodyID() const { return m_connectedBodyID; }
    // 目標の距離を設定・取得
    void SetRestLength(float restLength) { m_restLength = restLength; }
    float GetRestLength() const { return m_restLength; }

};

#endif // JOINT_COMPONENT_H