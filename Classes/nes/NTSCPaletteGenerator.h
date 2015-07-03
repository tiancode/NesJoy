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

#ifndef __NesEmulator__NTSCPaletteGenerator__
#define __NesEmulator__NTSCPaletteGenerator__

#include <stddef.h>

class NTSCPaletteGenerator
{
public:
    const float default_saturation = 1.5F;
    const float default_hue_tweak = 0.0F;
    const float default_contrast = 1.2F;
    const float default_brightness = 1.07F;
    const float default_gamma = 1.8F;
    // Voltage levels, relative to synch voltage
    float black = 0.518F;
    float white = 1.962F;
    float attenuation = 0.746F;
    
    float saturation = 1.5F;
    float hue_tweak = 0.0F;
    float contrast = 1.2F;
    float brightness = 1.07F;
    float gamma = 1.8F;
    
    int *pal = NULL;
    
    int wave(int p, int color);
    float gammafix(float f, float gamma);
    int clamp(float v);
    int MakeRGBcolor(int pixel);
    // Generate the palette with format ARGB (include alpha)  return * rember free
    int* GeneratePalette();
    
    // Generate the palette with format AGBR (include alpha)
    int* GeneratePaletteGBR();
    
    NTSCPaletteGenerator();
    ~NTSCPaletteGenerator();
};

#endif /* defined(__NesEmulator__NTSCPaletteGenerator__) */
