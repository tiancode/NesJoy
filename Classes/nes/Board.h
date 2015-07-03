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

#ifndef __NesEmulator__Board__
#define __NesEmulator__Board__

#include "Types.h"
#include <stdlib.h>
#include <string>
#include <list>
#include "NesCartDatabase.h"

class BoardInfo
{
public:
    BoardInfo(std::string boardName, int inesMapperNumber);
    BoardInfo(std::string boardName, int inesMapperNumber, int defaultPRG_RAM_8KB_BanksCount, int defaultCHR_RAM_1KB_BanksCount);
    BoardInfo(std::string boardName, int inesMapperNumber, bool Enabled_ppuA12ToggleTimer, bool PPUA12TogglesOnRaisingEdge);
    BoardInfo(std::string boardName, int inesMapperNumber, int defaultPRG_RAM_8KB_BanksCount, int defaultCHR_RAM_1KB_BanksCount, bool Enabled_ppuA12ToggleTimer, bool PPUA12TogglesOnRaisingEdge);
    
    std::string _BoardName;
    int _INESMapper;
    int _DefaultPRG_RAM_8KB_BanksCount;
    int _DefaultCHR_RAM_1KB_BanksCount;
    bool _Enabled_ppuA12ToggleTimer;
    bool _PPUA12TogglesOnRaisingEdge;
};

struct BankInfo
{
    BankInfo(std::string ID, bool IsRAM, bool Writable, bool Enabled, bool IsBattery, unsigned char* DATA)
    {
        this->ID = ID;
        this->IsRAM = IsRAM;
        this->Writable = Writable;
        this->Enabled = Enabled;
        this->DATA = DATA;
        this->IsBattery = IsBattery;
    }
    
    bool IsRAM;
    bool Enabled;
    bool Writable;
    bool IsBattery;
    std::string ID;
    unsigned char *DATA;
    
    static int Compare(BankInfo x, BankInfo y)
    {
        int x_val = 0;
        int y_val = 0;
        x_val = atoi(x.ID.c_str());
        y_val = atoi(y.ID.c_str());
        return y_val - x_val;
    }
};

class Board
{
public:
public:
    static Board* createClass(std::string className);
    
    Board();
    ~Board();
    
public:
    /* Initialize the board */
    virtual void initialize(std::string sha1, unsigned char* prg_dump, int prg_dump_len, unsigned char* chr_dump, int chr_dump_len,unsigned char* trainer_dump, int trainer_dump_len,Mirroring defaultMirroring);
    
    virtual void hardReset();
    virtual void SoftReset();
    
    virtual unsigned char ReadEXP(int address);
    virtual void WriteEXP(int address, unsigned char data);
    virtual unsigned char ReadSRM(int address);
    virtual void WriteSRM(int address, unsigned char data);
    virtual unsigned char ReadPRG(int address);
    virtual void WritePRG(int address, unsigned char data);
    
    virtual unsigned char ReadCHR(int address, bool spriteFetch);
    virtual void WriteCHR(int address, unsigned char data);
    
    virtual unsigned char ReadNMT(int address);
    virtual void WriteNMT(int address, unsigned char data);
    
    virtual void OnPPUA12RaisingEdge();
    virtual void OnPPUAddressUpdate(int address);
    
    virtual void OnCPUClock();
    virtual void OnPPUClock();
    virtual void OnPPUScanlineTick();
    virtual void OnAPUClockDuration();
    virtual void OnAPUClockEnvelope();
    virtual void OnAPUClockSingle(bool isClockingLength);
    virtual double APUGetSamples();
    
    virtual void VSUnisystem4016RW(unsigned char data);
    
    void SaveSRAM();
    unsigned char* GetSRAMBuffer();
    void LoadSRAM();
    void LoadSRAM(int t);
    virtual void SaveState();
    virtual void LoadState();
protected:
    void switchNMT(unsigned char value);
    void switchNMT(Mirroring mirroring);
    void GetPRGRAM();
    void GetPRGROM(unsigned char* prg_dump, int prg_dump_len);
    void GetCHRRAM();
    void GetCHRROM(unsigned char* chr_dump, int chr_dump_len);
    
    void Switch08KPRG(int index, int where, bool isROM);
    void Switch16KPRG(int index, int where, bool isROM);
    void Switch32KPRG(int index, bool isROM);
    
    void TogglePRGRAMEnable(bool enable);
    void TogglePRGRAMWritableEnable(bool enable);
    void ToggleCHRRAMEnable(bool enable);
    void ToggleCHRRAMWritableEnable(bool enable);
    
    void Switch01KCHR(int index, int where, bool isROM);
    void Switch02KCHR(int index, int where, bool isROM);
    void Switch04KCHR(int index, int where, bool isROM);
    void Switch08KCHR(int index, bool isROM);
    
protected:
    /* Board info properties*/
    std::string Name;   /* BoardInfo */
    int MapperNumber;   /* BoardInfo */
    std::string BoardType;
    std::string BoardPCB;
    std::list<std::string> *Chips;
    /* bool Supported;
     bool NotImplementedWell;
     std::string Issues; */
    std::string RomSHA1;
public:
     bool IsGameFoundOnDB;
     NesCartDatabaseGameElement *GameInfo;
     NesCartDatabaseCartridgeElement *GameCartInfo;
    
    bool enable_external_sound;
protected:
    /* Clocks*/
    bool _enabled_ppuA12ToggleTimer;  /* BoardInfo */
    bool _ppuA12TogglesOnRaisingEdge; /* BoardInfo */
    int _old_vram_address;
    int _new_vram_address;
    int _ppu_cycles_timer;
    
    /* CHR */
    unsigned char** _chr_banks;
    int *_chr_indexes;
    bool *_chr_enable;
    bool *_chr_isram;
    bool *_chr_writable;
    int _chr_rom_bank_offset;
    int _chr_01K_rom_count;
    int _chr_02K_rom_count;
    int _chr_04K_rom_count;
    int _chr_08K_rom_count;
    int _chr_01K_rom_mask;
    int _chr_02K_rom_mask;
    int _chr_04K_rom_mask;
    int _chr_08K_rom_mask;
    int _chr_ram_1KB_default_banks_count;  /* BoardInfo */
    int _chr_01K_ram_count;
    int _chr_02K_ram_count;
    int _chr_04K_ram_count;
    int _chr_08K_ram_count;
    int _chr_01K_ram_mask;
    int _chr_02K_ram_mask;
    int _chr_04K_ram_mask;
    int _chr_08K_ram_mask;
    int _chr_bank_index_temp;
    
    std::list<BankInfo> __chrBanks;
    /* NMT */
    unsigned char **_nmt_banks;   /* initialize */
    int *_nmt_indexes;   /* initialize */
    Mirroring _default_mirroring;   /* initialize */
    
protected:
    /* PRG  Indicates if the engine should save sram. */
    bool _SRAMSaveRequired;
    unsigned char** _prg_banks;
    int* _prg_indexes;
    bool* _prg_enable;
    bool* _prg_isram;
    bool* _prg_writable;
    bool* _prg_battery;
    int _prg_rom_bank_offset;  /* initialize */
    int _prg_rom_size_KB;
    int _prg_08K_rom_count;
    int _prg_16K_rom_count;
    int _prg_32K_rom_count;
    int _prg_08K_rom_mask;
    int _prg_16K_rom_mask;
    int _prg_32K_rom_mask;
    // Default ram size
    int _prg_ram_default_08KB_count;  /* BoardInfo */
    int _prg_ram_size_KB;     /* GetPRGRAM */
    int _prg_08K_ram_count;   /* GetPRGRAM */
    int _prg_16K_ram_count;   /* GetPRGRAM */
    int _prg_32K_ram_count;   /* GetPRGRAM */
    int _prg_08K_ram_mask;    /* GetPRGRAM */
    int _prg_16K_ram_mask;    /* GetPRGRAM */
    int _prg_32K_ram_mask;    /* GetPRGRAM */
    int _prg_bank_index_temp;
    
    std::list<BankInfo> __prgBanks;
};


#endif /* defined(__NesEmulator__Board__) */
