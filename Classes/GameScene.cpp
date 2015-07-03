//
//  GameScene.cpp
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#include "GameScene.h"

#include "NesLayer.h"
#include "PadControlLayer.h"

USING_NS_CC;
GameScene::GameScene()
{
    
}

GameScene::~GameScene()
{
    
}

bool GameScene::init()
{
    if( !Scene::init() )
    {
        return false;
    }
    
    NesLayer *nesLayer = NesLayer::create();
    this->addChild(nesLayer);
    
    PadControlLayer *control = PadControlLayer::create();
    this->addChild(control,1);
    
    
    return true;
}

