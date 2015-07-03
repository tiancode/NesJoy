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

#include "Memory.h"
#include "NesEmulator.h"
#include "PPU.h"
#include "Interrupt.h"
#include "APU.h"
#include "DMC.h"
#include "Pulse1.h"
#include "Pulse2.h"
#include "Noise.h"
#include "Triangle.h"

#include "Input.h"
#include "VSUnisystem.h"
#include "DMA.h"

Memory* Memory::__instance = NULL;

Memory* Memory::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new Memory;
    }
    return __instance;
}

void Memory::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
        __instance = NULL;
    }
}

Memory::Memory()
{
    ram = (unsigned char*)malloc(0x800);
    memset(palettes_bank, 0, sizeof(palettes_bank));
    oam_ram = (unsigned char*)malloc(256);
    oam_secondary = (unsigned char*)malloc(32);
}
Memory::~Memory()
{
    if(ram != NULL)
        free(ram);
    if(oam_ram != NULL)
        free(oam_ram);
    if( oam_secondary != NULL )
        free(oam_secondary);
}

bool Memory::init(Rom *rom)
{
    char buff[4];
    sprintf(buff, "%03d", rom->getMapperNumber() );
    std::string className = std::string("Mapper") + buff;
    board = Board::createClass(className);
    if(board != NULL)
    {
        board->initialize(rom->getSHA1(), rom->getPRG(), rom->getPRGSize(), rom->getCHR(), rom->getCHRSize(), rom->getTrainer(), rom->getTrainerSize(), rom->getMirroring());
    }
    else
    {
        printf("mapper not supported\n");
        return false;
    }
    
    return true;
}

void Memory::hardReset()
{
    memset(ram, 0, 0x800);
    ram[0x0008] = 0xF7;
    ram[0x0009] = 0xEF;
    ram[0x000A] = 0xDF;
    ram[0x000F] = 0xBF;
    char palettes_bank_temp[] =  // Miscellaneous, real NES loads values similar to these during power up
    {
        0x09, 0x01, 0x00, 0x01, 0x00, 0x02, 0x02, 0x0D, 0x08, 0x10, 0x08, 0x24, 0x00, 0x00, 0x04, 0x2C, // Bkg palette
        0x09, 0x01, 0x34, 0x03, 0x00, 0x04, 0x00, 0x14, 0x08, 0x3A, 0x00, 0x02, 0x00, 0x20, 0x2C, 0x08  // Spr palette
    };
    memcpy(palettes_bank, palettes_bank_temp, sizeof(palettes_bank_temp));
    
    memset(oam_ram, 0, 256);
    memset(oam_secondary, 0, 32);
    
    BUS_ADDRESS = 0;
    BUS_RW = false;
    BUS_RW_P = false;
    
    // SRAM 加载保存的游戏进度
//    if(cocos2d::FileUtils::getInstance()->isFileExist(NesEmulator::getInstance()->SRAMFileName))
//    {
//        printf("file exist\n");
//        
//    }
//    else
//    {
//        printf("SRAM file not found; rom has no SRAM or file not exist.\n");
//    }
    board->hardReset();
}

unsigned char Memory::read(int address)
{
    BUS_RW_P = BUS_RW;
    BUS_ADDRESS = address;
    BUS_RW = true;
    
    NesEmulator::__instance->syncClock();
    
    // Cartridge PRG-ROM Area 32K
    if ( address >= 0x8000 && address <= 0xFFFF)
    {
        //printf("R_PRG-ROMM\n");
        return board->ReadPRG(address);
    }
    // Internal 2K Work RAM (mirrored to 800h-1FFFh)
    else if (address < 0x2000)
    {
        //printf("R_ram\n");
        return ram[address & 0x7FF];
    }
     // Internal PPU Registers (mirrored to 2008h-3FFFh)
    else if (address < 0x4000)
    {
        //printf("R_ppu\n");
        switch (address & 7)
        {
            case 2:// $2002
            {
                PPU::__instance->ppu_2002_temp = 0;
                
                if (Interrupt::__instance->vbl_flag)
                    PPU::__instance->ppu_2002_temp |= 0x80;
                if (PPU::__instance->spr_0Hit)
                    PPU::__instance->ppu_2002_temp |= 0x40;
                if (PPU::__instance->spr_overflow)
                    PPU::__instance->ppu_2002_temp |= 0x20;
                
                Interrupt::__instance->vbl_flag_temp = false;
                PPU::__instance->vram_flipflop = false;
                
                // NMI disable effect only at vbl set period (HClock between 1 and 3)
                if ((PPU::__instance->VClock == PPU::__instance->vbl_vclock_Start) && (PPU::__instance->HClock <= 3))
                    Interrupt::__instance->NMI_Current = (Interrupt::__instance->vbl_flag_temp & Interrupt::__instance->nmi_enabled);
                
                return PPU::__instance->ppu_2002_temp;
            }
            case 4:// $2004
            {
                PPU::__instance->ppu_2004_temp = oam_ram[PPU::__instance->oam_address];
                if (PPU::__instance->VClock < 240 && PPU::__instance->IsRenderingOn())
                {
                    if (PPU::__instance->HClock < 64)
                        PPU::__instance->ppu_2004_temp = 0xFF;
                    else if (PPU::__instance->HClock < 192)
                        PPU::__instance->ppu_2004_temp = oam_ram[((PPU::__instance->HClock - 64) << 1) & 0xFC];
                    else if (PPU::__instance->HClock < 256)
                        PPU::__instance->ppu_2004_temp = ((PPU::__instance->HClock & 1) == 1) ? oam_ram[0xFC] : oam_ram[((PPU::__instance->HClock - 192) << 1) & 0xFC];
                    else if (PPU::__instance->HClock < 320)
                        PPU::__instance->ppu_2004_temp = 0xFF;
                    else
                        PPU::__instance->ppu_2004_temp = oam_ram[0];
                }
                return PPU::__instance->ppu_2004_temp;
            }
            case 7:// $2007
            {
                PPU::__instance->ppu_2007_temp = 0;
                
                if ((PPU::__instance->vram_address & 0x3F00) == 0x3F00)
                {
                    // The return value should be from the palettes bank
                    PPU::__instance->ppu_2007_temp = (unsigned char)(palettes_bank[PPU::__instance->vram_address & ((PPU::__instance->PPU::__instance->vram_address & 0x03) == 0 ? 0x0C : 0x1F)] & PPU::__instance->grayscale);
                    // fill buffer from chr or nametables
                    PPU::__instance->vram_address_temp_access1 = PPU::__instance->vram_address & 0x2FFF;
                    if (PPU::__instance->vram_address_temp_access1 < 0x2000)
                    {
                        PPU::__instance->reg2007buffer = Memory::__instance->board->ReadCHR( PPU::__instance->vram_address_temp_access1, false);
                        
                    }
                    else
                    {
                        PPU::__instance->reg2007buffer = Memory::__instance->board->ReadNMT(PPU::__instance->vram_address_temp_access1);
                    }
                }
                else
                {
                    PPU::__instance->ppu_2007_temp = PPU::__instance->reg2007buffer;
                    // fill buffer
                    PPU::__instance->vram_address_temp_access1 = PPU::__instance->vram_address & 0x3FFF;
                    if (PPU::__instance->vram_address_temp_access1 < 0x2000)
                    {
                        PPU::__instance->reg2007buffer = Memory::__instance->board->ReadCHR(PPU::__instance->vram_address_temp_access1, false);
                    }
                    else if (PPU::__instance->vram_address_temp_access1 < 0x3F00)
                    {
                        PPU::__instance->reg2007buffer = Memory::__instance->board->ReadNMT(PPU::__instance->vram_address_temp_access1);
                    }
                    else
                    {
                        PPU::__instance->reg2007buffer = palettes_bank[PPU::__instance->vram_address_temp_access1 & ((PPU::__instance->vram_address_temp_access1 & 0x03) == 0 ? 0x0C : 0x1F)];
                    }
                }
                
                PPU::__instance->vram_address = (PPU::__instance->vram_address + PPU::__instance->vram_increament) & 0x7FFF;
                Memory::__instance->board->OnPPUAddressUpdate(PPU::__instance->vram_address);
                return PPU::__instance->ppu_2007_temp;
            }
        }
    }
    // Internal APU Registers
    else if (address < 0x4020)
    {
        switch (address)
        {
            case 0x4015:
            {
                temp_4015 = 0;
                // Channels enable
                if (Pulse1::__instance->sq1_duration_counter > 0) temp_4015 |= 0x01;
                if (Pulse2::__instance->sq2_duration_counter > 0) temp_4015 |= 0x02;
                if (Triangle::__instance->trl_duration_counter > 0) temp_4015 |= 0x04;
                if (Noise::__instance->noz_duration_counter > 0) temp_4015 |= 0x08;
                if (DMC::__instance->dmc_dmaSize > 0) temp_4015 |= 0x10;
                // IRQ
                if (APU::__instance->FrameIrqFlag) temp_4015 |= 0x40;
                if (DMC::__instance->DeltaIrqOccur) temp_4015 |= 0x80;
                
                APU::__instance->FrameIrqFlag = false;
                Interrupt::__instance->IRQFlags &= ~Interrupt::__instance->IRQ_APU;
                
                return temp_4015;
            }
            case 0x4016:
            {
                temp_4016 = (unsigned char)(Input::__instance->PORT0 & 1);
                
                Input::__instance->PORT0 >>= 1;
                
                if (Input::__instance->IsZapperConnected)
                    temp_4016 |= Input::__instance->zapper->GetData();
                
                if (VSUnisystem::IsVSUnisystem)
                    temp_4016 |= Input::__instance->VSUnisystemDIP->GetData4016();
                
                return temp_4016;
            }
            case 0x4017:
            {
                temp_4017 = (unsigned char)(Input::__instance->PORT1 & 1);
                
                Input::__instance->PORT1 >>= 1;
                
                if (Input::__instance->IsZapperConnected)
                    temp_4017 |= Input::__instance->zapper->GetData();
                if (VSUnisystem::IsVSUnisystem)
                    temp_4017 |= Input::__instance->VSUnisystemDIP->GetData4017();
                
                return temp_4017;
            }
        }
    }
    // Cartridge Expansion Area almost 8K
    else if(address < 0x6000)
    {
        //printf("R_Expansion\n");
        return board->ReadEXP(address);
    }
    // Cartridge SRAM Area 8K
    else if(address < 0x8000)
    {
        //printf("R_Cartridge SRAM\n");
        return board->ReadSRM(address);
    }
    
    //assert(false);
    return 0;
}

extern unsigned char DurationTable[];
extern int NozFrequencyTable[][16];
extern int SequenceMode0[][7];
extern int SequenceMode1[][5];
void Memory::write(int address, unsigned char value)
{
    BUS_RW_P = BUS_RW;
    BUS_ADDRESS = address;
    BUS_RW = false;
    
    NesEmulator::__instance->syncClock();
    
    // Internal 2K Work RAM (mirrored to 800h-1FFFh)
    if (address < 0x2000)
    {
        //printf("W_ram\n");
        ram[address & 0x7FF] = value;
    }
    // Internal PPU Registers (mirrored to 2008h-3FFFh)
    else if (address < 0x4000)
    {
        //printf("W_ppu\n");
        switch (address & 7)
        {
            case 0:// $2000
            {
                PPU::__instance->vram_temp = (PPU::__instance->vram_temp & 0x73FF) | ((value & 0x3) << 10);
                PPU::__instance->vram_increament = ((value & 0x4) != 0) ? 32 : 1;
                PPU::__instance->spr_patternAddress = ((value & 0x8) != 0) ? 0x1000 : 0x0000;
                PPU::__instance->bkg_patternAddress = ((value & 0x10) != 0) ? 0x1000 : 0x0000;
                PPU::__instance->spr_size16 = (value & 0x20) != 0 ? 0x0010 : 0x0008;
                
                Interrupt::__instance->nmi_old = Interrupt::__instance->nmi_enabled;
                Interrupt::__instance->nmi_enabled = (value & 0x80) != 0;
                
                if (!Interrupt::__instance->nmi_enabled)// NMI disable effect only at vbl set period (HClock between 1 and 3)
                {
                    if ((PPU::__instance->VClock == PPU::__instance->vbl_vclock_Start) && (PPU::__instance->HClock <= 3))
                        Interrupt::__instance->NMI_Current = (Interrupt::__instance->vbl_flag_temp & Interrupt::__instance->nmi_enabled);
                }
                else if (Interrupt::__instance->vbl_flag_temp & !Interrupt::__instance->nmi_old)// Special case ! NMI can be enabled anytime if vbl already set
                    Interrupt::__instance->NMI_Current = true;
                break;
            }
            case 1:// $2001
            {
                PPU::__instance->grayscale = (value & 0x01) != 0 ? 0x30 : 0x3F;
                PPU::__instance->emphasis = (value & 0xE0) << 1;
                
                PPU::__instance->bkg_clipped = (value & 0x02) == 0;
                PPU::__instance->spr_clipped = (value & 0x04) == 0;
                PPU::__instance->bkg_enabled = (value & 0x08) != 0;
                PPU::__instance->spr_enabled = (value & 0x10) != 0;
                break;
            }
            case 3:// $2003
            {
                PPU::__instance->oam_address = value;
                break;
            }
            case 4:// $2004
            {
                if (PPU::__instance->VClock < 240 && PPU::__instance->IsRenderingOn())
                    value = 0xFF;
                if ((PPU::__instance->oam_address & 0x03) == 0x02)
                    value &= 0xE3;
                oam_ram[PPU::__instance->oam_address++] = value;
                break;
            }
            case 5:// $2005
            {
                if (!PPU::__instance->vram_flipflop)
                {
                    PPU::__instance->vram_temp = (PPU::__instance->vram_temp & 0x7FE0) | ((value & 0xF8) >> 3);
                    PPU::__instance->vram_fine = (unsigned char)(value & 0x07);
                }
                else
                {
                    PPU::__instance->vram_temp = (PPU::__instance->vram_temp & 0x0C1F) | ((value & 0x7) << 12) | ((value & 0xF8) << 2);
                }
                PPU::__instance->vram_flipflop = !PPU::__instance->vram_flipflop;
                break;
            }
            case 6:// $2006
            {
                if (!PPU::__instance->vram_flipflop)
                {
                    PPU::__instance->vram_temp = (PPU::__instance->vram_temp & 0x00FF) | ((value & 0x3F) << 8);
                }
                else
                {
                    PPU::__instance->vram_temp = (PPU::__instance->vram_temp & 0x7F00) | value;
                    PPU::__instance->vram_address = PPU::__instance->vram_temp;
                    Memory::__instance->board->OnPPUAddressUpdate(PPU::__instance->vram_address);
                }
                PPU::__instance->vram_flipflop = !PPU::__instance->vram_flipflop;
                break;
            }
            case 7:// $2007
            {
                PPU::__instance->vram_address_temp_access = PPU::__instance->vram_address & 0x3FFF;
                if (PPU::__instance->vram_address_temp_access < 0x2000)
                {
                    Memory::__instance->board->WriteCHR(PPU::__instance->vram_address_temp_access, value );
                }
                else if (PPU::__instance->vram_address_temp_access < 0x3F00)
                {
                    Memory::__instance->board->WriteNMT(PPU::__instance->vram_address_temp_access, value);
                }
                else
                {
                    palettes_bank[PPU::__instance->vram_address_temp_access & ((PPU::__instance->vram_address_temp_access & 0x03) == 0 ? 0x0C : 0x1F)] = value;
                }
                PPU::__instance->vram_address = (PPU::__instance->vram_address + PPU::__instance->vram_increament) & 0x7FFF;
                Memory::__instance->board->OnPPUAddressUpdate(PPU::__instance->vram_address);
                break;
            }
        }
    }
    // Internal APU Registers
    else if(address < 0x4020)
    {
        //printf("W_apu\n");
        switch (address)
        {
            /*Pulse 1*/
            case 0x4000:
            {
                Pulse1::__instance->sq1_volume_decay_time = value & 0xF;
                Pulse1::__instance->sq1_duration_haltRequset = (value & 0x20) != 0;
                Pulse1::__instance->sq1_constant_volume_flag = (value & 0x10) != 0;
                Pulse1::__instance->sq1_envelope = Pulse1::__instance->sq1_constant_volume_flag ? Pulse1::__instance->sq1_volume_decay_time : Pulse1::__instance->sq1_env_counter;
                Pulse1::__instance->sq1_dutyForm = (value & 0xC0) >> 6;
                break;
            }
            case 0x4001:
            {
                Pulse1::__instance->sq1_sweepEnable = (value & 0x80) == 0x80;
                Pulse1::__instance->sq1_sweepDeviderPeriod = (value >> 4) & 7;
                Pulse1::__instance->sq1_sweepNegateFlag = (value & 0x8) == 0x8;
                Pulse1::__instance->sq1_sweepShiftCount = value & 7;
                Pulse1::__instance->sq1_sweepReload = true;
                break;
            }
            case 0x4002:
            {
                Pulse1::__instance->sq1_frequency = (Pulse1::__instance->sq1_frequency & 0x0700) | value;
                break;
            }
            case 0x4003:
            {
                Pulse1::__instance->sq1_duration_reload = DurationTable[value >> 3];
                Pulse1::__instance->sq1_duration_reloadRequst = true;
                Pulse1::__instance->sq1_frequency = (Pulse1::__instance->sq1_frequency & 0x00FF) | ((value & 7) << 8);
                Pulse1::__instance->sq1_dutyStep = 0;
                Pulse1::__instance->sq1_env_startflag = true;
                break;
            }
                /*Pulse 2*/
            case 0x4004:
            {
                Pulse2::__instance->sq2_volume_decay_time = value & 0xF;
                Pulse2::__instance->sq2_duration_haltRequset = (value & 0x20) != 0;
                Pulse2::__instance->sq2_constant_volume_flag = (value & 0x10) != 0;
                Pulse2::__instance->sq2_envelope = Pulse2::__instance->sq2_constant_volume_flag ? Pulse2::__instance->sq2_volume_decay_time : Pulse2::__instance->sq2_env_counter;
                Pulse2::__instance->sq2_dutyForm = (value & 0xC0) >> 6;
                break;
            }
            case 0x4005:
            {
                Pulse2::__instance->sq2_sweepEnable = (value & 0x80) == 0x80;
                Pulse2::__instance->sq2_sweepDeviderPeriod = (value >> 4) & 7;
                Pulse2::__instance->sq2_sweepNegateFlag = (value & 0x8) == 0x8;
                Pulse2::__instance->sq2_sweepShiftCount = value & 7;
                Pulse2::__instance->sq2_sweepReload = true;
                break;
            }
            case 0x4006:
            {
                Pulse2::__instance->sq2_frequency = (Pulse2::__instance->sq2_frequency & 0x0700) | value;
                break;
            }
            case 0x4007:
            {
                Pulse2::__instance->sq2_duration_reload = DurationTable[value >> 3];
                Pulse2::__instance->sq2_duration_reloadRequst = true;
                Pulse2::__instance->sq2_frequency = (Pulse2::__instance->sq2_frequency & 0x00FF) | ((value & 7) << 8);
                Pulse2::__instance->sq2_dutyStep = 0;
                Pulse2::__instance->sq2_env_startflag = true;
                break;
            }
                /*Triangle*/
            case 0x4008:
            {
                Triangle::__instance->trl_linearCounterHalt = Triangle::__instance->trl_duration_haltRequset = (value & 0x80) != 0;
                Triangle::__instance->trl_linearCounterReload = (unsigned char)(value & 0x7F);
                break;
            }
            case 0x400A:
            {
                Triangle::__instance->trl_frequency = (Triangle::__instance->trl_frequency & 0x700) | value;
                break;
            }
            case 0x400B:
            {
                Triangle::__instance->trl_frequency = (Triangle::__instance->trl_frequency & 0x00FF) | ((value & 7) << 8);
                
                Triangle::__instance->trl_duration_reload = DurationTable[value >> 3];
                Triangle::__instance->trl_duration_reloadRequst = true;
                Triangle::__instance->trl_halt = true;
                break;
            }
                /*Noise*/
            case 0x400C:
            {
                Noise::__instance->noz_volume_decay_time = value & 0xF;
                Noise::__instance->noz_duration_haltRequset = (value & 0x20) != 0;
                Noise::__instance->noz_constant_volume_flag = (value & 0x10) != 0;
                Noise::__instance->noz_envelope = Noise::__instance->noz_constant_volume_flag ? Noise::__instance->noz_volume_decay_time : Noise::__instance->noz_env_counter;
                break;
            }
            case 0x400E:
            {
                Noise::__instance->noz_frequency = NozFrequencyTable[APU::__instance->systemIndex][value & 0x0F];
                Noise::__instance->noz_mode = (value & 0x80) == 0x80;
                break;
            }
            case 0x400F:
            {
                Noise::__instance->noz_duration_reload = DurationTable[value >> 3];
                Noise::__instance->noz_duration_reloadRequst = true;
                Noise::__instance->noz_env_startflag = true;
                break;
            }
                /*DMC*/
            case 0x4010:
            {
                DMC::__instance->DMCIrqEnabled = (value & 0x80) != 0;
                DMC::__instance->dmc_dmaLooping = (value & 0x40) != 0;
                
                if (!DMC::__instance->DMCIrqEnabled)
                {
                    DMC::__instance->DeltaIrqOccur = false;
                    Interrupt::__instance->IRQFlags &= ~Interrupt::__instance->IRQ_DMC;
                }
                DMC::__instance->dmc_freqTimer = value & 0x0F;
                break;
            }
            case 0x4011: { DMC::__instance->dmc_output = (unsigned char)(value & 0x7F); break; }
            case 0x4012: { DMC::__instance->dmc_dmaAddrRefresh = (value << 6) | 0xC000; break; }
            case 0x4013: { DMC::__instance->dmc_dmaSizeRefresh = (value << 4) | 0x0001; break; }
            case 0x4014:
            {
                DMA::__instance->dmaOamaddress = value << 8;
                DMA::__instance->AssertOAMDMA();
                break;
            }
            case 0x4015:
            {
                // SQ1
                Pulse1::__instance->sq1_duration_reloadEnabled = (value & 0x01) != 0;
                if (!Pulse1::__instance->sq1_duration_reloadEnabled)
                    Pulse1::__instance->sq1_duration_counter = 0;
                // SQ2
                Pulse2::__instance->sq2_duration_reloadEnabled = (value & 0x02) != 0;
                if (!Pulse2::__instance->sq2_duration_reloadEnabled)
                    Pulse2::__instance->sq2_duration_counter = 0;
                // TRL
                Triangle::__instance->trl_duration_reloadEnabled = (value & 0x04) != 0;
                if (!Triangle::__instance->trl_duration_reloadEnabled)
                    Triangle::__instance->trl_duration_counter = 0;
                // NOZ
                Noise::__instance->noz_duration_reloadEnabled = (value & 0x08) != 0;
                if (!Noise::__instance->noz_duration_reloadEnabled)
                    Noise::__instance->noz_duration_counter = 0;
                // DMC
                if ((value & 0x10) != 0)
                {
                    if (DMC::__instance->dmc_dmaSize == 0)
                    {
                        DMC::__instance->dmc_dmaSize = DMC::__instance->dmc_dmaSizeRefresh;
                        DMC::__instance->dmc_dmaAddr = DMC::__instance->dmc_dmaAddrRefresh;
                    }
                }
                else { DMC::__instance->dmc_dmaSize = 0; }
                // Disable DMC IRQ
                DMC::__instance->DeltaIrqOccur = false;
                Interrupt::__instance->IRQFlags &= ~Interrupt::__instance->IRQ_DMC;
                // RDY ?
                if (!DMC::__instance->dmc_bufferFull && DMC::__instance->dmc_dmaSize > 0)
                {
                    DMA::__instance->AssertDMCDMA();
                }
                break;
            }
            case 0x4016:
            {
                if (Input::__instance->inputStrobe > (value & 0x01))
                {
                    if (Input::__instance->IsFourPlayers)
                    {
                        Input::__instance->PORT0 = Input::__instance->joypad3->GetData() << 8 | Input::__instance->joypad1->GetData() | 0x01010000;
                        Input::__instance->PORT1 = Input::__instance->joypad4->GetData() << 8 | Input::__instance->joypad2->GetData() | 0x02020000;
                    }
                    else
                    {
                        Input::__instance->PORT0 = Input::__instance->joypad1->GetData() | 0x01010100;// What is this ? see *
                        Input::__instance->PORT1 = Input::__instance->joypad2->GetData() | 0x02020200;
                    }
                }
                if (VSUnisystem::IsVSUnisystem)
                    board->VSUnisystem4016RW( value);
                Input::__instance-> inputStrobe = value & 0x01;
                break;
                // * The data port is 24 bits length
                // Each 8 bits indicates device, if that device is connected, then device data set on it normally...
                // So we have 4 block of data on each register ([] indicate byte block here, let's call these blocks a SEQ)
                // SEQ:
                // [block 3] [block 2] [block 1] [block 0]
                // 0000 0000 0000 0000 0000 0000 0000 0000
                // ^ bit 23                              ^ bit 0
                // Let's say we connect joypad 1 and joypad2, then:
                // In $4016: the data could be like this [00h][00h][00h][joy1]
                // In $4017: the data could be like this [00h][00h][00h][joy2]
                // Instead of having 00h value on other blocks, the read returns a bit set on each unused block
                // to indicate that there's no device (i.e. joypad) is connected :
                // In $4016 the first bit (i.e. bit 0) is set if no device connected on that block
                // Example: [01h][01h][01h][joy1] (we only have joypad 1 connected so other blocks are blocked)
                // In $4017 work the same but with second bit (i.e. bit 1) is set if no device connected on other blocks
                // Example: [02h][02h][02h][joy2] (when we have joypad 2 connected so other blocks are blocked)
                // If we connect 4 joypads then:
                // $4016 : [01h][01h][joy3][joy1]
                // $4017 : [02h][02h][joy4][joy2]
            }
            case 0x4017:
            {
                APU::__instance->SequencingMode = (value & 0x80) != 0;
                APU::__instance->FrameIrqEnabled = (value & 0x40) == 0;
                
                APU::__instance->CurrentSeq = 0;
                
                if (!APU::__instance->SequencingMode)
                    APU::__instance->Cycles = SequenceMode0[APU::__instance->systemIndex][0];
                else
                    APU::__instance->Cycles = SequenceMode1[APU::__instance->systemIndex][0];
                
                if (!APU::__instance->oddCycle)
                    APU::__instance->Cycles++;
                else
                    APU::__instance->Cycles += 2;
                
                if (!APU::__instance->FrameIrqEnabled)
                {
                    APU::__instance->FrameIrqFlag = false;
                    Interrupt::__instance->IRQFlags &= ~Interrupt::__instance->IRQ_APU;
                }
                break;
            }
        }
    }
    // Cartridge Expansion Area almost 8K
    else if (address < 0x6000)
    {
        //printf("W_Expansion\n");
        if (VSUnisystem::IsVSUnisystem && address == 0x4020)
            Input::__instance->VSUnisystemDIP->Write4020(value);
        board->WriteEXP( address,  value);
    }
    // Cartridge SRAM Area 8K
    else if (address < 0x8000)
    {
        //printf("W_Cartridge SRAM\n");
        board->WriteSRM(address, value);
    }
    // Cartridge PRG-ROM Area 32K
    else if (address <= 0xFFFF)
    {
        //printf("W_PRG-ROMM\n");
        board->WritePRG(address, value);
    }
}
