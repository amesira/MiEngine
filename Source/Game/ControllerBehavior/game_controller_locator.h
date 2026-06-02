//---------------------------------------------------
// game_controller_locator.h
// 
// Author: Miu Kitamura
// Date  : 2026/04/27
//---------------------------------------------------
#ifndef GAME_CONTROLLER_LOCATOR_H
#define GAME_CONTROLLER_LOCATOR_H

class GameEffectController;
class CustomPostEffectController;

class GameControllerLocator {
private:
    friend class GameEffectController;
    friend class CustomPostEffectController;

    static inline GameEffectController* s_gameEffectController = nullptr;
    static inline CustomPostEffectController* s_customPostEffectController = nullptr;

public:
    static GameEffectController* GetGameEffectController() {
        return s_gameEffectController;
    }

    static CustomPostEffectController* GetCustomPostEffectController() {
        return s_customPostEffectController;
    }

};

#endif // GAME_CONTROLLER_LOCATOR_H
