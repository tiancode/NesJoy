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

#ifndef __NesEmulator__Interrupt__
#define __NesEmulator__Interrupt__

class Interrupt
{
public:
    void pollInterruptStatus();
public:
    const int IRQ_APU = 0x1;
    const int IRQ_BOARD = 0x2;
    const int IRQ_DMC = 0x4;
    // Represents the current NMI pin (connected to ppu)
    bool NMI_Current;
    // Represents the old status if NMI pin, used to generate NMI in raising edge
    bool NMI_Old;
    // Determines that NMI is pending (active when NMI pin become true and was false)
    bool NMI_Detected;
    // Determines that IRQ flags (pins)
    int IRQFlags;
    // Determines that IRQ is pending
    bool IRQ_Detected;
    // This is the interrupt vector to jump in the last 2 cycles of BRK/IRQ/NMI
    int interrupt_vector;
    // These flags suspend interrupt polling
    bool interrupt_suspend_nmi;
    bool interrupt_suspend_irq;
    bool nmi_enabled;
    bool nmi_old;
    
    bool vbl_flag;
    bool vbl_flag_temp;
    
public:
    ~Interrupt();
    static Interrupt* getInstance();
    static void destroy();
    
    static Interrupt* __instance;
private:
    Interrupt();
};

#endif /* defined(__NesEmulator__Interrupt__) */
