/* This file is part of NesEmulator
 *
 * A Nintendo Entertainment System / Family Computer (Nes/Famicom)
 * Emulator written in C++.
 *
 * Copyright © tianshuai - 2015
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Input.h"
#include <stddef.h>

Input* Input::__instance = NULL;
#include "VSUnisystem.h"

Input::Input()
{
    joypad1 = new IJoypadConnecter;
    joypad2 = new IJoypadConnecter;
    joypad3 = new IJoypadConnecter;
    joypad4 = new IJoypadConnecter;
}

Input::~Input()
{
    
}

Input* Input::getInstance()
{
    if( __instance == NULL )
    {
        __instance = new Input;
    }
    return __instance;
}

void Input::destroy()
{
    if( __instance != NULL )
    {
        delete __instance;
        __instance = NULL;
    }
}


void Input::InputFinishFrame()
{
    joypad1->Update();
    joypad2->Update();
    if (IsFourPlayers)
    {
        if(joypad3 != NULL )
            joypad3->Update();
        if(joypad4 != NULL)
            joypad4->Update();
    }
    if (IsZapperConnected)
        zapper->Update();
    if (VSUnisystem::IsVSUnisystem)
        VSUnisystemDIP->Update();
}

void Input::SetupJoypads(IJoypadConnecter *joy1, IJoypadConnecter *joy2, IJoypadConnecter *joy3, IJoypadConnecter *joy4)
{
    if (joy1 != NULL)
        joypad1 = joy1;
    if(joy2 != NULL)
        joypad2 = joy2;
    if(joy3 != NULL)
        joypad3 = joy3;
    if(joy4 != NULL)
        joypad4 = joy4;
}
