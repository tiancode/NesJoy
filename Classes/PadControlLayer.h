//
//  PadControlLayer.h
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#ifndef __NesJoy__PadControlLayer__
#define __NesJoy__PadControlLayer__

#include "cocos2d.h"
#include "NesGameMenuLayer.h"

class PadControlLayer : public cocos2d::Layer
{
public:
    PadControlLayer();
    ~PadControlLayer();
    bool init();
    CREATE_FUNC(PadControlLayer);
    
private:
    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _origin;
    
    cocos2d::Vec2 kCenter;
    cocos2d::Vec2 velocity;
    
    int JoystickID;
    int button1ID;
    int button2ID;
    
    cocos2d::Sprite* thumbPad;
    cocos2d::Sprite *joystickBackground;
    cocos2d::Sprite *buttonA;
    cocos2d::Sprite *buttonB;
    cocos2d::Sprite *start;
    cocos2d::Sprite *select;
    
    bool isPressed;
    bool isButtonA;
    bool isButtonB;
    bool isStart;
    bool isSelect;
    
    int _JOYSTICK_RADIUS;
    int _THUMB_RADIUS;
private:
    void initSprite();
    
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event);
    void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event);
    void nesStart(cocos2d::Ref* pSender);
    void nesSelect(cocos2d::Ref* pSender);
    void nesSelectGame(cocos2d::Ref* pSender);
    
    bool isPointInCircle(cocos2d::Vec2 point, cocos2d::Vec2 center, float radius);
    void updateVelocity(cocos2d::Vec2 point);
    bool handleLastTouch();
    void resetJoystick();
    
private:
    int JoyStickState;
    int keyState;
    void update(float dt);
    
private:
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    
    NesGameMenuLayer *gameMenu;
};

#endif /* defined(__NesJoy__PadControlLayer__) */
