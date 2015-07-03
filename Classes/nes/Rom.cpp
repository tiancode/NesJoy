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

#include "Rom.h"
#include "sha1.h"

Rom::Rom()
{
    _trainer = NULL;
    _PRG = NULL;
    _CHR = NULL;
    
    _trainer_size = 0;
    _CHR_size = 0;
    _PRG_size = 0;
}

Rom::~Rom()
{
    if(_trainer != NULL)
        free(_trainer);
    if(_PRG != NULL)
        free(_PRG);
    if(_CHR != NULL)
        free(_CHR);
}

bool Rom::parseNes(unsigned char* fileString, int len, bool loadDumps)
{
    if(len < 16)
    {
        return false;
    }
    unsigned char *filePos = fileString;
    unsigned char header[16] = { 0 };
    memcpy(header, filePos, 16);
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A)
    {
        return false;
    }
    _PRGCount = header[4];
    _CHRCount = header[5];
    switch (header[6] & 0x9)
    {
        case 0x0:
            _mirroring= Horz;
            break;
        case 0x1:
            _mirroring = Vert;
            break;
        case 0x8:
        case 0x9:
            _mirroring = Full;
            break;
    }
    _hasBattery = (header[6] & 0x2) != 0x0;
    _hasTrainer = (header[6] & 0x4) != 0x0;
    if ((header[7] & 0x0F) == 0)
        _mapperNumber = (unsigned char)((header[7] & 0xF0) | (header[6] >> 4));
    else
        _mapperNumber = (unsigned char)((header[6] >> 4));
    _isVSUnisystem = (header[7] & 0x01) != 0;
    _isPlaychoice10 = (header[7] & 0x02) != 0;
    
    filePos += 16;
    char *buff = new char[len - 16];
    char *buffer = buff;
    memcpy(buffer, filePos, len - 16);
    _SHA1 = "";
    unsigned char output[32] = { 0 };
    SHA1((char*)output, buffer, len - 16);
    char strBuff[4] = { 0 };
    for(int i = 0; i < 20; i++)
    {
        sprintf(strBuff, "%02x", output[i]);
        _SHA1 += strBuff;
    }
    
    if (loadDumps)
    {
        if (_hasTrainer)
        {
            _trainer = (unsigned char*)malloc(sizeof(char)*512);
            memcpy(_trainer, buffer, 512);
            _trainer_size = 512;
            buffer += 512;
        }
        
        _PRG = (unsigned char*)malloc(sizeof(char)*_PRGCount * 0x4000);  // 0x4000 - 16kb
        memcpy(_PRG, buffer, _PRGCount * 0x4000);
        _PRG_size = _PRGCount * 0x4000;
        buffer += _PRG_size;
        
        if (_CHRCount > 0)
        {
            _CHR = (unsigned char*)malloc(sizeof(char)*_CHRCount * 0x2000);  // 0x2000 - 8kb
            memcpy(_CHR, buffer, _CHRCount * 0x2000);
            _CHR_size = _CHRCount * 0x2000;
        }
    }
    
    delete buff;
    return true;
}
