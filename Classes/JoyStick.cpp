//
//  JoyStick.cpp
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#include "JoyStick.h"

JoyStick::JoyStick()
{
    
}

JoyStick *JoyStick::__instance = NULL;

JoyStick* JoyStick::getInstance()
{
    if(__instance == NULL)
        __instance = new JoyStick;
    return __instance;
}

void JoyStick::Update()
{
    turbo = !turbo;
    DATA = 0;
    
    if ((keyState >> 3 & 0x1) == 1)   // A
        DATA |= 1;
    if ((keyState >> 2 & 0x1) == 1)   //B
        DATA |= 2;
    if ((keyState >> 1 & 0x1) == 1 && turbo)  //turboA
        DATA |= 1;
    if ((keyState >> 0 & 0x1) == 1 && turbo) //turboB
        DATA |= 2;
     if ((keyState >> 5 & 0x1) == 1)  //select
        DATA |= 4;
    if ((keyState >> 4 & 0x1) == 1) //start
            DATA |= 8;
    if ((keyState >> 9 & 0x1) == 1)  //up
        DATA |= 0x10;
    if ((keyState >> 8 & 0x1) == 1) //down
        DATA |= 0x20;
    if ((keyState >> 7 & 0x1) == 1)  //left
        DATA |= 0x40;
    if ((keyState >> 6 & 0x1) == 1) //right
        DATA |= 0x80;
}
