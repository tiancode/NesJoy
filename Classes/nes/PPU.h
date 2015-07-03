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

#ifndef __NesEmulator__PPU__
#define __NesEmulator__PPU__

class PPU
{
public:
    void hardReset();
    void SetPalette(int* newPalette);
    void SetupVideoRenderer(int* scan0, int scanStart, int scanSize);
    void SetupFrameSkip(bool frameSkipEnabled, unsigned char frameSkipReload);
    int GetPixel(int x, int y);
    bool IsRenderingOn();
    bool IsInRender();
    
    void PPUSoftReset();
    void PPUShutdown();
    
    void clock();
private:
    int* screenPointer;
    int screenPointerPos;
    int screenPointerSize;
    int screenPointerStart;
    bool hideForbbidenLines;
    int forbbidenLinesCount;
    int* palette;
    int* screen;
    int* bkg_pixels;
    int* spr_pixels;
    // Frame Timing
public:
    int vbl_vclock_Start;
    int vbl_vclock_End;
    int frameEnd;
public:
    int VClock;
    int HClock;
private:
    bool UseOddFrame;
    bool oddSwap;
    
    // Frame skip
    bool FrameSkipEnabled;
    unsigned char FrameSkipTimer;
    unsigned char FrameSkipReload;
    int current_pixel;
    int temp;
    int temp_comparator;
    int bkg_pos;
    int spr_pos;
    int object0;
    int infront;
    int bkgPixel;
    int sprPixel;
    int bkg_fetch_address;
    unsigned char bkg_fetch_nametable;
    unsigned char bkg_fetch_attr;
    unsigned char bkg_fetch_bit0;
    unsigned char bkg_fetch_bit1;
    int spr_fetch_address;
    unsigned char spr_fetch_bit0;
    unsigned char spr_fetch_bit1;
    unsigned char spr_fetch_attr;
    bool* spr_zero_buffer;
    // VRAM Address register
public:
    int vram_temp;
    int vram_address;
    int vram_address_temp_access;
    int vram_address_temp_access1;
    int vram_increament;
public:
    bool vram_flipflop;
public:
    unsigned char vram_fine;
    unsigned char reg2007buffer;
    // Background
    bool bkg_enabled;
    bool bkg_clipped;
    int bkg_patternAddress;
    // Sprites
    bool spr_enabled;
    bool spr_clipped;
    int spr_patternAddress;
    int spr_size16;
public:
    bool spr_0Hit;
    bool spr_overflow;
public:
    // Colors
    int grayscale;
    int emphasis;
    // OAM
public:
    unsigned char oam_address;
private:
    unsigned char oam_fetch_data;
    unsigned char oam_evaluate_slot;
    unsigned char oam_evaluate_count;
    bool oam_fetch_mode;
    unsigned char oam_phase_index;
    int spr_render_i;
    int bkg_render_i;
    int spr_evaluation_i;
    int spr_render_temp_pixel;
public:
    // Others
    unsigned char ppu_2002_temp;
    unsigned char ppu_2004_temp;
    unsigned char ppu_2007_temp;
    
public:
    ~PPU();
    static PPU* getInstance();
    static void destroy();
    
public:
    int* paletteIndexes;
    int paletteIndexesSize;
    

    static PPU *__instance;
private:
    PPU();
};

#endif /* defined(__NesEmulator__PPU__) */
