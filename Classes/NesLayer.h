//
//  NesLayer.h
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#ifndef __NesEmulator__NesLayer__
#define __NesEmulator__NesLayer__

#include "cocos2d.h"
#include "nes/NesEmulator.h"
#include "JoyStick.h"


#define JOYSTICK_OFFSET_X 8.0f
#define JOYSTICK_OFFSET_Y 8.0f
#define JOYSTICK_RADIUS   100.0f
#define THUMB_RADIUS      20.0f

enum GameState
{
    BEGIN,
    GAMING
};

class NesLayer : public cocos2d::LayerColor
{
public:
    NesLayer();
    ~NesLayer();
    bool init();
    CREATE_FUNC(NesLayer);
private:
    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _origin;

    int _screenDataSize;
    int *_screenData;
    int _scanLine;
    cocos2d::Texture2D *_texture;
    
    NesEmulator *_nesEmu;
    
    JoyStick *joyStick;
    
    GameState _state;
private:
    bool warmUpEmulator();
    void ScreenData();
    
    void update(float dt);
    void beginTvScreen();
    
    void fpsfun(float dt);
};

#endif /* defined(__NesEmulator__NesLayer__) */
