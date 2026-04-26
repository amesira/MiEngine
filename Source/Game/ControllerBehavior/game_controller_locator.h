//---------------------------------------------------
// game_controller_locator.h
// 
// Author：Miu Kitamura
// Date  ：2026/04/27
//---------------------------------------------------
#ifndef GAME_CONTROLLER_LOCATOR_H
#define GAME_CONTROLLER_LOCATOR_H

// ゲーム全体でアクセス可能なコントローラーへのアクセスを提供するクラス
class GameControllerLocator {
private: friend class GameEffectController;
    // ゲームエフェクトコントローラーへのポインタ
    static inline GameEffectController* s_gameEffectController = nullptr;

public:
    // ゲームエフェクトコントローラーへのアクセス
    static GameEffectController* GetGameEffectController() {
        return s_gameEffectController;
    }

};

#endif // GAME_CONTROLLER_LOCATOR_H