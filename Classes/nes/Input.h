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

#ifndef __NesEmulator__Input__
#define __NesEmulator__Input__


class IJoypadConnecter
{
protected:
    unsigned char DATA;
    
public:
    // Nes emulation engine call this at frame end, can be used to update input device status.
    virtual void Update() {};

    // Nes emulation engine call this at $4016 writes.
    char GetData()
    {
        return DATA;
    }
};

class IZapperConnecter
{
public:
    bool Trigger;
    bool State;
    
    /// Nes emulation engine call this at frame end, can be used to update input device status.
    void Update() {};

    // Nes emulation engine call this at $4016 writes.
    // <returns>The input data</returns>
    char GetData()
    {
        return (char)((Trigger ? 0x10 : 0x0) | (State ? 0x8 : 0x0));
    }
};

class IVSUnisystemDIPConnecter
{
public:
    void Update() {};
    char GetData4016()
    {
        return 0;
    }
    char GetData4017()
    {
        return 0;
    }
    void Write4020(char data)
    {
        
    }
};

class Input
{
public:
    void SetupJoypads(IJoypadConnecter *joy1, IJoypadConnecter *joy2, IJoypadConnecter *joy3, IJoypadConnecter *joy4);
    void InputFinishFrame();
public:
    // TODO: controllers for zapper and vsunisystem
    int PORT0;
    int PORT1;
    int inputStrobe;
    IJoypadConnecter *joypad1;
    IJoypadConnecter *joypad2;
    IJoypadConnecter *joypad3;
    IJoypadConnecter *joypad4;
    IZapperConnecter *zapper;
    IVSUnisystemDIPConnecter *VSUnisystemDIP;
    bool IsFourPlayers = false;
    bool IsZapperConnected = false;

public:
    ~Input();
    static Input *getInstance();
    static void destroy();
    
    static Input* __instance;
private:
    Input();
};

#endif /* defined(__NesEmulator__Input__) */
