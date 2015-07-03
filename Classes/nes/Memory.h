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

#ifndef __NesEmulator__Memory__
#define __NesEmulator__Memory__

/*-------------------------------------------------------------------------------------------------------------
   | Zero Page | Stack | RAM | Mirrors($0000-$07FF) | I/O Registers | Mirrors($2000-$2007) | I/O Registers  |
   |           |       |     |                      |               |                      |                |
 $0000--------$0100---$0200--$0800-----------------$2000----------$2008-------------------$4000-----------$4020--
 
 -----------------------------------------------------------------------------------------------------------
   Expansion ROM | SRAM | PRG-ROM Lower Bank |  PRG-ROM Upper Bank  |                                      |
                 |      |                    |                      |                                      |
 --------------$6000---$8000---------------$C000-----------------$10000---------------------------------*/

#include "Rom.h"
#include "Board.h"

class Memory
{
public:
    bool init(Rom *rom);
    void hardReset();
    
    unsigned char read(int address);
    void write(int address, unsigned char value);
    
    Board *board;
public:
    unsigned char *ram;
    unsigned char palettes_bank[32];
    // Accessed via $2004
    unsigned char* oam_ram;
    // The secondary oam...
    unsigned char* oam_secondary;
    
    int BUS_ADDRESS;
    bool BUS_RW;
    bool BUS_RW_P;
    
    // temps
    unsigned char temp_4015;
    unsigned char temp_4016;
    unsigned char temp_4017;
public:
    static Memory* getInstance();
    static void destroy();
    ~Memory();

    static Memory* __instance;
private:
    Memory();
};

#endif /* defined(__NesEmulator__Memory__) */
