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


#ifndef __NesEmulator__util__
#define __NesEmulator__util__

#include <stdio.h>
#include <string>
#include <list>

std::string getFileExtension(std::string filePath);
std::string getFileWithoutExtension(std::string filePath);

std::string toLower(std::string *s);

bool replace(std::string& str, const std::string& from, const std::string& to);

static bool contain(std::list<std::string> *list, std::string str)
{
    for(std::list<std::string>::iterator it = list->begin(); it != list->end(); it++)
    {
        if(*it == str)
        {
            return true;
        }
    }
    return false;
}

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
                        (byte & 0x80 ? 1 : 0), \
                        (byte & 0x40 ? 1 : 0), \
                        (byte & 0x20 ? 1 : 0), \
                        (byte & 0x10 ? 1 : 0), \
                        (byte & 0x08 ? 1 : 0), \
                        (byte & 0x04 ? 1 : 0), \
                        (byte & 0x02 ? 1 : 0), \
                        (byte & 0x01 ? 1 : 0)

#endif /* defined(__NesEmulator__util__) */
