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

#ifndef _VSUNISYSTEM_H_
#define _VSUNISYSTEM_H_

#include <string>

class VSUnisystem
{
public:
    VSUnisystem();
    ~VSUnisystem();
    
    static bool IsVSUnisystem;
    
    static void CheckGameVSUnisystem(std::string sha1, bool trueByHeader, int mapperNumber);
    
private:
    static int VS1[];
    static int VS2[];
    static int VS3[];
    static int VS4[];
    static int VS5[];
    static int VS6[];
    static int VS7[];
};

#endif
