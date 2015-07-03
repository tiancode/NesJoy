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

#include "Mapper001.h"
#include "util.h"

Mapper001::Mapper001()
{
    BoardInfo info("MMC1", 1, 4, 64);
    
    Board::Name = info._BoardName;
    Board::MapperNumber = info._INESMapper;
    Board::_prg_ram_default_08KB_count = info._DefaultPRG_RAM_8KB_BanksCount;
    Board::_chr_ram_1KB_default_banks_count = info._DefaultCHR_RAM_1KB_BanksCount;
    Board::_enabled_ppuA12ToggleTimer = info._Enabled_ppuA12ToggleTimer;
    Board::_ppuA12TogglesOnRaisingEdge = info._PPUA12TogglesOnRaisingEdge;
    
    shift = 0;
    buffer = 0;
    reg = new unsigned char[4];
    memset(reg, 0, 4);
}

void Mapper001::hardReset()
{
    Board::hardReset();
    cpuCycles = 0;
    // Registers
    address_reg = 0;
    memset(reg, 0, 4);
    reg[0] = 0x0C;
    flag_c = false;
    flag_s = flag_p = true;
    prg_hijackedbit = 0;
    reg[1] = reg[2] = reg[3] = 0;
    // Buffers
    buffer = 0;
    shift = 0;
    
    if( contain(Chips, "MMC1B") || contain(Chips, "MMC1B2") )
    {
        TogglePRGRAMEnable(false);
    }
    
    enable_wram_enable = !contain(Chips, "MMC1A");

    //  use hijacked
    use_hijacked = (_prg_16K_rom_mask & 0x10) == 0x10;
    if (use_hijacked)
        prg_hijackedbit = 0x10;
    // SRAM Switch ?
    use_sram_switch = false;
    if (_prg_08K_ram_count > 0)
    {
        use_sram_switch = true;
        sram_switch_mask = use_hijacked ? 0x08 : 0x18;
        sram_switch_mask &= _prg_08K_ram_mask << 3;
        
        if (sram_switch_mask == 0)
            use_sram_switch = false;
    }
    Switch16KPRG(0xF | prg_hijackedbit, 0xC000, true);
}

void Mapper001::WritePRG(int address, unsigned char value)
{
    // Too close writes ignored !
    if (cpuCycles > 0)
    {
        return;
    }
    cpuCycles = 3;// Make save cycles ...
    //Temporary reg port ($8000-FFFF):
    //[r... ...d]
    //r = reset flag
    //d = data bit
    
    //r is set
    if ((value & 0x80) == 0x80)
    {
        reg[0] |= 0x0C;//bits 2,3 of reg $8000 are set (16k PRG mode, $8000 swappable)
        flag_s = flag_p = true;
        shift = buffer = 0;//hidden temporary reg is reset
        return;
    }
    //d is set
    if ((value & 0x01) == 0x01)
        buffer |= (unsigned char)(1 << shift);//'d' proceeds as the next bit written in the 5-bit sequence
    if (++shift < 5)
        return;
    // If this completes the 5-bit sequence:
    // - temporary reg is copied to actual internal reg (which reg depends on the last address written to)
    address_reg = (address & 0x7FFF) >> 13;
    reg[address_reg] = buffer;
    
    // - temporary reg is reset (so that next write is the "first" write)
    shift = buffer = 0;
    
    // Update internal registers ...
    switch (address_reg)
    {
        case 0:// $8000-9FFF [Flags and mirroring]
        {
            // Flags
            flag_c = (reg[0] & 0x10) != 0;
            flag_p = (reg[0] & 0x08) != 0;
            flag_s = (reg[0] & 0x04) != 0;
            UpdatePRG();
            UpdateCHR();
            // Mirroring
            switch (reg[0] & 3)
            {
                case 0: switchNMT(OneScA); break;
                case 1: switchNMT(OneScB); break;
                case 2: switchNMT(Vert); break;
                case 3: switchNMT(Horz); break;
            }
            break;
        }
        case 1:// $A000-BFFF [CHR REG 0]
        {
            // CHR
            if (!flag_c)
                Switch08KCHR(reg[1] >> 1, _chr_01K_rom_count > 0);
            else
                Switch04KCHR(reg[1], 0x0000, _chr_01K_rom_count > 0);
            // SRAM
            if (use_sram_switch)
                Switch08KPRG((reg[1] & sram_switch_mask) >> 3, 0x6000, false);
            // PRG hijack
            if (use_hijacked)
            { prg_hijackedbit = reg[1] & 0x10; UpdatePRG(); }
            break;
        }
        case 2:// $C000-DFFF [CHR REG 1]
        {
            // CHR
            if (flag_c)
                Switch04KCHR(reg[2], 0x1000, _chr_01K_rom_count > 0);
            // SRAM
            if (use_sram_switch)
                Switch08KPRG((reg[2] & sram_switch_mask) >> 3, 0x6000, false);
            // PRG hijack
            if (use_hijacked)
            { prg_hijackedbit = reg[2] & 0x10; UpdatePRG(); }
            break;
        }
        case 3:// $E000-FFFF [PRG REG]
        {
            if (enable_wram_enable)
                TogglePRGRAMEnable((reg[3] & 0x10) == 0);
            UpdatePRG();
            break;
        }
    }
}

void Mapper001::OnCPUClock()
{
    if (cpuCycles > 0)
        cpuCycles--;
}

void Mapper001::UpdateCHR()
{
    if (!flag_c)
        Switch08KCHR(reg[1] >> 1, _chr_01K_rom_count > 0);
    else
    {
        Switch04KCHR(reg[1], 0x0000, _chr_01K_rom_count > 0);
        Switch04KCHR(reg[2], 0x1000, _chr_01K_rom_count > 0);
    }
    // SRAM
    if (use_sram_switch)
        Switch08KPRG((reg[1] & sram_switch_mask) >> 3, 0x6000, false);
}

void Mapper001::UpdatePRG()
{
    
    if (!flag_p)
    {
        Switch32KPRG(((reg[3] & 0xF) | prg_hijackedbit) >> 1, true);
    }
    else
    {
        if (flag_s)
        {
            Switch16KPRG((reg[3] & 0xF) | prg_hijackedbit, 0x8000, true);
            Switch16KPRG(0xF | prg_hijackedbit, 0xC000, true);
        }
        else
        {
            Switch16KPRG(prg_hijackedbit, 0x8000, true);
            Switch16KPRG((reg[3] & 0xF) | prg_hijackedbit, 0xC000, true);
        }
    }
}
