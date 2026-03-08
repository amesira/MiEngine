// type_id.h
// 型ごとに一意なIDを生成するためのユーティリティ
#ifndef TYPE_ID_H
#define TYPE_ID_H

struct ComponentTag {};
struct BehaviorTag {};

template<class T>
class TypeID {
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
};

using ComponentTypeID = TypeID<ComponentTag>;
using BehaviorTypeID = TypeID<BehaviorTag>;

#endif