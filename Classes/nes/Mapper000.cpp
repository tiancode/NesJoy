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

#include "Mapper000.h"
#include "Board.h"

Mapper000::Mapper000()
{
    BoardInfo info("NROM", 0);
    
    Board::Name = info._BoardName;
    Board::MapperNumber = info._INESMapper;
    Board::_prg_ram_default_08KB_count = info._DefaultPRG_RAM_8KB_BanksCount;
    Board::_chr_ram_1KB_default_banks_count = info._DefaultCHR_RAM_1KB_BanksCount;
    Board::_enabled_ppuA12ToggleTimer = info._Enabled_ppuA12ToggleTimer;
    Board::_ppuA12TogglesOnRaisingEdge = info._PPUA12TogglesOnRaisingEdge;
}

