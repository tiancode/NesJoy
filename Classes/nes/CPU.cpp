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

#include "CPU.h"

#include "Memory.h"
#include "Interrupt.h"

CPU* CPU::__instance = NULL;

CPU* CPU::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new CPU;
    }
    return __instance;
}

void CPU::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
        __instance = NULL;
    }
}

CPU::CPU()
{
    
}

CPU::~CPU()
{
    
}

void CPU::setCpuSpeed(int cpuSpeedInHz)
{
    this->cpuSpeedInHz = cpuSpeedInHz;
}

void CPU::hardReset()
{
    memset(&_register, 0, sizeof(Register));
    
    _register.spl = 0xFD;
    _register.sph = 0x01;

    // memory must init done
    _register.pcl = Memory::__instance->read(0xFFFC);
    _register.pch = Memory::__instance->read(0xFFFC+1);

    _register.i = true;
    _register.ea = 0;
    //interrupts
    Interrupt::__instance->NMI_Current = false;
    Interrupt::__instance->NMI_Old = false;
    Interrupt::__instance->NMI_Detected = false;
    Interrupt::__instance->IRQFlags = 0;
    Interrupt::__instance->IRQ_Detected = false;
    Interrupt::__instance->interrupt_vector = 0;
    Interrupt::__instance->interrupt_suspend_irq = false;
    Interrupt::__instance->interrupt_suspend_nmi = false;
    Interrupt::__instance->nmi_enabled = false;
    Interrupt::__instance->nmi_old = false;
    Interrupt::__instance->vbl_flag = false;
    Interrupt::__instance->vbl_flag_temp = false;
    //others
    _opcode = 0;
    _M = 0;
}

void CPU::softReset()
{
    _register.i = true;
    _register.sp -= 3;
    
    _register.pcl = Memory::__instance->read(0xFFFC);
    _register.pch = Memory::__instance->read(0xFFFC+1);
}


void CPU::clock()
{
    // First clock is to fetch opcode
    _opcode = Memory::__instance->read(_register.pc);
    _register.pc++;
    
    switch (_opcode)
    {
        case 0x4C:
        {
            Absolute_W();
            _register.pc = _register.ea;/*JMP*/
            break;
        }
        case 0xAD:
        {
            Absolute_R();
            LDA();
            break;
        }
        case 0x29:
        {
            Immediate();
            AND();
            break;
        }
        case 0xD0:
        {
            Branch(!_register.z);
            break;
        }
        case 0xF0:
        {
            Branch(_register.z);
            break;
        }
        case 0xC8:
        {
            ImpliedAccumulator();
            INY();
            break;
        }
        case 0x85:
        {
            ZeroPage_W();
            STA();
            break;
        }
        case 0x99:
        {
            AbsoluteY_W();
            STA();
            break;
        }
        case 0x4A:
        {
            ImpliedAccumulator();
            LSR_A();
            break;
        }
        case 0xC9:
        {
            Immediate();
            CMP();
            break;
        }
        case 0xBD:
        {
            AbsoluteX_R();
            LDA();
            break;
        }
        case 0xA5:
        {
            ZeroPage_R();
            LDA();
            break;
        }
        case 0x20:
        {
            JSR();
            break;
        }
        case 0x60:
        {
            ImpliedAccumulator();
            RTS();
            break;
        }
        case 0x10:
        {
            Branch(!_register.n);
            break;
        }
        case 0x90:
        {
            Branch(!_register.c);
            break;
        }
        case 0x18:
        {
            ImpliedAccumulator();
            _register.c = false;
            break;
        }
        case 0x88:
        {
            ImpliedAccumulator();
            DEY();
            break;
        }
        case 0xCA:
        {
            ImpliedAccumulator();
            DEX();
            break;
        }
        case 0xB5:
        {
            ZeroPageX_R();
            LDA();
            break;
        }
        case 0xA9:
        {
            Immediate();
            LDA();
            break;
        }
        case 0x68:
        {
            ImpliedAccumulator();
            PLA();
            break;
        }
        case 0x8D:
        {
            Absolute_W();
            STA();
            break;
        }
        case 0xB9:
        {
            AbsoluteY_R();
            LDA();
            break;
        }
        case 0x0A:
        {
            ImpliedAccumulator();
            ASL_A();
            break;
        }
        case 0x48:
        {
            ImpliedAccumulator();
            PHA();
            break;
        }
        case 0xB1:
        {
            IndirectY_R();
            LDA();
            break;
        }
        case 0xE8:
        {
            ImpliedAccumulator();
            INX();
            break;
        }
        case 0xB0:
        {
            Branch(_register.c);
            break;
        }
        case 0xA8:
        {
            ImpliedAccumulator();
            TAY();
            break;
        }
        case 0x69:
        {
            Immediate();
            ADC();
            break;
        }
        case 0x9D:
        {
            AbsoluteX_W();
            STA();
            break;
        }
        case 0xA0:
        {
            Immediate();
            LDY();
            break;
        }
        case 0xAC:
        {
            Absolute_R();
            LDY();
            break;
        }
        case 0x86:
        {
            ZeroPage_W();
            STX();
            break;
        }
        case 0xA4:
        {
            ZeroPage_R();
            LDY();
            break;
        }
        case 0xA6:
        {
            ZeroPage_R();
            LDX();
            break;
        }
        case 0x38:
        {
            ImpliedAccumulator();
            _register.c = true;
            break;
        }
        case 0xC0:
        {
            Immediate();
            CPY();
            break;
        }
        case 0x79:
        {
            AbsoluteY_R();
            ADC();
            break;
        }
        case 0xA2:
        {
            Immediate();
            LDX();
            break;
        }
        case 0x2A:
        {
            ImpliedAccumulator();
            ROL_A();
            break;
        }
        case 0xF5:
        {
            ZeroPageX_R();
            SBC();
            break;
        }
        case 0xC5:
        {
            ZeroPage_R();
            CMP();
            break;
        }
        case 0xE0:
        {
            Immediate();
            CPX();
            break;
        }
        case 0x30:
        {
            Branch(_register.n);
            break;
        }
        case 0xBE:
        {
            AbsoluteY_R();
            LDX();
            break;
        }
        case 0x98:
        {
            ImpliedAccumulator();
            TYA();
            break;
        }
        case 0x65:
        {
            ZeroPage_R();
            ADC();
            break;
        }
        case 0x84:
        {
            ZeroPage_W();
            STY();
            break;
        }
        case 0x95:
        {
            ZeroPageX_W();
            STA();
            break;
        }
        case 0xF9:
        {
            AbsoluteY_R();
            SBC();
            break;
        }
        case 0xE9:
        {
            Immediate();
            SBC();
            break;
        }
        case 0xAA:
        {
            ImpliedAccumulator();
            TAX();
            break;
        }
        case 0xAE:
        {
            Absolute_R();
            LDX();
            break;
        }
        case 0x6A:
        {
            ImpliedAccumulator();
            ROR_A();
            break;
        }
        case 0xC6:
        {
            ZeroPage_RW();
            DEC();
            break;
        }
        case 0x00:
        {
            BRK();
            break;
        }
        case 0x01:
        {
            IndirectX_R();
            ORA();
            break;
        }
        case 0x02:
        {
            ImpliedAccumulator();// ILLEGAL ! set JAM.
            break;
        }
        case 0x03:
        {
            IndirectX_W();
            SLO();
            break;
        }
        case 0x04:
        {
            ZeroPage_R(); // ILLEGAL ! set DOP
            break;
        }
        case 0x05:
        {
            ZeroPage_R();
            ORA();
            break;
        }
        case 0x06:
        {
            ZeroPage_RW();
            ASL_M();
            break;
        }
        case 0x07:
        {
            ZeroPage_W();
            SLO();
            break;
        }
        case 0x08:
        {
            ImpliedAccumulator();
            PHP();
            break;
        }
        case 0x09:
        {
            Immediate();
            ORA();
            break;
        }
        case 0x0B:
        {
            Immediate();
            ANC();
            break;
        }
        case 0x0C:
        {
            Absolute_R(); // ILLEGAL ! set TOP
            break;
        }
        case 0x0D:
        {
            Absolute_R();
            ORA();
            break;
        }
        case 0x0E:
        {
            Absolute_RW();
            ASL_M();
            break;
        }
        case 0x0F:
        {
            Absolute_W();
            SLO();
            break;
        }
        case 0x11:
        {
            IndirectY_R();
            ORA();
            break;
        }
        case 0x12:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0x13:
        {
            IndirectY_W();
            SLO();
            break;
        }
        case 0x14:
        {
            ZeroPageX_R();// ILLEGAL ! set DOP
            break;
        }
        case 0x15:
        {
            ZeroPageX_R();
            ORA();
            break;
        }
        case 0x16:
        {
            ZeroPageX_RW();
            ASL_M();
            break;
        }
        case 0x17:
        {
            ZeroPageX_W();
            SLO();
            break;
        }
        case 0x19:
        {
            AbsoluteY_R();
            ORA();
            break;
        }
        case 0x1A:
        {
            ImpliedAccumulator();// LEGAL ! set NOP. (is NOP a legal instruction ?)
            break;
        }
        case 0x1B:
        {
            AbsoluteY_W();
            SLO();
            break;
        }
        case 0x1C:
        {
            AbsoluteX_R(); // ILLEGAL ! set TOP
            break;
        }
        case 0x1D:
        {
            AbsoluteX_R();
            ORA();
            break;
        }
        case 0x1E:
        {
            AbsoluteX_RW();
            ASL_M();
            break;
        }
        case 0x1F:
        {
            AbsoluteX_W();
            SLO();
            break;
        }
        case 0x21:
        {
            IndirectX_R();
            AND();
            break;
        }
        case 0x22:
        {
            ImpliedAccumulator();// ILLEGAL ! set JAM.
            break;
        }
        case 0x23:
        {
            IndirectX_W();
            RLA();
            break;
        }
        case 0x24:
        {
            ZeroPage_R();
            BIT();
            break;
        }
        case 0x25:
        {
            ZeroPage_R();
            AND();
            break;
        }
        case 0x26:
        {
            ZeroPage_RW();
            ROL_M();
            break;
        }
        case 0x27:
        {
            ZeroPage_W();
            RLA();
            break;
        }
        case 0x28:
        {
            ImpliedAccumulator();
            PLP();
            break;
        }
        case 0x2B:
        {
            Immediate();
            ANC();
            break;
        }
        case 0x2C:
        {
            Absolute_R();
            BIT();
            break;
        }
        case 0x2D:
        {
            Absolute_R();
            AND();
            break;
        }
        case 0x2E:
        {
            Absolute_RW();
            ROL_M();
            break;
        }
        case 0x2F:
        {
            Absolute_W();
            RLA();
            break;
        }
        case 0x31:
        {
            IndirectY_R();
            AND();
            break;
        }
        case 0x32:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0x33:
        {
            IndirectY_W();
            RLA();
            break;
        }
        case 0x34:
        {
            ZeroPageX_R();// ILLEGAL ! set DOP
            break;
        }
        case 0x35:
        {
            ZeroPageX_R();
            AND();
            break;
        }
        case 0x36:
        {
            ZeroPageX_RW();
            ROL_M();
            break;
        }
        case 0x37:
        {
            ZeroPageX_W();
            RLA();
            break;
        }
        case 0x39:
        {
            AbsoluteY_R();
            AND();
            break;
        }
        case 0x3A:
        {
            ImpliedAccumulator();// LEGAL ! set NOP. (is NOP a legal instruction ?)
            break;
        }
        case 0x3B:
        {
            AbsoluteY_W();
            RLA();
            break;
        }
        case 0x3C:
        {
            AbsoluteX_R(); // ILLEGAL ! set TOP
            break;
        }
        case 0x3D:
        {
            AbsoluteX_R();
            AND();
            break;
        }
        case 0x3E:
        {
            AbsoluteX_RW();
            ROL_M();
            break;
        }
        case 0x3F:
        {
            AbsoluteX_W();
            RLA();
            break;
        }
        case 0x40:
        {
            ImpliedAccumulator();
            RTI();
            break;
        }
        case 0x41:
        {
            IndirectX_R();
            EOR();
            break;
        }
        case 0x42:
        {
            ImpliedAccumulator();// ILLEGAL ! set JAM.
            break;
        }
        case 0x43:
        {
            IndirectX_W();
            SRE();
            break;
        }
        case 0x44:
        {
            ZeroPage_R(); // ILLEGAL ! set DOP
            break;
        }
        case 0x45:
        {
            ZeroPage_R();
            EOR();
            break;
        }
        case 0x46:
        {
            ZeroPage_RW();
            LSR_M();
            break;
        }
        case 0x47:
        {
            ZeroPage_W();
            SRE();
            break;
        }
        case 0x49:
        {
            Immediate();
            EOR();
            break;
        }
        case 0x4B:
        {
            Immediate();
            ALR();
            break;
        }
        case 0x4D:
        {
            Absolute_R();
            EOR();
            break;
        }
        case 0x4E:
        {
            Absolute_RW();
            LSR_M();
            break;
        }
        case 0x4F:
        {
            Absolute_W();
            SRE();
            break;
        }
        case 0x50:
        {
            Branch(!_register.v);
            break;
        }
        case 0x51:
        {
            IndirectY_R();
            EOR();
            break;
        }
        case 0x52:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0x53:
        {
            IndirectY_W();
            SRE();
            break;
        }
        case 0x54:
        {
            ZeroPageX_R();// ILLEGAL ! set DOP
            break;
        }
        case 0x55:
        {
            ZeroPageX_R();
            EOR();
            break;
        }
        case 0x56:
        {
            ZeroPageX_RW();
            LSR_M();
            break;
        }
        case 0x57:
        {
            ZeroPageX_W();
            SRE();
            break;
        }
        case 0x58:
        {
            ImpliedAccumulator();
            _register.i = false;
            break;
        }
        case 0x59:
        {
            AbsoluteY_R();
            EOR();
            break;
        }
        case 0x5A:
        {
            ImpliedAccumulator();// LEGAL ! set NOP. (is NOP a legal instruction ?)
            break;
        }
        case 0x5B:
        {
            AbsoluteY_W();
            SRE();
            break;
        }
        case 0x5C:
        {
            AbsoluteX_R(); // ILLEGAL ! set TOP
            break;
        }
        case 0x5D:
        {
            AbsoluteX_R();
            EOR();
            break;
        }
        case 0x5E:
        {
            AbsoluteX_RW();
            LSR_M();
            break;
        }
        case 0x5F:
        {
            AbsoluteX_W();
            SRE();
            break;
        }
        case 0x61:
        {
            IndirectX_R();
            ADC();
            break;
        }
        case 0x62:
        {
            ImpliedAccumulator();// ILLEGAL ! set JAM.
            break;
        }
        case 0x63:
        {
            IndirectX_W();
            RRA();
            break;
        }
        case 0x64:
        {
            ZeroPage_R(); // ILLEGAL ! set DOP
            break;
        }
        case 0x66:
        {
            ZeroPage_RW();
            ROR_M();
            break;
        }
        case 0x67:
        {
            ZeroPage_W();
            RRA();
            break;
        }
        case 0x6B:
        {
            Immediate();
            ARR();
            break;
        }
        case 0x6C:
        {
            JMP_I();
            break;
        }
        case 0x6D:
        {
            Absolute_R();
            ADC();
            break;
        }
        case 0x6E:
        {
            Absolute_RW();
            ROR_M();
            break;
        }
        case 0x6F:
        {
            Absolute_W();
            RRA();
            break;
        }
        case 0x70:
        {
            Branch(_register.v);
            break;
        }
        case 0x71:
        {
            IndirectY_R();
            ADC();
            break;
        }
        case 0x72:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0x73:
        {
            IndirectY_W();
            RRA();
            break;
        }
        case 0x74:
        {
            ZeroPageX_R();// ILLEGAL ! set DOP
            break;
        }
        case 0x75:
        {
            ZeroPageX_R();
            ADC();
            break;
        }
        case 0x76:
        {
            ZeroPageX_RW();
            ROR_M();
            break;
        }
        case 0x77:
        {
            ZeroPageX_W();
            RRA();
            break;
        }
        case 0x78:
        {
            ImpliedAccumulator();
            _register.i = true;
            break;
        }
        case 0x7A:
        {
            ImpliedAccumulator();// LEGAL ! set NOP. (is NOP a legal instruction ?)
            break;
        }
        case 0x7B:
        {
            AbsoluteY_W();
            RRA();
            break;
        }
        case 0x7C:
        {
            AbsoluteX_R(); // ILLEGAL ! set TOP
            break;
        }
        case 0x7D:
        {
            AbsoluteX_R();
            ADC();
            break;
        }
        case 0x7E:
        {
            AbsoluteX_RW();
            ROR_M();
            break;
        }
        case 0x7F:
        {
            AbsoluteX_W();
            RRA();
            break;
        }
        case 0x80:
        {
            Immediate(); // ILLEGAL ! set DOP
            break;
        }
        case 0x81:
        {
            IndirectX_W();
            STA();
            break;
        }
        case 0x82:
        {
            Immediate();// ILLEGAL ! set DOP.
            break;
        }
        case 0x83:
        {
            IndirectX_W();
            SAX();
            break;
        }
        case 0x87:
        {
            ZeroPage_W();
            SAX();
            break;
        }
        case 0x89:
        {
            Immediate();// ILLEGAL ! set DOP
            break;
        }
        case 0x8A:
        {
            ImpliedAccumulator();
            TXA();
            break;
        }
        case 0x8B:
        {
            Immediate();
            XAA();
            break;
        }
        case 0x8C:
        {
            Absolute_W();
            STY();
            break;
        }
        case 0x8E:
        {
            Absolute_W();
            STX();
            break;
        }
        case 0x8F:
        {
            Absolute_W();
            SAX();
            break;
        }
        case 0x91:
        {
            IndirectY_W();
            STA();
            break;
        }
        case 0x92:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0x93:
        {
            IndirectY_W();
            AHX();
            break;
        }
        case 0x94:
        {
            ZeroPageX_W();
            STY();
            break;
        }
        case 0x96:
        {
            ZeroPageY_W();
            STX();
            break;
        }
        case 0x97:
        {
            ZeroPageY_W();
            SAX();
            break;
        }
        case 0x9A:
        {
            ImpliedAccumulator();
            TXS();
            break;
        }
        case 0x9B:
        {
            AbsoluteY_W();
            XAS();
            break;
        }
        case 0x9C:
        {
            Absolute_W();
            SHY(); // ILLEGAL ! SHY fits here.
            break;
        }
        case 0x9E:
        {
            Absolute_W();
            SHX();// ILLEGAL ! SHX fits here.
            break;
        }
        case 0x9F:
        {
            AbsoluteY_W();
            AHX();
            break;
        }
        case 0xA1:
        {
            IndirectX_R();
            LDA();
            break;
        }
        case 0xA3:
        {
            IndirectX_R();
            LAX();
            break;
        }
        case 0xA7:
        {
            ZeroPage_R();
            LAX();
            break;
        }
        case 0xAB:
        {
            Immediate();
            LAX();
            break;
        }
        case 0xAF:
        {
            Absolute_R();
            LAX();
            break;
        }
        case 0xB2:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0xB3:
        {
            IndirectY_R();
            LAX();
            break;
        }
        case 0xB4:
        {
            ZeroPageX_R();
            LDY();
            break;
        }
        case 0xB6:
        {
            ZeroPageY_R();
            LDX();
            break;
        }
        case 0xB7:
        {
            ZeroPageY_R();
            LAX();
            break;
        }
        case 0xB8:
        {
            ImpliedAccumulator();
            _register.v = false;
            break;
        }
        case 0xBA:
        {
            ImpliedAccumulator();
            TSX();
            break;
        }
        case 0xBB:
        {
            AbsoluteY_R();
            LAR();
            break;
        }
        case 0xBC:
        {
            AbsoluteX_R();
            LDY();
            break;
        }
        case 0xBF:
        {
            AbsoluteY_R();
            LAX();
            break;
        }
        case 0xC1:
        {
            IndirectX_R();
            CMP();
            break;
        }
        case 0xC2:
        {
            Immediate(); // ILLEGAL ! set DOP.
            break;
        }
        case 0xC3:
        {
            IndirectX_R();
            DCP();
            break;
        }
        case 0xC4:
        {
            ZeroPage_R();
            CPY();
            break;
        }
        case 0xC7:
        {
            ZeroPage_R();
            DCP();
            break;
        }
        case 0xCB:
        {
            Immediate();
            AXS();
            break;
        }
        case 0xCC:
        {
            Absolute_R();
            CPY();
            break;
        }
        case 0xCD:
        {
            Absolute_R();
            CMP();
            break;
        }
        case 0xCE:
        {
            Absolute_RW();
            DEC();
            break;
        }
        case 0xCF:
        {
            Absolute_R();
            DCP();
            break;
        }
        case 0xD1:
        {
            IndirectY_R();
            CMP();
            break;
        }
        case 0xD2:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0xD3:
        {
            IndirectY_RW();
            DCP();
            break;
        }
        case 0xD4:
        {
            ZeroPageX_R();// ILLEGAL ! set DOP
            break;
        }
        case 0xD5:
        {
            ZeroPageX_R();
            CMP();
            break;
        }
        case 0xD6:
        {
            ZeroPageX_RW();
            DEC();
            break;
        }
        case 0xD7:
        {
            ZeroPageX_RW();
            DCP();
            break;
        }
        case 0xD8:
        {
            ImpliedAccumulator();
            _register.d = false;
            break;
        }
        case 0xD9:
        {
            AbsoluteY_R();
            CMP();
            break;
        }
        case 0xDA:
        {
            ImpliedAccumulator();// LEGAL ! set NOP. (is NOP a legal instruction ?)
            break;
        }
        case 0xDB:
        {
            AbsoluteY_RW();
            DCP();
            break;
        }
        case 0xDC:
        {
            AbsoluteX_R(); // ILLEGAL ! set TOP
            break;
        }
        case 0xDD:
        {
            AbsoluteX_R();
            CMP();
            break;
        }
        case 0xDE:
        {
            AbsoluteX_RW();
            DEC();
            break;
        }
        case 0xDF:
        {
            AbsoluteX_RW();
            DCP();
            break;
        }
        case 0xE1:
        {
            IndirectX_R();
            SBC();
            break;
        }
        case 0xE2:
        {
            Immediate(); // ILLEGAL ! set DOP.
            break;
        }
        case 0xE3:
        {
            IndirectX_W();
            ISC();
            break;
        }
        case 0xE4:
        {
            ZeroPage_R();
            CPX();
            break;
        }
        case 0xE5:
        {
            ZeroPage_R();
            SBC();
            break;
        }
        case 0xE6:
        {
            ZeroPage_RW();
            INC();
            break;
        }
        case 0xE7:
        {
            ZeroPage_W();
            ISC();
            break;
        }
        case 0xEA:
        {
            ImpliedAccumulator();// NOP ...
            break;
        }
        case 0xEB:
        {
            Immediate();
            SBC();
            break;
        }
        case 0xEC:
        {
            Absolute_R();
            CPX();
            break;
        }
        case 0xED:
        {
            Absolute_R();
            SBC();
            break;
        }
        case 0xEE:
        {
            Absolute_RW();
            INC();
            break;
        }
        case 0xEF:
        {
            Absolute_W();
            ISC();
            break;
        }
        case 0xF1:
        {
            IndirectY_R();
            SBC();
            break;
        }
        case 0xF2:
        {
            // ILLEGAL ! set JAM.
            break;
        }
        case 0xF3:
        {
            IndirectY_W();
            ISC();
            break;
        }
        case 0xF4:
        {
            ZeroPageX_R();// ILLEGAL ! set DOP
            break;
        }
        case 0xF6:
        {
            ZeroPageX_RW();
            INC();
            break;
        }
        case 0xF7:
        {
            ZeroPageX_W();
            ISC();
            break;
        }
        case 0xF8:
        {
            ImpliedAccumulator();
            _register.d = true;
            break;
        }
        case 0xFA:
        {
            ImpliedAccumulator();// LEGAL ! set NOP. (is NOP a legal instruction ?)
            break;
        }
        case 0xFB:
        {
            AbsoluteY_W();
            ISC();
            break;
        }
        case 0xFC:
        {
            AbsoluteX_R(); // ILLEGAL ! set TOP
            break;
        }
        case 0xFD:
        {
            AbsoluteX_R();
            SBC();
            break;
        }
        case 0xFE:
        {
            AbsoluteX_RW();
            INC();
            break;
        }
        case 0xFF:
        {
            AbsoluteX_W();
            ISC();
            break;
        }
    }
    // Handle interrupts...
    if (Interrupt::__instance->NMI_Detected)
    {
        Interrupt();
        
        Interrupt::__instance->NMI_Detected = false;// NMI handled !
    }
    else if (Interrupt::__instance->IRQ_Detected)
    {
        Interrupt();
    }
}


/*
 * _R: read access instructions, set the M value. Some addressing modes will execute 1 extra cycle when page crossed.
 * _W: write access instructions, doesn't set the M value. The instruction should handle write at effective address (EF).
 * _RW: Read-Modify-Write instructions, set the M value and the instruction should handle write at effective address (EF).
 */
void CPU::IndirectX_R()
{
    byte_temp = Memory::__instance->read(_register.pc);
    _register.pc++; // CLock 1
    Memory::__instance->read(byte_temp); // Clock 2
    byte_temp += _register.x;
    _register.eal = Memory::__instance->read(byte_temp);// Clock 3
    byte_temp++;
    
    _register.eah = Memory::__instance->read(byte_temp);// Clock 4
    
    _M = Memory::__instance->read(_register.ea);
}

void CPU::IndirectX_W()
{
    byte_temp = Memory::__instance->read(_register.pc);
    _register.pc++; // CLock 1
    Memory::__instance->read(byte_temp);// Clock 2
    byte_temp += _register.x;
    
    _register.eal = Memory::__instance->read(byte_temp);// Clock 3
    byte_temp++;
    
    _register.eah = Memory::__instance->read(byte_temp);// Clock 4
}

void CPU::IndirectX_RW()
{
    byte_temp = Memory::__instance->read(_register.pc);
    _register.pc++; // CLock 1
    Memory::__instance->read(byte_temp); // Clock 2
    byte_temp += _register.x;
    
    _register.eal = Memory::__instance->read(byte_temp);// Clock 3
    byte_temp++;
    
    _register.eah = Memory::__instance->read(byte_temp);// Clock 4
    
    _M = Memory::__instance->read(_register.ea);
}

void CPU::IndirectY_R()
{
    byte_temp = Memory::__instance->read(_register.pc);
    _register.pc++; // CLock 1
    _register.eal = Memory::__instance->read(byte_temp);
    byte_temp++;// Clock 2
    _register.eah = Memory::__instance->read(byte_temp);// Clock 2
    
    _register.eal += _register.y;
    
    _M= Memory::__instance->read(_register.ea);// Clock 3
    if (_register.eal < _register.y)
    {
        _register.eah++;
        _M = Memory::__instance->read(_register.ea);// Clock 4
    }
}

void CPU::IndirectY_W()
{
    byte_temp = Memory::__instance->read(_register.pc);
    _register.pc++;// CLock 1
    _register.eal = Memory::__instance->read(byte_temp);
    byte_temp++;// Clock 2
    _register.eah = Memory::__instance->read(byte_temp);// Clock 2
    
    _register.eal += _register.y;
    
    _M = Memory::__instance->read(_register.ea);// Clock 3
    if (_register.eal < _register.y)
        _register.eah++;
}

void CPU::IndirectY_RW()
{
    byte_temp = Memory::__instance->read(_register.pc);
    _register.pc++; // CLock 1
    _register.eal = Memory::__instance->read(byte_temp);
    byte_temp++; // Clock 2
    _register.eah = Memory::__instance->read(byte_temp); // Clock 2
    
    _register.eal += _register.y;
    
    Memory::__instance->read(_register.ea); // Clock 3
    if (_register.eal < _register.y)
        _register.eah++;
    
    _M = Memory::__instance->read(_register.ea);// Clock 4
}

void CPU::ZeroPage_R()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;   // Clock 1
    _M = Memory::__instance->read(_register.ea); // Clock 2
}

void CPU::ZeroPage_W()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
}

void CPU::ZeroPage_RW()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    _M = Memory::__instance->read(_register.ea);// Clock 2
}

void CPU::ZeroPageX_R()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    Memory::__instance->read(_register.ea);// Clock 2
    _register.eal += _register.x;
    _M = Memory::__instance->read(_register.ea);// Clock 3
}

void CPU::ZeroPageX_W()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    Memory::__instance->read(_register.ea);// Clock 2
    _register.eal += _register.x;
}

void CPU::ZeroPageX_RW()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    Memory::__instance->read(_register.ea);// Clock 2
    _register.eal += _register.x;
    _M = Memory::__instance->read(_register.ea);// Clock 3
}

void CPU::ZeroPageY_R()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    Memory::__instance->read(_register.ea);// Clock 2
    _register.eal += _register.y;
    _M = Memory::__instance->read(_register.ea);// Clock 3
}

void CPU::ZeroPageY_W()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    Memory::__instance->read(_register.ea);// Clock 2
    _register.eal += _register.y;
}

void CPU::ZeroPageY_RW()
{
    _register.ea = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    Memory::__instance->read(_register.ea);// Clock 2
    _register.eal += _register.y;
    _M = Memory::__instance->read(_register.ea);// Clock 3
}

void CPU::Immediate()
{
    _M = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
}

void CPU::ImpliedAccumulator()
{
    dummy = Memory::__instance->read(_register.pc);
}

void CPU::Absolute_R()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++; // Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++; // Clock 2
    _M = Memory::__instance->read(_register.ea); // Clock 3
}

void CPU::Absolute_W()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++; // Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++; // Clock 2
}

void CPU::Absolute_RW()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 2
    _M = Memory::__instance->read(_register.ea);// Clock 3
}

void CPU::AbsoluteX_R()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 2
    
    _register.eal += _register.x;
    
    _M = Memory::__instance->read(_register.ea);// Clock 3
    if (_register.eal < _register.x)
    {
        _register.eah++;
        _M = Memory::__instance->read(_register.ea);// Clock 4
    }
}

void CPU::AbsoluteX_W()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 2
    
    _register.eal += _register.x;
    
    _M = Memory::__instance->read(_register.ea);// Clock 3
    if (_register.eal < _register.x)
        _register.eah++;
}

void CPU::AbsoluteX_RW()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 2
    
    _register.eal += _register.x;
    
    Memory::__instance->read(_register.ea);// Clock 3
    if (_register.eal < _register.x)
        _register.eah++;
    
    _M = Memory::__instance->read(_register.ea);// Clock 4
}

void CPU::AbsoluteY_R()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;  // Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++;  // Clock 2
    
    _register.eal += _register.y;
    
    _M = Memory::__instance->read(_register.ea);  // Clock 3
    if (_register.eal < _register.y)
    {
        _register.eah++;
        _M = Memory::__instance->read(_register.ea);  // Clock 4
    }
}

void CPU::AbsoluteY_W()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 2
    
    _register.eal += _register.y;
    
    _M = Memory::__instance->read(_register.ea);// Clock 3
    if (_register.eal < _register.y)
        _register.eah++;
}

void CPU::AbsoluteY_RW()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 1
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc++;// Clock 2
    
    _register.eal += _register.y;
    
    _M = Memory::__instance->read(_register.ea);// Clock 3
    if (_register.eal < _register.y)
        _register.eah++;
    
    _M = Memory::__instance->read(_register.ea);// Clock 4
}

void CPU::Push(unsigned char val)
{
    Memory::__instance->write(_register.sp, val);
    _register.spl--;
}

char CPU::Pull()
{
    _register.spl++;
    return Memory::__instance->read(_register.sp);
}

void CPU::Interrupt()
{
    Memory::__instance->read(_register.pc);
    Memory::__instance->read(_register.pc);
    
    Push(_register.pch);
    Push(_register.pcl);
    
    Push(_register.getP());
    // the vector is detected during φ2 of previous cycle (before push about 2 ppu cycles)
    int_temp = Interrupt::__instance->interrupt_vector;
    
    Interrupt::__instance->interrupt_suspend_nmi = true;
    _register.pcl = Memory::__instance->read(int_temp);
    int_temp++;
    _register.i = true;
    _register.pch = Memory::__instance->read(int_temp);
    Interrupt::__instance->interrupt_suspend_nmi = false;
}

void CPU::Branch(bool condition)
{
    byte_temp = Memory::__instance->read(_register.pc);
    _register.pc++;

    if (condition)
    {
        Interrupt::__instance->interrupt_suspend_irq = true;
        Memory::__instance->read(_register.pc);
        _register.pcl += byte_temp;
        Interrupt::__instance->interrupt_suspend_irq = false;
        if (byte_temp >= 0x80)
        {
            if (_register.pcl >= byte_temp)
            {
                Memory::__instance->read(_register.pc);
                _register.pch--;
            }
        }
        else
        {
            if (_register.pcl < byte_temp)
            {
                Memory::__instance->read(_register.pc);
                _register.pch++;
            }
        }
    }
}

void CPU::ADC()
{
    int_temp = (_register.a + _M + (_register.c ? 1 : 0));
    
    _register.v = ((int_temp ^ _register.a) & (int_temp ^ _M) & 0x80) != 0;
    _register.n = (int_temp & 0x80) != 0;
    _register.z = (int_temp & 0xFF) == 0;
    _register.c = (int_temp >> 0x8) != 0;
    
    _register.a = (unsigned char)(int_temp & 0xFF);
}

void CPU::AHX()
{
    byte_temp = (unsigned char)((_register.a & _register.x) & 7);
    Memory::__instance->write(_register.ea, byte_temp);
}

void CPU::ALR()
{
    _register.a &= _M;
    _register.c = (_register.a & 0x01) != 0;
    _register.a >>= 1;
    
    _register.n = (_register.a & 0x80) != 0;
    _register.z = _register.a == 0;
}

void CPU::ANC()
{
    _register.a &= _M;
    _register.n = (_register.a & 0x80) != 0;
    _register.z = _register.a == 0;
    _register.c = (_register.a & 0x80) != 0;
}

void CPU::AND()
{
    _register.a &= _M;
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::ARR()
{
    _register.a = (unsigned char)(((_M & _register.a) >> 1) | (_register.c ? 0x80 : 0x00));
    
    _register.z = (_register.a & 0xFF) == 0;
    _register.n = (_register.a & 0x80) != 0;
    _register.c = (_register.a & 0x40) != 0;
    _register.v = ((_register.a << 1 ^ _register.a) & 0x40) != 0;
}

void CPU::AXS()
{
    int_temp = (_register.a & _register.x) - _M;
    
    _register.n = (int_temp & 0x80) != 0;
    _register.z = (int_temp & 0xFF) == 0;
    _register.c = (~int_temp >> 8) != 0;
    
    _register.x = (unsigned char)(int_temp & 0xFF);
}

void CPU::ASL_M()
{
    _register.c = (_M & 0x80) == 0x80;
    Memory::__instance->write(_register.ea, _M);
    
    _M = (unsigned char)((_M << 1) & 0xFE);
    
    Memory::__instance->write(_register.ea, _M);
    
    _register.n = (_M & 0x80) == 0x80;
    _register.z = (_M == 0);
}

void CPU::ASL_A()
{
    _register.c = (_register.a & 0x80) == 0x80;
    
    _register.a = (unsigned char)((_register.a << 1) & 0xFE);
    
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::BIT()
{
    _register.n = (_M & 0x80) != 0;
    _register.v = (_M & 0x40) != 0;
    _register.z = (_M & _register.a) == 0;
}

void CPU::BRK()
{
    Memory::__instance->read(_register.pc);
    _register.pc++;
    
    Push(_register.pch);
    Push(_register.pcl);
    
    Push(_register.pb());
    // the vector is detected during φ2 of previous cycle (before push about 2 ppu cycles)
    int_temp = Interrupt::__instance->interrupt_vector;
    
    Interrupt::__instance->interrupt_suspend_nmi = true;
    _register.pcl = Memory::__instance->read(int_temp);
    int_temp++;
    _register.i = true;
    _register.pch = Memory::__instance->read(int_temp);
    Interrupt::__instance->interrupt_suspend_nmi = false;
}

void CPU::CMP()
{
    int_temp = _register.a - _M;
    _register.n = (int_temp & 0x80) == 0x80;
    _register.c = (_register.a >= _M);
    _register.z = (int_temp == 0);
}

void CPU::CPX()
{
    int_temp = _register.x - _M;
    _register.n = (int_temp & 0x80) == 0x80;
    _register.c = (_register.x >= _M);
    _register.z = (int_temp == 0);
}

void CPU::CPY()
{
    int_temp = _register.y - _M;
    _register.n = (int_temp & 0x80) == 0x80;
    _register.c = (_register.y >= _M);
    _register.z = (int_temp == 0);
}

void CPU::DCP()
{
    Memory::__instance->write(_register.ea, _M);
    
    _M--;
    Memory::__instance->write(_register.ea, _M);
    
    int_temp = _register.a -_M;
    
    _register.n = (int_temp & 0x80) != 0;
    _register.z = int_temp == 0;
    _register.c = (~int_temp >> 8) != 0;
}

void CPU::DEC()
{
    Memory::__instance->write(_register.ea, _M);
    _M--;
    Memory::__instance->write(_register.ea, _M);
    _register.n = (_M & 0x80) == 0x80;
    _register.z = (_M == 0);
}

void CPU::DEY()
{
    _register.y--;
    _register.z = (_register.y == 0);
    _register.n = (_register.y & 0x80) == 0x80;
}

void CPU::DEX()
{
    _register.x--;
    _register.z = (_register.x == 0);
    _register.n = (_register.x & 0x80) == 0x80;
}

void CPU::EOR()
{
    _register.a ^= _M;
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::INC()
{
    Memory::__instance->write(_register.ea, _M);
    _M++;
    Memory::__instance->write(_register.ea, _M);
    _register.n = (_M & 0x80) == 0x80;
    _register.z = (_M == 0);
}

void CPU::INX()
{
    _register.x++;
    _register.z = (_register.x == 0);
    _register.n = (_register.x & 0x80) == 0x80;
}

void CPU::INY()
{
    _register.y++;
    _register.n = (_register.y & 0x80) == 0x80;
    _register.z = (_register.y == 0);
}

void CPU::ISC()
{
    byte_temp = Memory::__instance->read(_register.ea);
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    byte_temp++;
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    int_temp = byte_temp ^ 0xFF;
    int_temp1 = (_register.a + int_temp + (_register.c ? 1 : 0));
    
    _register.n = (int_temp1 & 0x80) != 0;
    _register.v = ((int_temp1 ^ _register.a) & (int_temp1 ^ int_temp) & 0x80) != 0;
    _register.z = (int_temp1 & 0xFF) == 0;
    _register.c = (int_temp1 >> 0x8) != 0;
    _register.a = (unsigned char)(int_temp1 & 0xFF);
}

void CPU::JMP_I()
{
    // Fetch pointer
    _register.eal = Memory::__instance->read(_register.pc++);
    _register.eah = Memory::__instance->read(_register.pc++);
    
    _register.pcl = Memory::__instance->read(_register.ea);
    _register.eal++; // only increment the low byte, causing the "JMP ($nnnn)" bug
    _register.pch = Memory::__instance->read(_register.ea);
}

void CPU::JSR()
{
    _register.eal = Memory::__instance->read(_register.pc);
    _register.pc++;
    
    Memory::__instance->read(_register.sp);// Dummy read !
    
    Push(_register.pch);
    Push(_register.pcl);
    
    _register.eah = Memory::__instance->read(_register.pc);
    _register.pc = _register.ea;
}

void CPU::LAR()
{
    _register.spl &= _M;
    _register.a = _register.spl;
    _register.x = _register.spl;
    
    _register.n = (_register.spl & 0x80) != 0;
    _register.z = (_register.spl & 0xFF) == 0;
}

void CPU::LAX()
{
    _register.x = _register.a = _M;
    
    _register.n = (_register.x & 0x80) != 0;
    _register.z = (_register.x & 0xFF) == 0;
}

void CPU::LDA()
{
    _register.a = _M;
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::LDX()
{
    _register.x = _M;
    _register.n = (_register.x & 0x80) == 0x80;
    _register.z = (_register.x == 0);
}

void CPU::LDY()
{
    _register.y = _M;
    _register.n = (_register.y & 0x80) == 0x80;
    _register.z = (_register.y == 0);
}

void CPU::LSR_A()
{
    _register.c = (_register.a & 1) == 1;
    _register.a >>= 1;
    _register.z = (_register.a == 0);
    _register.n = (_register.a & 0x80) != 0;
}

void CPU::LSR_M()
{
    _register.c = (_M & 1) == 1;
    Memory::__instance->write(_register.ea, _M);
    _M >>= 1;
    
    Memory::__instance->write(_register.ea, _M);
    _register.z = (_M == 0);
    _register.n = (_M & 0x80) != 0;
}

void CPU::ORA()
{
    _register.a |= _M;
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::PHA()
{
    Push(_register.a);
}

void CPU::PHP()
{
    Push(_register.pb());
}

void CPU::PLA()
{
    Memory::__instance->read(_register.sp);
    _register.a = Pull();
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::PLP()
{
    Memory::__instance->read(_register.sp);
    _register.setP(Pull());
}

void CPU::RLA()
{
    byte_temp = Memory::__instance->read(_register.ea);
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    dummy = (unsigned char)((byte_temp << 1) | (_register.c ? 0x01 : 0x00));
    
    Memory::__instance->write(_register.ea, dummy);
    
    _register.n = (dummy & 0x80) != 0;
    _register.z = (dummy & 0xFF) == 0;
    _register.c = (byte_temp & 0x80) != 0;
    
    _register.a &= dummy;
    _register.n = (_register.a & 0x80) != 0;
    _register.z = (_register.a & 0xFF) == 0;
}

void CPU::ROL_A()
{
    byte_temp = (unsigned char)((_register.a << 1) | (_register.c ? 0x01 : 0x00));
    
    _register.n = (byte_temp & 0x80) != 0;
    _register.z = (byte_temp & 0xFF) == 0;
    _register.c = (_register.a & 0x80) != 0;
    
    _register.a = byte_temp;
}

void CPU::ROL_M()
{
    Memory::__instance->write(_register.ea, _M);
    byte_temp = (unsigned char)((_M << 1) | (_register.c ? 0x01 : 0x00));
    
    Memory::__instance->write(_register.ea, byte_temp);
    _register.n = (byte_temp & 0x80) != 0;
    _register.z = (byte_temp & 0xFF) == 0;
    _register.c = (_M & 0x80) != 0;
}

void CPU::ROR_A()
{
    byte_temp = (unsigned char)((_register.a >> 1) | (_register.c ? 0x80 : 0x00));
    
    _register.n = (byte_temp & 0x80) != 0;
    _register.z = (byte_temp & 0xFF) == 0;
    _register.c = (_register.a & 0x01) != 0;
    
    _register.a = byte_temp;
}

void CPU::ROR_M()
{
    Memory::__instance->write(_register.ea, _M);
    
    byte_temp = (unsigned char)((_M >> 1) | (_register.c ? 0x80 : 0x00));
    Memory::__instance->write(_register.ea, byte_temp);
    
    _register.n = (byte_temp & 0x80) != 0;
    _register.z = (byte_temp & 0xFF) == 0;
    _register.c = (_M & 0x01) != 0;
}

void CPU::RRA()
{
    byte_temp = Memory::__instance->read(_register.ea);
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    dummy = (unsigned char)((byte_temp >> 1) | (_register.c ? 0x80 : 0x00));
    
    Memory::__instance->write(_register.ea, dummy);
    
    _register.n = (dummy & 0x80) != 0;
    _register.z = (dummy & 0xFF) == 0;
    _register.c = (byte_temp & 0x01) != 0;
    
    byte_temp = dummy;
    int_temp = (_register.a + byte_temp + (_register.c ? 1 : 0));
    
    _register.n = (int_temp & 0x80) != 0;
    _register.v = ((int_temp ^ _register.a) & (int_temp ^ byte_temp) & 0x80) != 0;
    _register.z = (int_temp & 0xFF) == 0;
    _register.c = (int_temp >> 0x8) != 0;
    _register.a = (unsigned char)(int_temp);
}

void CPU::RTI()
{
    Memory::__instance->read(_register.sp);
    _register.setP(Pull());
    
    _register.pcl = Pull();
    _register.pch = Pull();
}

void CPU::RTS()
{
    Memory::__instance->read(_register.sp);
    _register.pcl = Pull();
    _register.pch = Pull();
    
    _register.pc++;
    
    Memory::__instance->read(_register.pc);
}

void CPU::SAX()
{
    Memory::__instance->write(_register.ea, (unsigned char)(_register.x & _register.a));
}

void CPU::SBC()
{
    _M ^= 0xFF;
    int_temp = (_register.a + _M + (_register.c ? 1 : 0));
    
    _register.n = (int_temp & 0x80) != 0;
    _register.v = ((int_temp ^ _register.a) & (int_temp ^ _M) & 0x80) != 0;
    _register.z = (int_temp & 0xFF) == 0;
    _register.c = (int_temp >> 0x8) != 0;
    _register.a = (unsigned char)(int_temp);
}

void CPU::SHX()
{
    byte_temp = (unsigned char)(_register.x & (_register.eah + 1));
    
    Memory::__instance->read(_register.ea);
    _register.eal += _register.y;
    
    if (_register.eal < _register.y)
        _register.eah = byte_temp;
    
    Memory::__instance->write(_register.ea, byte_temp);
}

void CPU::SHY()
{
    byte_temp = (unsigned char)(_register.y & (_register.eah + 1));
    
    Memory::__instance->read(_register.ea);
    _register.eal += _register.x;
    
    if (_register.eal < _register.x)
        _register.eah = byte_temp;
    Memory::__instance->write(_register.ea, byte_temp);
}

void CPU::SLO()
{
    byte_temp = Memory::__instance->read(_register.ea);
    
    _register.c = (byte_temp & 0x80) != 0;
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    byte_temp <<= 1;
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    _register.n = (byte_temp & 0x80) != 0;
    _register.z = (byte_temp & 0xFF) == 0;
    
    _register.a |= byte_temp;
    _register.n = (_register.a & 0x80) != 0;
    _register.z = (_register.a & 0xFF) == 0;
}

void CPU::SRE()
{
    byte_temp = Memory::__instance->read(_register.ea);
    
    _register.c = (byte_temp & 0x01) != 0;
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    byte_temp >>= 1;
    
    Memory::__instance->write(_register.ea, byte_temp);
    
    _register.n = (byte_temp & 0x80) != 0;
    _register.z = (byte_temp & 0xFF) == 0;
    
    _register.a ^= byte_temp;
    _register.n = (_register.a & 0x80) != 0;
    _register.z = (_register.a & 0xFF) == 0;
}

void CPU::STA()
{
    Memory::__instance->write(_register.ea, _register.a);
}

void CPU::STX()
{
    Memory::__instance->write(_register.ea, _register.x);
}

void CPU::STY()
{
    Memory::__instance->write(_register.ea, _register.y);
}

void CPU::TAX()
{
    _register.x = _register.a;
    _register.n = (_register.x & 0x80) == 0x80;
    _register.z = (_register.x == 0);
}

void CPU::TAY()
{
    _register.y = _register.a;
    _register.n = (_register.y & 0x80) == 0x80;
    _register.z = (_register.y == 0);
}

void CPU::TSX()
{
    _register.x = _register.spl;
    _register.n = (_register.x & 0x80) != 0;
    _register.z = _register.x == 0;
}

void CPU::TXA()
{
    _register.a = _register.x;
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::TXS()
{
    _register.spl = _register.x;
}

void CPU::TYA()
{
    _register.a = _register.y;
    _register.n = (_register.a & 0x80) == 0x80;
    _register.z = (_register.a == 0);
}

void CPU::XAA()
{
    _register.a = (unsigned char)(_register.x & _M);
    _register.n = (_register.a & 0x80) != 0;
    _register.z = (_register.a & 0xFF) == 0;
}

void CPU::XAS()
{
    _register.spl = (unsigned char)(_register.a & _register.x /*& ((dummyVal >> 8) + 1)*/);
    Memory::__instance->write(_register.ea, _register.spl);
}
