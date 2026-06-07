#pragma once

class EnemyBehavior;
class EnemyStateMachineBehavior;
class BaseEnemyAttackBehavior;

class TransformComponent;
class RigidbodyComponent;
class GameObject;

enum class EnemyState {
    Idle,
    Chase,
    Attack,
    Stunned,
    Dead,
};

enum class EnemyAttackType {
    Melee,
    Shooter,
    Area,
};

struct EnemyContext {
    EnemyState state = EnemyState::Idle;
    EnemyAttackType attackType = EnemyAttackType::Melee;

    GameObject* targetObject = nullptr;
    TransformComponent* targetTransform = nullptr;

    TransformComponent* transform = nullptr;
    RigidbodyComponent* rigidbody = nullptr;

    EnemyBehavior* enemyBehavior = nullptr;
    EnemyStateMachineBehavior* stateMachine = nullptr;
    BaseEnemyAttackBehavior* attackBehavior = nullptr;

    float distanceToTarget = 0.0f;
    bool canSeeTarget = false;
};
