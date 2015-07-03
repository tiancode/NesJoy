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

#include "Interrupt.h"
#include "CPU.h"
#include <stddef.h>

Interrupt* Interrupt::__instance = NULL;

Interrupt* Interrupt::getInstance()
{
    if( __instance == NULL )
    {
        __instance = new Interrupt;
    }
    return __instance;
}

void Interrupt::destroy()
{
    if( __instance != NULL )
    {
        delete __instance;
        __instance = NULL;
    }
}

Interrupt::Interrupt()
{
    IRQFlags = 0;
}

Interrupt::~Interrupt()
{
    
}

void Interrupt::pollInterruptStatus()
{
    if (!interrupt_suspend_nmi)
    {
        // The edge detector, see if nmi occurred.
        if (NMI_Current & !NMI_Old) // Raising edge, set nmi request
            NMI_Detected = true;
        NMI_Old = NMI_Current = false;// NMI detected or not, low both lines for this form ___|-|__
    }
    if (!interrupt_suspend_irq)
    {
        // irq level detector
        IRQ_Detected = (!CPU::__instance->_register.i && IRQFlags != 0);
    }
    // Update interrupt vector !
    interrupt_vector = NMI_Detected ? 0xFFFA : 0xFFFE;
}
