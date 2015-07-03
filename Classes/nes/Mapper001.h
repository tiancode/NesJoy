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

#ifndef __NesEmulator__Mapper001__
#define __NesEmulator__Mapper001__

#include "Board.h"

class Mapper001 : public Board
{
public:
    Mapper001();
    
    void hardReset();
    void WritePRG(int address, unsigned char value);
    void OnCPUClock();
private:
    void UpdateCHR();
    void UpdatePRG();
private:
    int address_reg;
    unsigned char* reg;
    unsigned char shift;
    unsigned char buffer;
    bool flag_p;
    bool flag_c;
    bool flag_s;
    bool enable_wram_enable;
    int prg_hijackedbit;
    bool use_hijacked;
    bool use_sram_switch;
    int sram_switch_mask;
    int cpuCycles;
};

#endif /* defined(__NesEmulator__Mapper001__) */
