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

#include "Board.h"
#include "NesEmulator.h"
#include "util.h"

#include "Mapper000.h"
#include "Mapper001.h"

BoardInfo::BoardInfo(std::string boardName, int inesMapperNumber)
{
    _BoardName = boardName;
    _INESMapper = inesMapperNumber;
    _DefaultPRG_RAM_8KB_BanksCount = 1;
    _DefaultCHR_RAM_1KB_BanksCount = 8;
    _Enabled_ppuA12ToggleTimer = false;
    _PPUA12TogglesOnRaisingEdge = false;
}
BoardInfo::BoardInfo(std::string boardName, int inesMapperNumber, int defaultPRG_RAM_8KB_BanksCount, int defaultCHR_RAM_1KB_BanksCount)
{
    _BoardName = boardName;
    _INESMapper = inesMapperNumber;
    _DefaultPRG_RAM_8KB_BanksCount = defaultPRG_RAM_8KB_BanksCount;
    _DefaultCHR_RAM_1KB_BanksCount = defaultCHR_RAM_1KB_BanksCount;
    _Enabled_ppuA12ToggleTimer = false;
    _PPUA12TogglesOnRaisingEdge = false;
}
BoardInfo::BoardInfo(std::string boardName, int inesMapperNumber, bool Enabled_ppuA12ToggleTimer, bool PPUA12TogglesOnRaisingEdge)
{
    _BoardName = boardName;
    _INESMapper = inesMapperNumber;
    _DefaultPRG_RAM_8KB_BanksCount = 1;
    _DefaultCHR_RAM_1KB_BanksCount = 8;
    _Enabled_ppuA12ToggleTimer = Enabled_ppuA12ToggleTimer;
    _PPUA12TogglesOnRaisingEdge = PPUA12TogglesOnRaisingEdge;
}
BoardInfo::BoardInfo(std::string boardName, int inesMapperNumber, int defaultPRG_RAM_8KB_BanksCount, int defaultCHR_RAM_1KB_BanksCount, bool Enabled_ppuA12ToggleTimer, bool PPUA12TogglesOnRaisingEdge)
{
    _BoardName = boardName;
    _INESMapper = inesMapperNumber;
    _DefaultPRG_RAM_8KB_BanksCount = defaultPRG_RAM_8KB_BanksCount;
    _DefaultCHR_RAM_1KB_BanksCount = defaultCHR_RAM_1KB_BanksCount;
    _Enabled_ppuA12ToggleTimer = Enabled_ppuA12ToggleTimer;
    _PPUA12TogglesOnRaisingEdge = PPUA12TogglesOnRaisingEdge;
}

Board* Board::createClass(std::string className)
{
    Board *board = NULL;
    if(className == "Mapper000")
        board = new Mapper000;
    else if(className == "Mapper001")
        board = new Mapper001;
//    else if(className == "Mapper002")
//        board = new Mapper002;
//    else if(className == "Mapper003")
//        board = new Mapper003;
    else
        board = NULL;
    
    return board;
}

Board::Board()
{
    GameInfo = NULL;
    GameCartInfo = NULL;
    _nmt_banks = NULL;
    _nmt_indexes = NULL;
    _chr_banks = NULL;
    _chr_indexes = NULL;
    _chr_enable = NULL;
    _chr_isram = NULL;
    _chr_writable = NULL;
    _prg_banks = NULL;
    _prg_indexes = NULL;
    _prg_enable = NULL;
    _prg_isram = NULL;
    _prg_writable = NULL;
    _prg_battery = NULL;
    enable_external_sound = false;
    
    _nmt_banks = new unsigned char*[4];
    for(int i = 0; i < 4; i++)
    {
        /*Only 2 nmt banks should be used in normal state*/
        _nmt_banks[i] = new unsigned char[0x0400];
    }
    _nmt_indexes = new int[4];
    
    _prg_indexes = new int[5];
    
    _chr_indexes = new int[8];
}

Board::~Board()
{
    if(_nmt_banks != NULL)
    {
        for(int i = 0; i < 4; i++)
        {
            delete[] _nmt_banks[i];
        }
        delete[] _nmt_banks;
    }
    if( _nmt_indexes != NULL)
        delete[] _nmt_indexes;
    if( _prg_indexes != NULL )
        delete[] _prg_indexes;
    if (_chr_indexes != NULL)
        delete _chr_indexes;
}

void Board::switchNMT(unsigned char value)
{
    _nmt_indexes[0] = (unsigned char)(value >> 0 & 0x03);
    _nmt_indexes[1] = (unsigned char)(value >> 2 & 0x03);
    _nmt_indexes[2] = (unsigned char)(value >> 4 & 0x03);
    _nmt_indexes[3] = (unsigned char)(value >> 6 & 0x03);
}
void Board::switchNMT(Mirroring mirroring)
{
    switchNMT((unsigned char)mirroring);
}
void Board::GetPRGRAM()
{
    if ( IsGameFoundOnDB )
    {
        if (GameCartInfo->WRAMBanks.size() > 0)
        {
            int id = 0;
            for(std::list<SRAMBankInfo>::iterator it = GameCartInfo->WRAMBanks.begin(); it != GameCartInfo->WRAMBanks.end(); it++)
            {
                replace(it->SIZE, "k", "");
                int wsize = atoi( it->SIZE.c_str() );
                if( wsize == 0)
                {
                    // Unacceptable value, assume 8k
                    char buff[16];
                    sprintf(buff, "%d" , id);
                    BankInfo info( buff, true ,true, true, true, (unsigned char*)malloc(0x2000) ); //free
                    __prgBanks.push_back(info);
                    id++;
                }
                if( wsize >= 8 )
                {
                    for (int i = 0; i < wsize / 8; i++)
                    {
                        char buff[16];
                        sprintf(buff, "%d" , id);
                        BankInfo info( buff, true ,true, true, it->BATTERY, (unsigned char*)malloc(0x2000) ); //free
                        __prgBanks.push_back(info);
                        id++;
                    }
                }
                else
                {
                    // The value is under 8K, assume 8k ...
                    char buff[16];
                    sprintf(buff, "%d" , id);
                    BankInfo info( buff, true ,true, true, it->BATTERY, (unsigned char*)malloc(0x2000) ); //free
                    __prgBanks.push_back(info);
                }
            }
        }
        else
        {
            // No info for SRAM; This mean this rom has no sram !
        }
    }
    else
    {
        // Assume 8k
        for (int i = 0; i < _prg_ram_default_08KB_count; i++)
        {
            char buff[16];
            sprintf(buff, "%d" , i);
            BankInfo info( buff, true ,true, true, true, (unsigned char*)malloc(0x2000) );  //free
            __prgBanks.push_back(info);
        }
    }

    // Reverse sort by id
    __prgBanks.sort(BankInfo::Compare);
    
    // Calculate values
    _prg_ram_size_KB = __prgBanks.size() * 8;
    _prg_08K_ram_count = __prgBanks.size();
    _prg_16K_ram_count = __prgBanks.size() / 2;
    _prg_32K_ram_count = __prgBanks.size() / 4;
    _prg_08K_ram_mask = _prg_08K_ram_count - 1;
    _prg_16K_ram_mask = _prg_16K_ram_count - 1;
    _prg_32K_ram_mask = _prg_32K_ram_count - 1;
}

void Board::GetPRGROM(unsigned char* prg_dump, int prg_dump_len)
{
    _prg_rom_size_KB = prg_dump_len / 1024;
    _prg_08K_rom_count = _prg_rom_size_KB / 8;
    _prg_16K_rom_count = _prg_rom_size_KB / 16;
    _prg_32K_rom_count = _prg_rom_size_KB / 32;
    _prg_08K_rom_mask = _prg_08K_rom_count - 1;
    _prg_16K_rom_mask = _prg_16K_rom_count - 1;
    _prg_32K_rom_mask = _prg_32K_rom_count - 1;
    
    for(int i = 0; i < _prg_08K_rom_count; i++)
    {
        unsigned char* data = (unsigned char*)malloc(0x2000);   //free
        for(int j = 0; j < 0x2000; j++)
        {
            data[j] = prg_dump[(i * 0x2000) + j];
        }
        char buff[16];
        sprintf(buff, "%d" , i);
        BankInfo info(buff, false, false, true, false, data);
        __prgBanks.push_back(info);
    }
}

void Board::GetCHRRAM()
{
    if(IsGameFoundOnDB)
    {
        if ( GameCartInfo->VRAM_sizes.size() > 0)
        {
            bool ramAdded = false;
            for(std::list<std::string>::iterator it = GameCartInfo->VRAM_sizes.begin(); it != GameCartInfo->VRAM_sizes.end(); it++)
            {
                replace(*it, "k", "");
                int vsize = atoi( it->c_str() );
                for(int i = 0; i < vsize; i++)
                {
                    char buff[4];
                    sprintf(buff, "%d", i);
                    BankInfo info( buff, true, true, true, false, (unsigned char*)malloc(0x400) );
                    __chrBanks.push_back(info);
                    ramAdded = true;
                }
            }
            if (!ramAdded)
            {
                for (int i = 0; i < _chr_ram_1KB_default_banks_count; i++)
                {
                    char buff[4];
                    sprintf(buff, "%d", i);
                    BankInfo info( buff, true, true, true, false, (unsigned char*)malloc(0x400) );
                    __chrBanks.push_back(info);
                }
            }
        }
        else
        {
            for (int i = 0; i < _chr_ram_1KB_default_banks_count; i++)
            {
                char buff[4];
                sprintf(buff, "%d", i);
                BankInfo info( buff, true, true, true, false, (unsigned char*)malloc(0x400) );
                __chrBanks.push_back(info);
            }
        }
    }
    else
    {
        for (int i = 0; i < _chr_ram_1KB_default_banks_count; i++)
        {
            char buff[4];
            sprintf(buff, "%d", i);
            BankInfo info( buff, true, true, true, false, (unsigned char*)malloc(0x400) );
            __chrBanks.push_back(info);
        }
    }

    // Calculate
    _chr_01K_ram_count = __chrBanks.size();
    _chr_02K_ram_count = __chrBanks.size() / 2;
    _chr_04K_ram_count = __chrBanks.size() / 4;
    _chr_08K_ram_count = __chrBanks.size() / 8;
    _chr_01K_ram_mask = _chr_01K_ram_count - 1;
    _chr_02K_ram_mask = _chr_02K_ram_count - 1;
    _chr_04K_ram_mask = _chr_04K_ram_count - 1;
    _chr_08K_ram_mask = _chr_08K_ram_count - 1;
}

void Board::GetCHRROM(unsigned char* chr_dump, int chr_dump_len)
{
    _chr_01K_rom_count = chr_dump_len/1024;
    _chr_02K_rom_count = chr_dump_len/(1024 * 2);
    _chr_04K_rom_count = chr_dump_len/(1024 * 4);
    _chr_08K_rom_count = chr_dump_len/(1024 * 8);
    _chr_01K_rom_mask = _chr_01K_rom_count - 1;
    _chr_02K_rom_mask = _chr_02K_rom_count - 1;
    _chr_04K_rom_mask = _chr_04K_rom_count - 1;
    _chr_08K_rom_mask = _chr_08K_rom_count - 1;
    
    for(int i = 0; i < _chr_01K_rom_count; i++)
    {
        unsigned char *data = (unsigned char*)malloc(0x400);   //free
        for(int j = 0; j < 0x400; j++)
        {
            data[j] = chr_dump[(i * 0x400) + j];
        }
        char buff[16];
        sprintf(buff, "%d" , i);
        BankInfo info(buff, false, false, true, false, data);
        __chrBanks.push_back(info);
    }
}

void Board::initialize(std::string sha1, unsigned char* prg_dump, int prg_dump_len, unsigned char* chr_dump, int chr_dump_len,unsigned char* trainer_dump, int trainer_dump_len,Mirroring defaultMirroring)
{
    RomSHA1 = sha1;
    Chips = NULL;
    GameInfo = NULL;
    GameCartInfo = NULL;
    __prgBanks.clear();
    __chrBanks.clear();

    GameInfo = NesEmulator::__instance->nesCartDatabase->findRom(sha1);
    if(GameInfo == NULL)
        IsGameFoundOnDB = false;
    else
        IsGameFoundOnDB = true;
    if(IsGameFoundOnDB == true)
    {
        if(!GameInfo->Cartridges.empty())
        {
            for(std::list<NesCartDatabaseCartridgeElement>::iterator it = GameInfo->Cartridges.begin(); it != GameInfo->Cartridges.end(); it++)
            {
                std::string str = it->SHA1;
                if(toLower(&str) == toLower(&RomSHA1))
                {
                    GameCartInfo = &*it;
                    break;
                }
            }
        }
    }
    BoardType = "N/A";
    BoardPCB = "N/A";
    
    if (IsGameFoundOnDB)
    {
        BoardType = GameCartInfo->Board_Type;
        BoardPCB = GameCartInfo->Board_Pcb;
        if (!GameCartInfo->chip_type.empty())
        {
            Chips = &GameCartInfo->chip_type;
        }
    }

    _default_mirroring = defaultMirroring;
    for(int i = 0; i < 4; i++)
    {
        memset(_nmt_banks[i], 0, 0x0400);
    }
    memset(_nmt_indexes, 0, 4 *sizeof(int));
    
    switchNMT(defaultMirroring);
    
    /* prg __ramBanks recall free mem  */
    GetPRGRAM();
    _prg_rom_bank_offset = __prgBanks.size();
    GetPRGROM(prg_dump, prg_dump_len);
    _SRAMSaveRequired = false;
    
    _prg_banks = new unsigned char*[ __prgBanks.size() ];     //delete
    _prg_enable = new bool[ __prgBanks.size() ];              //delete
    _prg_writable = new bool[ __prgBanks.size() ];            //delete
    _prg_isram = new bool[ __prgBanks.size() ];               //delete
    _prg_battery = new bool[ __prgBanks.size() ];             //delete
    int j = 0;
    for(std::list<BankInfo>::iterator i = __prgBanks.begin(); i != __prgBanks.end(); i++)
    {
        _prg_banks[j] = i->DATA;
        _prg_enable[j] = i->Enabled;
        _prg_writable[j] = i->Writable;
        _prg_isram[j] = i->IsRAM;
        _prg_battery[j] = i->IsBattery;
        if(!_SRAMSaveRequired)
        {
            if( _prg_battery[j] )
                _SRAMSaveRequired = true;
        }
        j++;
    }
    memset(_prg_indexes, 0, 5 *sizeof(int));
    
    // Put trainer at first ram bank.
    if (trainer_dump != NULL && trainer_dump_len > 0)
    {
        for(int i = 0; i < __prgBanks.size(); i++)
        {
            if (_prg_isram[i])
            {
                memcpy(_prg_banks[i], trainer_dump, trainer_dump_len);
                break;
            }
        }
    }
    
    // CHR data
    GetCHRRAM();
    _chr_rom_bank_offset = __chrBanks.size();
    GetCHRROM(chr_dump, chr_dump_len);
    _chr_banks = new unsigned char*[ __chrBanks.size() ];  //delete
    _chr_enable = new bool[ __chrBanks.size() ];           //delete
    _chr_isram = new bool[ __chrBanks.size() ];            //delete
    _chr_writable = new bool[ __chrBanks.size() ];         //delete
    j = 0;
    for(std::list<BankInfo>::iterator i = __chrBanks.begin(); i != __chrBanks.end(); i++)
    {
        _chr_banks[j] = i->DATA;
        _chr_enable[j] = i->Enabled;
        _chr_isram[j] = i->IsRAM;
        _chr_writable[j] = i->Writable;
        j++;
    }
    memset(_chr_indexes, 0 ,8 *sizeof(int));
}

void Board::Switch08KPRG(int index, int where, bool isROM)
{
    if( !isROM && _prg_08K_ram_count == 0 )
        return;
    
    if(where == 0x6000)
    {
        if(isROM)
            _prg_indexes[0] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset;
        else
            _prg_indexes[0] = index & _prg_08K_ram_mask;
    }
    else
    {
        if (isROM)
            _prg_indexes[((where >> 13) & 0x3) + 1] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset;
        else
            _prg_indexes[((where >> 13) & 0x3) + 1] = index & _prg_08K_ram_mask;
    }
}

void Board::Switch16KPRG(int index, int where, bool isROM)
{
    if (!isROM && _prg_08K_ram_count == 0) return;// Nothing to do if there's no ram
    where >>= 13;
    where &= 3;
    where++;
    
    index <<= 1;
    if (isROM)
    {
        _prg_indexes[where] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset; index++; where++;
        _prg_indexes[where] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset;
    }
    else
    {
        _prg_indexes[where] = index & _prg_08K_ram_mask; index++; where++;
        _prg_indexes[where] = index & _prg_08K_ram_mask;
    }
}

void Board::Switch32KPRG(int index, bool isROM)
{
    if ( !isROM && _prg_08K_ram_count == 0 )
        return;
    index <<= 2;
    if (isROM)
    {
        _prg_indexes[1] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset;
        index++;
        _prg_indexes[2] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset;
        index++;
        _prg_indexes[3] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset;
        index++;
        _prg_indexes[4] = (index & _prg_08K_rom_mask) + _prg_rom_bank_offset;
    }
    else
    {
        _prg_indexes[1] = index & _prg_08K_ram_mask;
        index++;
        _prg_indexes[2] = index & _prg_08K_ram_mask;
        index++;
        _prg_indexes[3] = index & _prg_08K_ram_mask;
        index++;
        _prg_indexes[4] = index & _prg_08K_ram_mask;
    }
}

void Board::TogglePRGRAMEnable(bool enable)
{
    for(int i = 0; i < __prgBanks.size(); i++)
    {
        if(_prg_isram[i])
        {
            _prg_enable[i] = enable;
        }
    }
}

void Board::Switch01KCHR(int index, int where, bool isROM)
{
    if (!isROM && _chr_01K_ram_count == 0)
        return;
    if (isROM)
        _chr_indexes[(where >> 10) & 0x07] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
    else
        _chr_indexes[(where >> 10) & 0x07] = index & _chr_01K_ram_mask;
}

void Board::Switch02KCHR(int index, int where, bool isROM)
{
    if (!isROM && _chr_01K_ram_count == 0)
        return;
    where >>= 10;
    where &= 0x7;
    index <<= 1;
    if (isROM)
    {
        _chr_indexes[where] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++; where++;
        _chr_indexes[where] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
    }
    else
    {
        _chr_indexes[where] = index & _chr_01K_ram_mask;
        index++; where++;
        _chr_indexes[where] = index & _chr_01K_ram_mask;
    }
}

void Board::Switch04KCHR(int index, int where, bool isROM)
{
    if (!isROM && _chr_01K_ram_count == 0)
        return;
    where >>= 10;
    where &= 0x7;
    index <<= 2;
    if (isROM)
    {
        _chr_indexes[where] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++; where++;
        _chr_indexes[where] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++; where++;
        _chr_indexes[where] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++; where++;
        _chr_indexes[where] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
    }
    else
    {
        _chr_indexes[where] = index & _chr_01K_ram_mask;
        index++; where++;
        _chr_indexes[where] = index & _chr_01K_ram_mask;
        index++; where++;
        _chr_indexes[where] = index & _chr_01K_ram_mask;
        index++; where++;
        _chr_indexes[where] = index & _chr_01K_ram_mask;
    }
}

void Board::Switch08KCHR(int index, bool isROM)
{
    if (!isROM && _chr_01K_ram_count == 0)
        return;
    index <<= 3;
    
    if (isROM)
    {
        _chr_indexes[0] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++;
        _chr_indexes[1] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++;
        _chr_indexes[2] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++;
        _chr_indexes[3] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++;
        _chr_indexes[4] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++;
        _chr_indexes[5] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++;
        _chr_indexes[6] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
        index++;
        _chr_indexes[7] = (index & _chr_01K_rom_mask) + _chr_rom_bank_offset;
    }
    else
    {
        _chr_indexes[0] = index & _chr_01K_ram_mask;
        index++;
        _chr_indexes[1] = index & _chr_01K_ram_mask;
        index++;
        _chr_indexes[2] = index & _chr_01K_ram_mask;
        index++;
        _chr_indexes[3] = index & _chr_01K_ram_mask;
        index++;
        _chr_indexes[4] = index & _chr_01K_ram_mask;
        index++;
        _chr_indexes[5] = index & _chr_01K_ram_mask;
        index++;
        _chr_indexes[6] = index & _chr_01K_ram_mask;
        index++;
        _chr_indexes[7] = index & _chr_01K_ram_mask;
    }
}

void Board::TogglePRGRAMWritableEnable(bool enable)
{
    for (int i = 0; i < __prgBanks.size(); i++)
    {
        if (_prg_isram[i])
            _prg_writable[i] = enable;
    }
}

void Board::ToggleCHRRAMEnable(bool enable)
{
    for (int i = 0; i < __prgBanks.size(); i++)
    {
        if (_chr_isram[i])
            _chr_enable[i] = enable;
    }
}

void Board::ToggleCHRRAMWritableEnable(bool enable)
{
    for (int i = 0; i < __prgBanks.size(); i++)
    {
        if (_chr_isram[i])
            _chr_writable[i] = enable;
    }
}

void Board::hardReset()
{
    Switch08KPRG(0, 0x6000, false);
    Switch32KPRG(0, true);
    TogglePRGRAMEnable(true);
    
    Switch08KCHR(0, _chr_01K_rom_count > 0);
    
    for(int i = 0; i < 4; i++)
    {
        memset(_nmt_banks[i], 0, 0x0400);
    }
    memset(_nmt_indexes, 0, sizeof(int)*4);
    
    switchNMT(_default_mirroring);
}

void Board::SoftReset()
{
    
}

unsigned char Board::ReadEXP(int address)
{
    return 0;
}
void Board::WriteEXP(int address,unsigned char data)
{
    
}

unsigned char Board::ReadSRM(int address)
{
    if (_prg_enable[_prg_indexes[0]])
        return _prg_banks[_prg_indexes[0]][address & 0x1FFF];
    return 0;
}
void Board::WriteSRM(int address,unsigned char data)
{
    if (_prg_enable[_prg_indexes[0]])
    {
        if (_prg_isram[_prg_indexes[0]])
        {
            if (_prg_writable[_prg_indexes[0]])
                _prg_banks[_prg_indexes[0]][address & 0x1FFF] = data;
        }
    }
}

unsigned char Board::ReadPRG(int address)
{
    return _prg_banks[_prg_indexes[((address >> 13) & 3) + 1]][address & 0x1FFF];
}
void Board::WritePRG(int address, unsigned char data)
{
    _prg_bank_index_temp = _prg_indexes[((address >> 13) & 3) + 1];
    if (_prg_enable[_prg_bank_index_temp])
    {
        if (_prg_isram[_prg_bank_index_temp])
        {
            if (_prg_writable[_prg_bank_index_temp])
                _prg_banks[_prg_bank_index_temp][address & 0x1FFF] = data;
        }
    }
}

unsigned char Board::ReadCHR(int address, bool spriteFetch)
{
    return _chr_banks[_chr_indexes[(address >> 10) & 7]][address & 0x3FF];
}

void Board::WriteCHR(int address, unsigned char data)
{
    _chr_bank_index_temp = _chr_indexes[(address >> 10) & 7];
    if (_chr_enable[_chr_bank_index_temp])
    {
        if (_chr_isram[_chr_bank_index_temp])
        {
            if (_chr_writable[_chr_bank_index_temp])
                _chr_banks[_chr_bank_index_temp][address & 0x3FF] = data;
        }
    }
}

unsigned char Board::ReadNMT(int address)
{
    return _nmt_banks[_nmt_indexes[(address >> 10) & 3]][address & 0x3FF];
}

void Board::WriteNMT(int address, unsigned char data)
{
    _nmt_banks[_nmt_indexes[(address >> 10) & 3]][address & 0x3FF] = data;
}

void Board::OnPPUA12RaisingEdge()
{
    
}

void Board::OnPPUAddressUpdate(int address)
{
    if (_enabled_ppuA12ToggleTimer)
    {
        _old_vram_address = _new_vram_address;
        _new_vram_address = address & 0x1000;
        if (_ppuA12TogglesOnRaisingEdge)
        {
            if (_old_vram_address < _new_vram_address)
            {
                if (_ppu_cycles_timer > 8)
                {
                    OnPPUA12RaisingEdge();
                }
                _ppu_cycles_timer = 0;
            }
        }
        else
        {
            if (_old_vram_address > _new_vram_address)
            {
                if (_ppu_cycles_timer > 8)
                {
                    OnPPUA12RaisingEdge();
                }
                _ppu_cycles_timer = 0;
            }
        }
    }
}

void Board::OnCPUClock()
{
    
}

void Board::OnPPUClock()
{
    if (_enabled_ppuA12ToggleTimer)
        _ppu_cycles_timer++;
}

void Board::OnPPUScanlineTick()
{
    
}

void Board::OnAPUClockDuration()
{
    
}

void Board::OnAPUClockEnvelope()
{
    
}

void Board::OnAPUClockSingle(bool isClockingLength)
{
    
}

double Board::APUGetSamples()
{
    return 0;
}

void Board::SaveSRAM()
{

}

/// Get prg ram banks (all togather) that marked as battery as one buffer.
unsigned char* Board::GetSRAMBuffer()
{
    return NULL;
}

void Board::LoadSRAM()
{
    
}

void Board::LoadSRAM(int t)
{
    
}

void Board::SaveState()
{
    
}

void Board::LoadState()
{
    
}

void Board::VSUnisystem4016RW(unsigned char data)
{
    if ((data & 0x4) == 0x4)
        Switch08KCHR(1, _chr_01K_rom_count > 0);
    else
        Switch08KCHR(0, _chr_01K_rom_count > 0);
}
