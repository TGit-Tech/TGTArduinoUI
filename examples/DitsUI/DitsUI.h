///@file    DitsUI.h
///@brief   TFT & Keypad user interface (UI) for RF-DITS protocol.
#pragma once

#include "Config.h"
#include <rf_dits.h>
#include <Adafruit_ST7735.h>
#include <OneSlot9pt7b.h>
#include <Ebyte.h>
#include "UIBase.h"     // Setup Base Class

class DitsUI
{
public:
  DitsUI::~DitsUI() {   }
  
  void Begin(DITSEngine& _dits, Adafruit_GFX& _gfx, Ebyte& _pEbyte);
  uint8_t ScreenNavigate(uint8_t _Button);

private:
  DITSEngine* pDits = nullptr;
  Adafruit_GFX* pGFX = nullptr;
  Ebyte* pEbyte = nullptr;
  void DumpRadio1DITS();
  UIScreen* OnScreen = nullptr;
  void SGValues(bool _Set = false);
  void NodeScreen(DITSEngine::NodeDIT& node, uint8_t NavKey);
  void SetupScreen(uint8_t NavKey);
  void NodeEditScreen(uint8_t NavKey);
  char* tmpDITName; // General use array for DIT get Names
};