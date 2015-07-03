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


#include "NesCartDatabase.h"
#include "util.h"

#include "tinyxml2/tinyxml2.h"

NesCartDatabase::NesCartDatabase()
{

}
NesCartDatabase::~NesCartDatabase()
{
    if(!_romDatabase.empty())
    {
        for( std::list<NesCartDatabaseGameElement*>::iterator it = _romDatabase.begin(); it != _romDatabase.end(); it++)
        {
            delete *it;
        }
        _romDatabase.clear();
    }
}

bool NesCartDatabase::loadDatabase(const char* database)
{
    if(!_romDatabase.empty())
    {
        for( std::list<NesCartDatabaseGameElement*>::iterator it = _romDatabase.begin(); it != _romDatabase.end(); it++)
        {
            delete *it;
        }
    }
    _romDatabase.clear();
    
    std::string wholeStr = database;
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
    tinyxml2::XMLError err = pDoc->Parse(wholeStr.c_str());
    if(err != 0)
    {
        printf("xmlLoad error");
        return false;
    }
    
    tinyxml2::XMLElement *rootEle = pDoc->RootElement();
    const tinyxml2::XMLAttribute *pAttri = rootEle->FirstAttribute();
    while(pAttri != NULL)
    {
        if( strcmp(pAttri->Name(), "version") == 0 )
        {
            DBVersion = pAttri->Value();
        }
        if( strcmp(pAttri->Name(), "conformance") == 0 )
        {
            DBConformance = pAttri->Value();
        }
        if( strcmp(pAttri->Name(), "agent") == 0 )
        {
            DBAgent = pAttri->Value();
        }
        if( strcmp(pAttri->Name(), "author") == 0 )
        {
            DBAuthor = pAttri->Value();
        }
        if( strcmp(pAttri->Name(), "timestamp") == 0 )
        {
            DBTimeStamp = pAttri->Value();
        }
        pAttri = pAttri->Next();
    }
    
    tinyxml2::XMLElement *nextElement = rootEle->FirstChildElement();
    while (nextElement)
    {
        NesCartDatabaseGameElement *drom = new NesCartDatabaseGameElement();
        drom->Game_Name = "";
        drom->Game_AltName = "";
        drom->Game_Catalog = "";
        drom->Game_Class = "";
        drom->Game_Developer = "";
        drom->Game_Players = "";
        drom->Game_Publisher = "";
        drom->Game_Region = "";
        drom->Game_ReleaseDate = "";
        const tinyxml2::XMLAttribute *dromAttri = nextElement->FirstAttribute();
        while (dromAttri)
        {
            if( strcmp(dromAttri->Name(), "name") == 0 )
            {
                drom->Game_Name = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "altname") == 0 )
            {
                drom->Game_AltName = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "class") == 0 )
            {
                drom->Game_Class = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "catalog") == 0 )
            {
                drom->Game_Catalog = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "publisher") == 0 )
            {
                drom->Game_Publisher = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "developer") == 0 )
            {
                drom->Game_Developer = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "region") == 0 )
            {
                drom->Game_Region = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "players") == 0 )
            {
                drom->Game_Players = dromAttri->Value();
            }
            if( strcmp(dromAttri->Name(), "date") == 0 )
            {
                drom->Game_ReleaseDate = dromAttri->Value();
            }
            
            dromAttri = dromAttri->Next();
        }
        
        tinyxml2::XMLElement *nextCartridgeElement = nextElement->FirstChildElement();
        while(nextCartridgeElement)
        {
            if( strcmp(nextCartridgeElement->Value(), "cartridge") == 0 )
            {
                NesCartDatabaseCartridgeElement cartridge;
                cartridge.System = "";
                cartridge.CRC = "";
                cartridge.SHA1 = "";
                cartridge.Dump = "";
                cartridge.Dumper = "";
                cartridge.DateDumped = "";
                cartridge.PAD_h = "";
                cartridge.PAD_v = "";
                cartridge.PRG_crc = "";
                cartridge.PRG_name = "";
                cartridge.PRG_sha1 = "";
                cartridge.PRG_size = "";
                cartridge.CHR_crc = "";
                cartridge.CHR_name = "";
                cartridge.CHR_sha1 = "";
                cartridge.CHR_size = "";
                cartridge.CIC_type = "";
                cartridge.Board_Mapper = "";
                cartridge.Board_Pcb = "";
                cartridge.Board_Type = "";
                const tinyxml2::XMLAttribute *cartridgeAttri = nextCartridgeElement->FirstAttribute();
                while (cartridgeAttri)
                {
                    if( strcmp(cartridgeAttri->Name(), "system") == 0 )
                    {
                        cartridge.System = cartridgeAttri->Value();
                    }
                    if( strcmp(cartridgeAttri->Name(), "crc") == 0 )
                    {
                        cartridge.CRC = cartridgeAttri->Value();
                    }
                    if( strcmp(cartridgeAttri->Name(), "sha1") == 0 )
                    {
                        cartridge.SHA1 = cartridgeAttri->Value();
                    }
                    if( strcmp(cartridgeAttri->Name(), "dump") == 0 )
                    {
                        cartridge.Dump = cartridgeAttri->Value();
                    }
                    if( strcmp(cartridgeAttri->Name(), "dumper") == 0 )
                    {
                        cartridge.Dumper = cartridgeAttri->Value();
                    }
                    if( strcmp(cartridgeAttri->Name(), "datedumped") == 0 )
                    {
                        cartridge.DateDumped = cartridgeAttri->Value();
                    }
                    
                    cartridgeAttri = cartridgeAttri->Next();
                }
                
                tinyxml2::XMLElement* board = nextCartridgeElement->FirstChildElement();
                const tinyxml2::XMLAttribute *boardAttri = board->FirstAttribute();
                while (boardAttri)
                {
                    if( strcmp(boardAttri->Name(), "type") == 0 )
                    {
                        cartridge.Board_Type = boardAttri->Value();
                    }
                    if( strcmp(boardAttri->Name(), "pcb") == 0 )
                    {
                        cartridge.Board_Pcb = boardAttri->Value();
                    }
                    if( strcmp(boardAttri->Name(), "mapper") == 0 )
                    {
                        cartridge.Board_Mapper = boardAttri->Value();
                    }
                    
                    boardAttri = boardAttri->Next();
                }
                
                tinyxml2::XMLElement* t = board->FirstChildElement();
                while(t)
                {
                    if(strcmp(t->Name(), "prg") == 0)
                    {
                        const tinyxml2::XMLAttribute *prgAttrit = t->FirstAttribute();
                        while(prgAttrit)
                        {
                            if( strcmp(prgAttrit->Name(), "name") == 0 )
                            {
                                cartridge.PRG_name = prgAttrit->Value();
                            }
                            if( strcmp(prgAttrit->Name(), "size") == 0 )
                            {
                                cartridge.PRG_size = prgAttrit->Value();
                            }
                            if( strcmp(prgAttrit->Name(), "crc") == 0 )
                            {
                                cartridge.PRG_crc = prgAttrit->Value();
                            }
                            if( strcmp(prgAttrit->Name(), "sha1") == 0 )
                            {
                                cartridge.PRG_sha1 = prgAttrit->Value();
                            }
                            
                            prgAttrit = prgAttrit->Next();
                        }
                    }
                    
                    if(strcmp(t->Name(), "chr") == 0)
                    {
                        const tinyxml2::XMLAttribute *chrAttrit = t->FirstAttribute();
                        while(chrAttrit)
                        {
                            if( strcmp(chrAttrit->Name(), "name") == 0 )
                            {
                                cartridge.CHR_name = chrAttrit->Value();
                            }
                            if( strcmp(chrAttrit->Name(), "size") == 0 )
                            {
                                cartridge.CHR_size = chrAttrit->Value();
                            }
                            if( strcmp(chrAttrit->Name(), "crc") == 0 )
                            {
                                cartridge.CHR_crc = chrAttrit->Value();
                            }
                            if( strcmp(chrAttrit->Name(), "sha1") == 0 )
                            {
                                cartridge.CHR_sha1 = chrAttrit->Value();
                            }
                            
                            chrAttrit = chrAttrit->Next();
                        }
                    }
                    
                    if(strcmp(t->Name(), "vram") == 0)
                    {
                        const tinyxml2::XMLAttribute *vramAttrit = t->FirstAttribute();
                        while(vramAttrit)
                        {
                            if( strcmp(vramAttrit->Name(), "size") == 0 )
                            {
                                cartridge.VRAM_sizes.push_back(vramAttrit->Value());
                            }
                            vramAttrit = vramAttrit->Next();
                        }
                    }
                    
                    if( strcmp(t->Name(), "wram") == 0 )
                    {
                        SRAMBankInfo wram;
                        memset(&wram, 0, sizeof(SRAMBankInfo));
                        const tinyxml2::XMLAttribute *wramAttrit = t->FirstAttribute();
                        while(wramAttrit)
                        {
                            if( strcmp(wramAttrit->Name(), "size") == 0 )
                            {
                                wram.SIZE = wramAttrit->Value();
                            }
                            if( strcmp(wramAttrit->Name(), "battery") == 0 )
                            {
                                wram.BATTERY = atoi(wramAttrit->Value()) == 1;
                            }
                            if( strcmp(wramAttrit->Name(), "id") == 0 )
                            {
                                wram.id = atoi(wramAttrit->Value());
                            }
                            
                            wramAttrit = wramAttrit->Next();
                        }
                        cartridge.WRAMBanks.push_back(wram);
                    }
                    
                    if( strcmp(t->Name(), "chip") == 0 )
                    {
                        const tinyxml2::XMLAttribute *chipAttrit = t->FirstAttribute();
                        while(chipAttrit)
                        {
                            if( strcmp(chipAttrit->Name(), "type") == 0 )
                            {
                                cartridge.chip_type.push_back(chipAttrit->Value());
                            }
                            chipAttrit = chipAttrit->Next();
                        }
                    }
                    
                    if( strcmp(t->Name(), "cic") == 0 )
                    {
                        const tinyxml2::XMLAttribute *cicAttrit = t->FirstAttribute();
                        while(cicAttrit)
                        {
                            if( strcmp(cicAttrit->Name(), "type") == 0 )
                            {
                                cartridge.CIC_type = cicAttrit->Value();
                            }
                            cicAttrit = cicAttrit->Next();
                        }
                    }
                    
                    if( strcmp(t->Name(), "pad") == 0 )
                    {
                        const tinyxml2::XMLAttribute *padAttrit = t->FirstAttribute();
                        if( strcmp(padAttrit->Name(), "h") == 0 )
                        {
                            cartridge.PAD_h = padAttrit->Value();
                        }
                        if (strcmp(padAttrit->Name(), "v") == 0 )
                        {
                            cartridge.PAD_v = padAttrit->Value();
                        }
                        
                        padAttrit = padAttrit->Next();
                    }
                    
                    t = t->NextSiblingElement();
                }
                
                drom->Cartridges.push_back(cartridge);
                nextCartridgeElement = nextCartridgeElement->NextSiblingElement();
                continue;
            }
            
            nextCartridgeElement = nextCartridgeElement->NextSiblingElement();
        }
        _romDatabase.push_back(drom);
        nextElement = nextElement->NextSiblingElement();
    }
    delete pDoc;

    return true;
}


NesCartDatabaseGameElement* NesCartDatabase::findRom( std::string Cart_sha1 )
{
    for( std::list<NesCartDatabaseGameElement*>::iterator it = _romDatabase.begin(); it != _romDatabase.end(); it++)
    {
        for(std::list<NesCartDatabaseCartridgeElement>::iterator crc = (*it)->Cartridges.begin(); crc != (*it)->Cartridges.end(); crc++)
        {
            if( toLower(&crc->SHA1) == toLower(&Cart_sha1))
            {
                return *it;
            }
        }
    }
    return NULL;
}
