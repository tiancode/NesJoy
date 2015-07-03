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

#ifndef __NesEmulator__NesCartDatabase__
#define __NesEmulator__NesCartDatabase__

#include <string>
#include <list>

struct SRAMBankInfo
{    
    int id;
    std::string SIZE;
    bool BATTERY;
};

struct NesCartDatabaseCartridgeElement
{
    //cartridge info
    std::string System;
    std::string CRC;
    std::string SHA1;
    std::string Dump;
    std::string Dumper;
    std::string DateDumped;
    //board info
    std::string Board_Type;
    std::string Board_Pcb;
    std::string Board_Mapper;
    //vram
    std::list<std::string> VRAM_sizes;
    
    //wram (s-ram)
    std::list<SRAMBankInfo> WRAMBanks;
    
    //prg
    std::string PRG_name;
    std::string PRG_size;
    std::string PRG_crc;
    std::string PRG_sha1;
    
    //chr
    std::string CHR_name;
    std::string CHR_size;
    std::string CHR_crc;
    std::string CHR_sha1;
    
    //chip, may be more than one chip
    std::list<std::string> chip_type;
    
    //cic
    std::string CIC_type;
    
    //pad
    std::string PAD_h;
    std::string PAD_v;
};

struct NesCartDatabaseGameElement
{
    std::string Game_Name;
    std::string Game_AltName;
    std::string Game_Class;
    std::string Game_Catalog;
    std::string Game_Publisher;
    std::string Game_Developer;
    std::string Game_Region;
    std::string Game_Players;
    std::string Game_ReleaseDate;
    
    //cartridges, game may has one or more cartridge dump
    std::list<NesCartDatabaseCartridgeElement> Cartridges;
};

class NesCartDatabase
{
public:
    NesCartDatabase();
    ~NesCartDatabase();
    
    bool loadDatabase(const char* database);
    std::list<NesCartDatabaseGameElement*> getRomDatabase() { return _romDatabase; }
    // Cart_sha1  The sha1 to match, file sha1 without header of INES (start index 16)
    NesCartDatabaseGameElement* findRom( std::string Cart_sha1 );
private:
    std::list<NesCartDatabaseGameElement*> _romDatabase;
    std::string DBVersion = "";
    std::string DBConformance = "";
    std::string DBAgent = "";
    std::string DBAuthor = "";
    std::string DBTimeStamp = "";
};

#endif /* defined(__NesEmulator__NesCartDatabase__) */
