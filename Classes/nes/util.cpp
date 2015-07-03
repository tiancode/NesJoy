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


#include "util.h"

std::string getFileExtension(std::string filePath)
{
    if(filePath.find_last_of(".") != std::string::npos)
        return filePath.substr(filePath.find_last_of(".")+1);
    return "";
}

std::string getFileWithoutExtension(std::string filePath)
{
    char buff[128] = {0};
    strcpy(buff, filePath.c_str());
    char *p = buff;
//    while(*p != '\0')
//    {
//        if(*p == '/')
//            *p = '_';
//        p++;
//    }
    p = p + filePath.length()-1;
    while(p != buff)
    {
        if(*p == '.')
        *p = '\0';
        p--;
    }
    return buff;
}

std::string toLower(std::string *s)
{
    std::string str = "";
    const char *p = s->c_str();
    while(*p != '\0')
    {
        str += tolower(*p);
        p++;
    }
    
    return str;
}

bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
