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

#ifndef __NesEmulator__CPU__
#define __NesEmulator__CPU__

struct Register
{
    unsigned char a;
    unsigned char x;
    unsigned char y;
    // PC
    union
    {
        unsigned short pc;
        struct
        {
            unsigned char pcl;
            unsigned char pch;
        };
    };
    // EA
    union
    {
        unsigned short ea;
        struct
        {
            unsigned char eal;
            unsigned char eah;
        };
    };
    // SP
    union
    {
        unsigned short sp;
        struct
        {
            unsigned char spl;
            unsigned char sph;
        };
    };
    
    // flags
    bool n;
    bool v;
    bool d;
    bool i;
    bool z;
    bool c;
    
    // P
    inline unsigned char getP()
    {
        return (unsigned char)(
                               (n ? 0x80 : 0) |
                               (v ? 0x40 : 0) |
                               (d ? 0x08 : 0) |
                               (i ? 0x04 : 0) |
                               (z ? 0x02 : 0) |
                               (c ? 0x01 : 0) | 0x20);
    }
    inline void setP(unsigned char flag)
    {
        n = (flag & 0x80) != 0;
        v = (flag & 0x40) != 0;
        d = (flag & 0x08) != 0;
        i = (flag & 0x04) != 0;
        z = (flag & 0x02) != 0;
        c = (flag & 0x01) != 0;
    }
    
    inline unsigned char pb()
    {
        return (unsigned char)(
                               (n ? 0x80 : 0) |
                               (v ? 0x40 : 0) |
                               (d ? 0x08 : 0) |
                               (i ? 0x04 : 0) |
                               (z ? 0x02 : 0) |
                               (c ? 0x01 : 0) | 0x30);
    }
};

class CPU
{
public:
    void setCpuSpeed(int cpuSpeedInHz);
    void hardReset();
    void softReset();
    void clock();

public:
    Register _register;
    int cpuSpeedInHz;
private:
    unsigned char _opcode;
    unsigned char _M;
    unsigned char dummy;
private:
    unsigned char byte_temp;
    int int_temp;
    int int_temp1;
private:
    inline void IndirectX_R();
    inline void IndirectX_W();
    inline void IndirectX_RW();
    inline void IndirectY_R();
    inline void IndirectY_W();
    inline void IndirectY_RW();
    inline void ZeroPage_R();
    inline void ZeroPage_W();
    inline void ZeroPage_RW();
    inline void ZeroPageX_R();
    inline void ZeroPageX_W();
    inline void ZeroPageX_RW();
    inline void ZeroPageY_R();
    inline void ZeroPageY_W();
    inline void ZeroPageY_RW();
    inline void Immediate();
    inline void ImpliedAccumulator();
    inline void Absolute_R();
    inline void Absolute_W();
    inline void Absolute_RW();
    inline void AbsoluteX_R();
    inline void AbsoluteX_W();
    inline void AbsoluteX_RW();
    inline void AbsoluteY_R();
    inline void AbsoluteY_W();
    inline void AbsoluteY_RW();
    
    inline void Push(unsigned char val);
    inline char Pull();
    inline void Interrupt();
    inline void Branch(bool condition);
    
    inline void ADC();
    inline void AHX();
    inline void ALR();
    inline void ANC();
    inline void AND();
    inline void ARR();
    inline void AXS();
    inline void ASL_M();
    inline void ASL_A();
    inline void BIT();
    inline void BRK();
    inline void CMP();
    inline void CPX();
    inline void CPY();
    inline void DCP();
    inline void DEC();
    inline void DEY();
    inline void DEX();
    inline void EOR();
    inline void INC();
    inline void INX();
    inline void INY();
    inline void ISC();
    inline void JMP_I();
    inline void JSR();
    inline void LAR();
    inline void LAX();
    inline void LDA();
    inline void LDX();
    inline void LDY();
    inline void LSR_A();
    inline void LSR_M();
    inline void ORA();
    inline void PHA();
    inline void PHP();
    inline void PLA();
    inline void PLP();
    inline void RLA();
    inline void ROL_A();
    inline void ROL_M();
    inline void ROR_A();
    inline void ROR_M();
    inline void RRA();
    inline void RTI();
    inline void RTS();
    inline void SAX();
    inline void SBC();
    inline void SHX();
    inline void SHY();
    inline void SLO();
    inline void SRE();
    inline void STA();
    inline void STX();
    inline void STY();
    inline void TAX();
    inline void TAY();
    inline void TSX();
    inline void TXA();
    inline void TXS();
    inline void TYA();
    inline void XAA();
    inline void XAS();
    
public:
    static CPU* getInstance();
    static void destroy();
    ~CPU();

    static CPU* __instance;
private:
    CPU();
};

#endif /* defined(__NesEmulator__CPU__) */
