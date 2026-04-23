// type_id.h
// 型ごとに一意なIDを生成するためのユーティリティ
#ifndef TYPE_ID_H
#define TYPE_ID_H

#include "component.h"

#include "Engine/Framework/Component/collider_component.h"
#include "Engine/Framework/Component/behavior_component.h"

class ComponentTypeID {
private:
    // 次に割り当てる型ID
    inline static int m_nextTypeId = 0;

public:
    // 型ごとに一意なIDを取得
    template <class T>
    static int getTypeID() {
        static int typeId = m_nextTypeId++;
        return typeId;
    }

    // 型のベースとなる型のIDを取得
    template <class T>
    static int getBaseTypeID() {
        if constexpr (std::is_base_of_v<ColliderComponent, T>)
            return getTypeID<ColliderComponent>();
        if constexpr (std::is_base_of_v<BehaviorComponent, T>)
            return getTypeID<BehaviorComponent>();
        return getTypeID<Component>();
    }
};

#endif