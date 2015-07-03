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

#ifndef __NesEmulator__Rom__
#define __NesEmulator__Rom__

#include "Types.h"
#include <string>

class Rom
{
public:
    bool parseNes(unsigned char* fileString, int len ,bool loadDumps);
    
    int getMapperNumber() { return _mapperNumber; }
    int getPRGCount() { return _PRGCount; }
    int getCHRCount() { return _CHRCount; }
    int getPRGSize() { return _PRG_size; }
    int getCHRSize() { return _CHR_size; }
    int getTrainerSize() { return _trainer_size; }
    Mirroring getMirroring() { return _mirroring; }
    bool isHasBattery() { return _hasBattery; }
    bool isHasTrainer() { return _hasTrainer; }
    bool isPlaychoice10() { return _isPlaychoice10; }
    bool isVSUnisystem() { return _isVSUnisystem; }
    unsigned char* getTrainer() { return _trainer; }
    unsigned char* getPRG() { return _PRG; }
    unsigned char* getCHR() { return _CHR; }
    std::string getSHA1() { return _SHA1; }
    
    Rom();
    ~Rom();
private:
    int _PRGCount;
    int _CHRCount;
    Mirroring _mirroring;
    bool _hasBattery;
    bool _hasTrainer;
    int _mapperNumber;
    bool _isPlaychoice10;
    bool _isVSUnisystem;
    
    unsigned char *_trainer;
    int _trainer_size;
    unsigned char *_PRG;
    int _PRG_size;
    unsigned char *_CHR;
    int _CHR_size;
    
    std::string _SHA1;
};

#endif /* defined(__NesEmulator__Rom__) */
