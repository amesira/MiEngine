//===================================================
// collider_component.cpp
//===================================================
#include "collider_component.h"

#include "Engine/Core/game_object.h"

void ColliderComponent::SetLayer(Layer layer)
{
    if (GetOwner()) {
        GetOwner()->SetCollisionLayer(layer);
    }
}

ColliderComponent::Layer ColliderComponent::GetLayer() const
{
    if (GetOwner()) {
        return GetOwner()->GetCollisionLayer();
    }
    return Layer::Default;
}
