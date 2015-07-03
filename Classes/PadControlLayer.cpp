//
//  PadControlLayer.cpp
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#include "PadControlLayer.h"
#include "JoyStick.h"


USING_NS_CC;
PadControlLayer::PadControlLayer()
{
    keyState = 0;
}

PadControlLayer::~PadControlLayer()
{
    
}

bool PadControlLayer::init()
{
    if(!Layer::init())
        return false;
    
    _visibleSize = Director::getInstance()->getVisibleSize();
    _origin = Director::getInstance()->getVisibleOrigin();
    
    initSprite();
    
    gameMenu = NesGameMenuLayer::create();
    this->addChild(gameMenu,3);
    gameMenu->setVisible(false);
    gameMenu->setShow(false);
    
    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = CC_CALLBACK_2(PadControlLayer::onTouchesBegan, this);
    touchListener->onTouchesMoved = CC_CALLBACK_2(PadControlLayer::onTouchesMoved, this);
    touchListener->onTouchesEnded = CC_CALLBACK_2(PadControlLayer::onTouchesEnded, this);
    touchListener->onTouchesEnded = CC_CALLBACK_2(PadControlLayer::onTouchesCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed =  CC_CALLBACK_2(PadControlLayer::onKeyPressed, this);
    keyListener->onKeyReleased = CC_CALLBACK_2(PadControlLayer::onKeyRelease, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
    
    scheduleUpdate();
    
    return true;
}

void PadControlLayer::update(float dt)
{
    JoyStickState = 0;
    if(velocity.x >= 0.1 && velocity.y >= -0.2 && velocity.y <= 0.2)
    {
        JoyStickState = JoyStickState | RIGHTBIT;
    }
    if(velocity.x <= -0.1 && velocity.y >= -0.2 && velocity.y <=0.2)
    {
        JoyStickState = JoyStickState | LEFTBIT;
    }
    if(velocity.y >= 0.1 &&  velocity.x >= -0.2 && velocity.x <= 0.2)
    {
        JoyStickState = JoyStickState | UPBIT;
    }
    if(velocity.y <= -0.1 &&  velocity.x >= -0.2 && velocity.x <= 0.2)
    {
        JoyStickState = JoyStickState | DOWNBIT;
    }
    if(isButtonA == true)
    {
        JoyStickState = JoyStickState | ABIT;
    }
    if(isButtonB == true)
    {
        JoyStickState = JoyStickState | BBIT;
    }
    if(isStart == true)
    {
        JoyStickState = JoyStickState | STARTBIT;
        isStart = false;
    }
    if(isSelect == true)
    {
        JoyStickState = JoyStickState | SELECTBIT;
        isSelect = false;
    }
    /* 更新joystick状态 */
    //JoyStick::getInstance()->setKey(JoyStickState);
    JoyStick::getInstance()->setKey(keyState);
    //keyState
}

void PadControlLayer::initSprite()
{
    velocity = Vec2::ZERO;
    isPressed = false;
    isButtonA = false;
    isButtonB = false;
    isStart = false;
    isSelect = false;
    
    float factor = 1.5;
    float JoySize = _visibleSize.height*124.8*factor/720;      // 124.8
    
    float edgeLong =  _visibleSize.height* 100*factor/720;   // 100
    kCenter = Vec2( edgeLong, edgeLong );
    _JOYSTICK_RADIUS = edgeLong;
    _THUMB_RADIUS = _visibleSize.height*20.0*factor/720;
    
    joystickBackground = Sprite::create("gamepad/ThumbPadRing.png");
    joystickBackground->setScale( JoySize/78 );
    joystickBackground->setPosition(kCenter);
    this->addChild(joystickBackground,0);
    
    float thumbSize = _visibleSize.height*57.6*factor/720;
    thumbPad = Sprite::create("gamepad/ThumbPad.png");
    thumbPad->setScale(thumbSize/64);
    thumbPad->setPosition(kCenter);
    this->addChild(thumbPad,1);
    
    float thumbab = _visibleSize.height*64*factor/720;
    buttonA = Sprite::create("gamepad/ButtonA.png");
    buttonA->setScale(thumbab/64);
    buttonA->setPosition(Vec2(_visibleSize.width - _visibleSize.height*160*factor/720, _visibleSize.height*80*factor/720));
    this->addChild(buttonA,1);
    
    buttonB = Sprite::create("gamepad/ButtonB.png");
    buttonB->setScale(thumbab/64);
    buttonB->setPosition(Vec2(_visibleSize.width - _visibleSize.height*70*factor/720, _visibleSize.height*115*factor/720));
    this->addChild(buttonB,1);
    
    float startSize = _visibleSize.width*130/1280;
    MenuItemImage *start = MenuItemImage::create("gamepad/start.png", "gamepad/start.png", CC_CALLBACK_1(PadControlLayer::nesStart, this));
    start->setScale(startSize/600);
    start->setPosition(_visibleSize.width - _visibleSize.width*100/1280, _visibleSize.height /2);
    
    MenuItemImage *select = MenuItemImage::create("gamepad/select.png", "gamepad/select.png", CC_CALLBACK_1(PadControlLayer::nesSelect, this));
    select->setScale(startSize/600);
    select->setPosition(_visibleSize.width - _visibleSize.width*100/1280, _visibleSize.height /2 - _visibleSize.height *70/720);
    
    float nesCardSize = _visibleSize.height*65/720;
    MenuItemImage *nescard = MenuItemImage::create("gamepad/nescard.png", "gamepad/nescard.png", CC_CALLBACK_1(PadControlLayer::nesSelectGame, this));
    nescard->setScale(nesCardSize/200);
    nescard->setPosition(Vec2(_visibleSize.width*50/1280, _visibleSize.height - _visibleSize.height*50/720));
    
    Menu *menu1 = Menu::create(start, select, nescard ,NULL);
    menu1->setPosition(Vec2::ZERO);
    this->addChild(menu1,1);
}

void PadControlLayer::nesSelectGame(Ref* pSender)
{
    /* nes 选择菜单 */
    gameMenu->setVisible(true);
    gameMenu->setShow(true);
}

void PadControlLayer::nesStart(Ref* pSender)
{
    isStart = true;
}
void PadControlLayer::nesSelect(Ref* pSender)
{
    isSelect = true;
}

void PadControlLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event  *event)
{
    Vec2 pt;
    int touchID;
    for ( auto &item: touches )
    {
        auto touch = item;
        pt = touch->getLocation();
        touchID = touch->getID();
        if ( isPointInCircle(pt,kCenter,_JOYSTICK_RADIUS) )
        {
            JoystickID = touchID;
            isPressed = true;
            this->updateVelocity(pt);
        }
        if( buttonA->boundingBox().containsPoint(pt) )
        {
            button1ID = touchID;
            isButtonA = true;
        }
        if( buttonB->boundingBox().containsPoint(pt) )
        {
            button2ID = touchID;
            isButtonB = true;
        }
    }
}

void PadControlLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event  *event)
{
    Vec2 pt;
    int touchID;
    for ( auto &item: touches )
    {
        auto touch = item;
        pt = touch->getLocation();
        touchID = touch->getID();
        if ( JoystickID == touch->getID() && isPressed )
        {
            this->updateVelocity(pt);
        }
    }
}
void PadControlLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event  *event)
{
    Vec2 pt;
    int touchID;
    for ( auto &item: touches )
    {
        auto touch = item;
        pt = touch->getLocation();
        touchID = touch->getID();
        
        if( JoystickID == touchID && isPressed )
        {
            this->handleLastTouch();
        }
        if ( button1ID == touchID && isButtonA)
        {
            isButtonA = false;
        }
        if ( button2ID == touchID && isButtonB )
        {
            isButtonB = false;
        }
    }

}
void PadControlLayer::onTouchesCancelled(const std::vector<Touch*>& touches, Event  *event)
{
    onTouchesEnded(touches, event);
}

bool PadControlLayer::isPointInCircle(Vec2 point, Vec2 center, float radius)
{
    float dx = (point.x - center.x);
    float dy = (point.y - center.y);
    return (radius >= sqrt((dx*dx)+(dy*dy)));
}

void PadControlLayer::updateVelocity(Vec2 point)
{
    float dx = point.x - kCenter.x;
    float dy = point.y - kCenter.y;
    
    float distance = sqrt(dx*dx + dy*dy);
    float angle = atan2(dy,dx); /* in radians */
    
    if(distance > _JOYSTICK_RADIUS)
    {
        dx = cos(angle) * _JOYSTICK_RADIUS;
        dy = sin(angle) * _JOYSTICK_RADIUS;
    }
    
    velocity = Vec2(dx/_JOYSTICK_RADIUS, dy/_JOYSTICK_RADIUS);
    
    if(distance>_THUMB_RADIUS)
    {
        point.x = kCenter.x + cos(angle) * _THUMB_RADIUS;
        point.y = kCenter.y + sin(angle) * _THUMB_RADIUS;
    }
    
    thumbPad->setPosition(point);
}

bool PadControlLayer::handleLastTouch()
{
    bool wasPressed = isPressed;
    if(wasPressed)
    {
        this->resetJoystick();
        isPressed = false;
    }
    return (wasPressed ? true : false);
}

void PadControlLayer::resetJoystick()
{
    this->updateVelocity(kCenter);
}

void PadControlLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch(keyCode)
    {
        case EventKeyboard::KeyCode::KEY_Y:
        {
            keyState = keyState | SELECTBIT;
            break;
        }
        case EventKeyboard::KeyCode::KEY_U:
        {
            keyState = keyState | STARTBIT;
            break;
        }
        case EventKeyboard::KeyCode::KEY_K:
        {
            keyState = keyState | ABIT;
            break;
        }
        case EventKeyboard::KeyCode::KEY_L:
        {
            keyState = keyState | BBIT;
            break;
        }
        case EventKeyboard::KeyCode::KEY_I:
        {
            keyState = keyState | TRUBOA;
            break;
        }
        case EventKeyboard::KeyCode::KEY_O:
        {
            keyState = keyState | TRUBOB;
            break;
        }
        case EventKeyboard::KeyCode::KEY_A:
        {
            keyState = keyState | LEFTBIT;
            break;
        }
        case EventKeyboard::KeyCode::KEY_S:
        {
            keyState = keyState | DOWNBIT;
            break;
        }
        case EventKeyboard::KeyCode::KEY_D:
        {
            keyState = keyState | RIGHTBIT;
            break;
        }
        case EventKeyboard::KeyCode::KEY_W:
        {
            keyState = keyState | UPBIT;
            break;
        }
        default:
            break;
    }
}

void PadControlLayer::onKeyRelease(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch(keyCode)
    {
        case EventKeyboard::KeyCode::KEY_Y:
        {
            keyState = keyState & SELECTBIT_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_U:
        {
            keyState = keyState & STARTBIT_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_K:
        {
            keyState = keyState & ABIT_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_L:
        {
            keyState = keyState & BBIT_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_I:
        {
            keyState = keyState & TRUBOA_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_O:
        {
            keyState = keyState & TRUBOB_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_A:
        {
            keyState = keyState & LEFTBIT_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_S:
        {
            keyState = keyState & DOWNBIT_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_D:
        {
            keyState = keyState & RIGHTBIT_0;
            break;
        }
        case EventKeyboard::KeyCode::KEY_W:
        {
            keyState = keyState & UPBIT_0;
            break;
        }
        default:
            break;
    }
}
