//
//  JoyStick.h
//  NesJoy
//
//  Created by tianshuai on 7/1/15.
//
//

#ifndef __NesEmulator__Joypad__
#define __NesEmulator__Joypad__

#include "cocos2d.h"
#include "nes/Input.h"

#define SELECTBIT   1 << 5
#define STARTBIT    1 << 4
#define ABIT        1 << 3
#define BBIT        1 << 2
#define TRUBOA      1 << 1
#define TRUBOB      1 << 0
#define LEFTBIT     1 << 7
#define DOWNBIT     1 << 8
#define UPBIT       1 << 9
#define RIGHTBIT    1 << 6

#define SELECTBIT_0 0xFFFFFFDF
#define STARTBIT_0  0xFFFFFFEF
#define ABIT_0      0xFFFFFFF7
#define BBIT_0      0xFFFFFFFB
#define TRUBOA_0    0xFFFFFFFD
#define TRUBOB_0    0xFFFFFFFE
#define LEFTBIT_0   0xFFFFFF7F
#define DOWNBIT_0   0xFFFFFEFF
#define UPBIT_0     0xFFFFFDFF
#define RIGHTBIT_0  0xFFFFFFBF

//    // 9    8      7      6     5       4      3   2   1         0
//    // up  down  left   right  select  start   A   B   TruhboA   TruboB

class JoyStick : public IJoypadConnecter
{
private:
   // unsigned char DATA;
    bool turbo;
    
    int keyState;
public:
    void setKey(int KeyState)
    {
        this->keyState = KeyState;
    }
    void Update();
    
public:
    static JoyStick* getInstance();
private:
    static JoyStick *__instance;
    JoyStick();
};

#endif /* defined(__NesEmulator__Joypad__) */
