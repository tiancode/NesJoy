//
//  NesLayer.cpp
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#include "NesLayer.h"

USING_NS_CC;
NesLayer::NesLayer()
{
    _scanLine = 240;
}

NesLayer::~NesLayer()
{

}


Label *fps;
bool NesLayer::init()
{
    if(LayerColor::initWithColor(Color4B(random(0, 255) ,random(0, 255),random(0, 255),random(100, 200))) == false)
    {
        return false;
    }
    _visibleSize = Director::getInstance()->getVisibleSize();
    _origin = Director::getInstance()->getVisibleOrigin();
    
    _state = GAMING;
    
    warmUpEmulator();
    
    ScreenData();
    
    joyStick = JoyStick::getInstance();
    _nesEmu->setInput(joyStick, NULL, NULL, NULL);
    
    scheduleUpdate();
    
//    fps= Label::createWithTTF("fsa", "fonts/arial.ttf", 70);
//    fps->setPosition(Vec2(300,300));
//    this->addChild(fps);
    //schedule(schedule_selector(NesLayer::fpsfun), 1.0, CC_REPEAT_FOREVER, 0);
    
    return true;
}

void NesLayer::fpsfun(float dt)
{
    static int pre = NesEmulator::__instance->FPSDone;
    int curr = NesEmulator::__instance->FPSDone;
    char buff[4];
    sprintf(buff, "%d", curr - pre);
    fps->setString(buff);
    pre = NesEmulator::__instance->FPSDone;
}

void NesLayer::update(float dt)
{
    if(_state == BEGIN)
    {
        beginTvScreen();
    }
    else if(_state == GAMING)
    {
        _texture->updateWithData(_screenData, 0, 0, 256, _scanLine);
    }
}


void NesLayer::beginTvScreen()
{
    for (int i = 0; i < _screenDataSize; i++)
    {
        unsigned char c = (unsigned char)cocos2d::random(0, 255);
        _screenData[i] = c | (c << 8) | (c << 16) | (255 << 24);
    }
    _texture->updateWithData(_screenData, 0, 0, 256, _scanLine);
}

// warm up emulator
bool NesLayer::warmUpEmulator()
{
    Data database = FileUtils::getInstance()->getDataFromFile("nes/database.xml");
    if(database.getSize() == 0)
    {
        printf("database.xml file read fail\n");
        return false;
    }
    _nesEmu = NesEmulator::getInstance();
    _nesEmu->warmUp((const char*)database.getBytes());
    
    return true;
}

/*
 *  初始化screen data
 */
void NesLayer::ScreenData()
{
    bool cutLines = false;
    int firstToCut = 0;
    if (cutLines)
    {
        if (NesEmulator::__instance->TVFormat == NTSC)
        {
            _scanLine = 224;
            firstToCut = 8;
        }
        else
        {
            _scanLine = 238;
            firstToCut = 1;
        }
    }
    else
    {
        _scanLine = 240;
        firstToCut = 0;
    }
    _screenDataSize = 256 * _scanLine;
    _screenData = new int[256 * _scanLine];
    
    for (int i = 0; i < _screenDataSize; i++)
    {
        unsigned char c = (unsigned char)cocos2d::random(0, 255);
        _screenData[i] = c | (c << 8) | (c << 16) | (255 << 24);
    }
    
    _texture = new Texture2D;
    Size contentSize;
    _texture->initWithData(_screenData, 256 * _scanLine * 4, Texture2D::PixelFormat::RGBA8888, 256, _scanLine, contentSize);
    
    Sprite *screen = Sprite::createWithTexture(_texture);
    screen->setPosition(Vec2(_visibleSize.width/2 + _origin.x, _visibleSize.height/2 + _origin.y));
    screen->setScale( _visibleSize.height / screen->getContentSize().height );
    this->addChild(screen);
    
    _nesEmu->setScreenPointer(_screenData, firstToCut * 256, 256 * (240 - firstToCut));
}
