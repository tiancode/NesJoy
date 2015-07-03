//
//  GameScene.h
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#ifndef __NesEmulator__GameScene__
#define __NesEmulator__GameScene__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    GameScene();
    ~GameScene();
    bool init();
    CREATE_FUNC(GameScene);
};

#endif /* defined(__NesEmulator__GameScene__) */
