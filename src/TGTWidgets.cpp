#include "TGTWidgets.h"
#include "TGTDB.h"
#include <string.h>

const uint16_t MultiplierDEC[4] = {1, 10, 100, 1000};
const uint16_t MultiplierHEX[4] = {1, 16, 256, 4096};

TGTWidget::TGTWidget(byte _SelectNum,  Adafruit_GFX* _GFX,  uint8_t _PosX,  uint8_t _PosY,  bool _Settable,  uint16_t _WHandleUID)
  : SelectNum(_SelectNum), GFX(_GFX), PosX(_PosX), PosY(_PosY), Settable(_Settable), WHandleUID(_WHandleUID) {}

int TGTWidget::Value() { return 0; }

bool TGTWidget::Value(int _Value) { return false; }

char* TGTWidget::chValue() { return nullptr; }

bool TGTWidget::chValue(char* _Value) { return false; }

bool TGTWidget::chValue(const char* _Value) { return false; }

bool TGTWidget::AddEnum(int _Value,  const __FlashStringHelper* _Label) { return false; }

bool TGTWidget::Selected() { return bSelected; }

void TGTWidget::Selected(bool _Selected) { if (_Selected != bSelected) {bSelected = _Selected;Display();} }

void TGTWidget::Display() {  }

uint8_t TGTWidget::WidgetNavigate(uint8_t _Button) { return _Button; }

WFLabel::WFLabel(uint8_t _SelectNum,  Adafruit_GFX* _GFX,  uint8_t _PosX,  uint8_t _PosY,  const __FlashStringHelper* _Label,  uint16_t _WHandleUID)
  : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, false, _WHandleUID), Label(_Label) {  }

void WFLabel::Display() {
    GFX->setCursor(PosX, PosY);
    if (bSelected) {GFX->setTextColor(UI_SELCHAR_COLOR);} else {GFX->setTextColor(UI_STDCHAR_COLOR);}
    if (Label!=nullptr) GFX->print(Label);
  }

WChLabel::WChLabel(uint8_t _SelectNum,  Adafruit_GFX* _GFX,  uint8_t _PosX,  uint8_t _PosY, 
           int _charSize,  const char* _Label,  uint16_t _WHandleUID)
  : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, false, _WHandleUID), charSize(_charSize) {
    Label = new char[charSize + 1];
    if(Label != nullptr && _Label != nullptr) { strncpy(Label, _Label, charSize); Label[charSize] = '\0'; }
  }

WChLabel::~WChLabel() { delete[] Label; }

void WChLabel::Display() {
    GFX->setCursor(PosX, PosY);
    if(bSelected) { GFX->setTextColor(UI_SELCHAR_COLOR); }
    else { GFX->setTextColor(UI_STDCHAR_COLOR); }
    if(Label != nullptr) GFX->print(Label);
  }

WCharVal::WCharVal(uint8_t _SelectNum,  Adafruit_GFX* _GFX,  uint8_t _PosX,  uint8_t _PosY,  int _charSize,  bool _Settable,  uint16_t _WHandleUID)
    : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, _Settable, _WHandleUID), charSize(_charSize) {}

bool WCharVal::chValue(char* _Value) { if(EditMode) return false; Valchar = _Value; Display(); return true; }

// Preserve the original const-char overload behavior; the widget stores one shared buffer pointer.
bool WCharVal::chValue(const char* _Value) { if(EditMode) return false; Valchar = const_cast<char*>(_Value); Display(); return true; }

char* WCharVal::chValue() { return Valchar; }

void WCharVal::Display() {
    GFX->fillRect(PosX, PosY-FONT_Y+4, FONT_X*charSize, FONT_Y-2, (EditMode) ? UIKB_NAMEEDIT_BGCOLOR : UI_FILLSCR_COLOR );
    GFX->setCursor(PosX, PosY);
    if(EditMode) {
      GFX->fillRect(PosX, PosY-FONT_Y+4, FONT_X*charSize, FONT_Y-2, UIKB_NAMEEDIT_BGCOLOR);
      GFX->fillRect(PosX+(EditPos*FONT_X),PosY-FONT_Y+4, FONT_X, FONT_Y-2, ST77XX_YELLOW); // Highlight Edit Pos
      GFX->setTextColor(ST77XX_BLACK);
    } else if (bSelected) {
      GFX->setTextColor(UI_SELCHAR_COLOR);
    } else {GFX->setTextColor(UI_STDCHAR_COLOR);}
    if (Valchar!=nullptr) {GFX->print(Valchar);} else {GFX->print(F("nullptr"));}
  }

uint8_t WCharVal::WidgetNavigate(uint8_t _Button) {
    if(_Button==NAVKEYSET) {
      if(bSelected && Settable) {  // This Toggles Edit Mode
        if(!EditMode) { EditMode = true; Display(); return NAVKEYNONE; }
        EditMode = false; Display(); return NAVSETVAL;
      }
    }
    if(EditMode) {
      if(Valchar==nullptr) { DBUIERROR(("WCharVal::WidgetNavigate Valchar==nullptr\n")) return NAVKEYNONE; }
      if(_Button==NAVKEYLEFT) { 
        if(EditPos>0) { EditPos--; Display(); }
        return NAVKEYNONE;
      }
      if(_Button==NAVKEYRIGHT) {
        if(EditPos < charSize - 1) { EditPos++; Display(); }
        return NAVKEYNONE;
      }
      if(_Button==NAVKEYUP) { 
        Valchar[EditPos] = (Valchar[EditPos] & 0xE0) | ((Valchar[EditPos] - 1) & 0x1F);
        if((Valchar[EditPos] & 0x1F) == 31) { Valchar[EditPos] -= 5; }
        Display(); return NAVKEYNONE;
      }
      if(_Button==NAVKEYDOWN) {
        Valchar[EditPos] = (Valchar[EditPos] & 0xE0) | ((Valchar[EditPos] + 1) & 0x1F);
        if((Valchar[EditPos] & 0x1F) == 27) { Valchar[EditPos] -= 27; }
        Display(); return NAVKEYNONE;
      }
      if(_Button==NAVKEYGET) { 
        Valchar[EditPos] += (Valchar[EditPos] < 96) ? 32 : -64;
        Display(); return NAVKEYNONE;
      }
    }
    return _Button;
  }

WNumVal::WNumVal(uint8_t _SelectNum,  Adafruit_GFX* _GFX,  uint8_t _PosX,  uint8_t _PosY, 
      uint8_t _VDataType,  uint16_t _WHandleUID,  uint16_t _Vmin,  uint16_t _Vmax)
      : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, ((_VDataType&TSETMSK)!=TNOSET), _WHandleUID), 
        VDataType(_VDataType), Vmin(_Vmin), Vmax(_Vmax) {
      PerDigit = ((VDataType&TSETMSK)==TSETPD);
      VBase = ((VDataType&TBASEMSK)==THEX) ? HEX : DEC;
      VDataType = (VDataType&TTYPEMSK);
    
      if(VDataType==TBYTE&&Vmax>0xFF) Vmax = 0xFF;
      if(VBase==HEX) {
        uint16_t MaxVal = Vmax; maxDigits = 1;
        while(MaxVal >= 16) { MaxVal /= 16; maxDigits++; }
      } else {
        int16_t MinVal = (VDataType==TSINT) ? (int16_t)Vmin : Vmin;
        uint16_t Largest = (MinVal < 0) ? -MinVal : Vmax;
        if(Vmax > Largest) Largest = Vmax;
        maxDigits = (MinVal < 0) ? 1 : 0;
        do { maxDigits++; Largest /= 10;} 
        while(Largest);
      }
    }

int WNumVal::Value() { return (int16_t)uiValue; }

bool WNumVal::Value(int _Value) {
      if(EditMode) return false;
      if(_Value==uiValue) return true;
      bool bReturn = true;
      if(VDataType == TSINT) {
        if((int)_Value>(int)Vmax) { bReturn=false; _Value=Vmax; }
        if((int)_Value<(int)Vmin) { bReturn=false; _Value=Vmin; }
      } else {
        if(_Value>Vmax) { bReturn=false; _Value=Vmax; }
        if(_Value<Vmin) { bReturn=false; _Value=Vmin; }
      }
      uiValue = (uint16_t)_Value; Display();
      return bReturn;
    }

void WNumVal::Display() {
      GFX->setCursor(PosX, PosY);
      // Background
      if(PerDigit&&EditMode)  GFX->fillRect(PosX, PosY-FONT_Y+4, FONT_X*maxDigits, FONT_Y-2, UIKB_NAMEEDIT_BGCOLOR);
      else if(EditMode)       GFX->fillRect(PosX, PosY-FONT_Y+4, FONT_X*maxDigits, FONT_Y-2, ST77XX_YELLOW);
      else                    GFX->fillRect(PosX, PosY-FONT_Y+4, FONT_X*maxDigits, FONT_Y-2, UI_FILLSCR_COLOR);
      // Text
      if(PerDigit&&EditMode) {
        GFX->fillRect(PosX+((maxDigits-1-EditPos)*FONT_X), (PosY-FONT_Y)+4, FONT_X, FONT_Y-2, ST77XX_YELLOW); // Per-Digit
                                     GFX->setTextColor(ST77XX_BLACK); }
      else if(EditMode&&bSelected)  {GFX->setTextColor(ST77XX_BLACK); }
      else if(bSelected)            {GFX->setTextColor(UI_SELCHAR_COLOR);}
      else                          {GFX->setTextColor(UI_STDCHAR_COLOR);}
      // Print
      if(VDataType==TSINT && VBase==DEC) { GFX->print((int16_t)uiValue, DEC);}
      else {
        uint16_t TmpValue = uiValue;
        byte Digits = 1;
        while((TmpValue /= VBase) > 0) Digits++;
        while(Digits++ < maxDigits) GFX->print('0');
        GFX->print(uiValue, VBase);
      }
    }

uint8_t WNumVal::WidgetNavigate(uint8_t _Button) {
      if(_Button==NAVKEYSET) {
        if(bSelected && Settable) {  // This Toggles Edit Mode
          if(!EditMode) { EditMode = true; Display(); return NAVKEYNONE; }
          EditMode = false; Display(); return NAVSETVAL;
        }
      }
      if(EditMode) {
        if(_Button==NAVKEYLEFT) { 
          if(!PerDigit) return NAVKEYNONE;
          if(EditPos<(maxDigits-1)) { EditPos++; Display(); return NAVKEYNONE; } 
        }
        if(_Button==NAVKEYRIGHT) { 
          if(!PerDigit) return NAVKEYNONE;
          if(EditPos>0) { EditPos--; Display(); return NAVKEYNONE; } 
        }
        if(_Button==NAVKEYUP) {
          if(PerDigit) {
            uint16_t Step = (VBase==HEX) ? MultiplierHEX[EditPos] : MultiplierDEC[EditPos];
            if(((uiValue/Step)%VBase)==(VBase-1)) { uiValue -= (uint16_t)((VBase-1)*Step); }
            else { uiValue += Step; }
          } else {
            if(uiValue<Vmax) uiValue++;
          }
          if(VDataType == TSINT) {if((int)uiValue>(int)Vmax) uiValue=Vmax; if((int)uiValue<(int)Vmin) uiValue=Vmin;}
          else {if(uiValue>Vmax) uiValue=Vmax; if(uiValue<Vmin) uiValue=Vmin;}
          Display(); return NAVKEYNONE;
        }
        if(_Button==NAVKEYDOWN) {
          if(PerDigit) {
            uint16_t Step = (VBase==HEX) ? MultiplierHEX[EditPos] : MultiplierDEC[EditPos];
            if(((uiValue/Step)%VBase)==0) { uiValue += (uint16_t)((VBase-1)*Step); }
            else { uiValue -= Step; }
          } else {
            if(uiValue>Vmin) uiValue--;
          }
          if(VDataType == TSINT) {if((int)uiValue>(int)Vmax) uiValue=Vmax; if((int)uiValue<(int)Vmin) uiValue=Vmin;}
          else {if(uiValue>Vmax) uiValue=Vmax; if(uiValue<Vmin) uiValue=Vmin;}
          Display(); return NAVKEYNONE;
        }
      }
      return _Button;
    }

WEnumVal::WEnumVal(uint8_t _SelectNum,  Adafruit_GFX* _GFX,  uint8_t _PosX,  uint8_t _PosY,  bool _Settable,  uint16_t _WHandleUID)
  : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, _Settable, _WHandleUID) {}

bool WEnumVal::AddEnum(int _iVal,  const __FlashStringHelper* _Label ) {
    if(EnumIdx>=MAXWIDGETENUMS) { return false; } // Array Guard
    if(_Label==nullptr) return false;
    byte Len = strlen_P((PGM_P)_Label);
    if(Len > MaxChars) MaxChars = Len;
    EnumVal[EnumIdx] = _iVal;
    EnumLbls[EnumIdx] = _Label;
    EnumIdx++; Display();
    return true;
  }

int WEnumVal::Value() { return iEVal; }

bool WEnumVal::Value(int _Value) { 
    if(EditMode) return false;
    if(_Value==iEVal) return true;
    iEVal = _Value;
    GFX->setCursor(PosX, PosY);
    Display();
    return true; 
  }

void WEnumVal::Display() {
    GFX->fillRect(PosX, PosY-FONT_Y+4, FONT_X*MaxChars, FONT_Y-2, (EditMode) ? ST77XX_YELLOW : UI_FILLSCR_COLOR);
    GFX->setCursor(PosX, PosY);

    if(EditMode) { GFX->setTextColor(ST77XX_BLACK); }
    else if(bSelected) { GFX->setTextColor(UI_SELCHAR_COLOR); }
    else { GFX->setTextColor(UI_STDCHAR_COLOR); }

    bEValFound = false;
    for(int i=0;i<EnumIdx;i++) {                   // Print Enum
      if(EnumVal[i]==iEVal) {
        bEValFound = true;
        if (EnumLbls[i]!=nullptr) { GFX->print(EnumLbls[i]); }
        else { GFX->print("?P");} // ?P = nullptr
        break;
      }
    }
    if(!bEValFound) { GFX->print("?E"); } // ?E = NoEnum
  }

uint8_t WEnumVal::WidgetNavigate(uint8_t _Button) {
    if(_Button==NAVKEYSET) {
      DBUIINFO(("WEnumVal::NAVKEYSET"))
      if(bSelected && Settable && EnumIdx>0) {  // This Toggles Edit Mode
        if(!EditMode) { EditMode = true; Display(); return NAVKEYNONE; }
        EditMode = false; Display(); return NAVSETVAL;
      }
    }
    if(EditMode) {
      if(_Button==NAVKEYUP) { 
        if(!bEValFound) {iEVal=EnumVal[EnumIdx-1]; }
        else if(EditPos>0) {EditPos--; iEVal = EnumVal[EditPos]; }
        else { EditPos=EnumIdx-1; iEVal = EnumVal[EditPos]; }
        Display();
      }
      if(_Button==NAVKEYDOWN) {
        if(!bEValFound) {iEVal=EnumVal[0]; }
        else if(EditPos+1<EnumIdx) {EditPos++; iEVal=EnumVal[EditPos]; }
        else { EditPos=0; iEVal=EnumVal[EditPos]; }
        Display();
      } 
      return NAVKEYNONE;
    }
    return _Button;
  }

TGTScreen::TGTScreen(Adafruit_GFX& _GFX,  uint16_t _NScrIdx) : pGFX(&_GFX), NScrIdx(_NScrIdx) {}

TGTScreen::~TGTScreen() { for (uint8_t i = 0; i < WIdx; i++) { delete Widget[i]; Widget[i] = nullptr; } }

void TGTScreen::ClearScreen() {
      pGFX->fillScreen(UI_FILLSCR_COLOR);
      pGFX->drawFastHLine(0, FONT_Y+2, TFT_WIDTH, UI_STDLINE_COLOR);
      ScreenCleared = true;
    }

uint16_t TGTScreen::SelectedWHandleUID() { 
      if(pSelected!=nullptr) return pSelected->WHandleUID; return WHNONE; 
    }

char* TGTScreen::SelectedWchValue() {
      if(pSelected==nullptr) return nullptr;
      return pSelected->chValue();
    }

uint16_t TGTScreen::SelectedWValue() {
      if(pSelected==nullptr) return WHNONE;
      return pSelected->Value();
    }

bool TGTScreen::gotValue(uint16_t _NScrIdx,  uint16_t _WHandleUID,  int _Value) {
      if (_NScrIdx!=NScrIdx) return false;
      for (int i=0; i<WIdx; i++) {
        if(Widget[i]->WHandleUID==_WHandleUID) { Widget[i]->Value(_Value); return true; }
      } return false;
    }

bool TGTScreen::gotValue(uint16_t _NScrIdx,  uint16_t _WHandleUID,  char* _Value) {
      if (_NScrIdx!=NScrIdx) return false;
      for (int i=0; i<WIdx; i++) {
        if(Widget[i]->WHandleUID==_WHandleUID) { Widget[i]->chValue(_Value); return true; }
      } return false; 
    }

bool TGTScreen::gotValue(uint16_t _NScrIdx,  uint16_t _WHandleUID,  const char* _Value) {
      if (_NScrIdx!=NScrIdx) return false;
      for (int i=0; i<WIdx; i++) {
        if(Widget[i]->WHandleUID==_WHandleUID) { Widget[i]->chValue(_Value); return true; }
      } return false;      
    }

uint8_t TGTScreen::OnScreenNavigate(uint8_t _Button) {
      DBUIAENTER((_Button),("TGTScreen::Navigate(_Button)\n"))
      if(pSelected==nullptr) {DBUIERROR(("TGTScreen::OnScreenNavigate pSelected==nullptr\n")) return _Button;}
      uint8_t NavRtn = pSelected->WidgetNavigate(_Button); // Pass Button to Selected Widget

      if(NavRtn==NAVKEYUP) {
        DBUIINFO(("TGTScreen::OnScreenNavigate NavRtn==NAVKEYUP Prev Widget\n"))
        for (int s=pSelected->SelectNum - 1; s>0; s--) {
          for (int i=0; i<WIdx; i++) {
            if(Widget[i]->SelectNum==s) {
              pSelected->Selected(false); pSelected = Widget[i]; pSelected->Selected(true);
              return NAVKEYNONE;
            }
          }
        } 
      }
      if(NavRtn==NAVKEYDOWN) {
        DBUIINFO(("TGTScreen::OnScreenNavigate NavRtn==NAVKEYDOWN Next Widget\n"))
        for (int s=pSelected->SelectNum + 1; s<=LastSelectNum; s++) {
          for (int i=0; i<WIdx; i++) {
            if(Widget[i]->SelectNum==s) {
              pSelected->Selected(false); pSelected = Widget[i]; pSelected->Selected(true);
              return NAVKEYNONE;
            }
          }
        }     
      }
      return NavRtn;
    }

bool TGTScreen::NewWFLabel(uint8_t _SelectNum,  uint8_t _PosX,  uint8_t _PosY, 
                    const __FlashStringHelper* _Label, 
                    uint16_t _WHandleUID) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WFLabel(_SelectNum,pGFX,_PosX,_PosY,_Label,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }

bool TGTScreen::NewWChLabel(uint8_t _SelectNum,  uint8_t _PosX,  uint8_t _PosY, 
                     int _charSize,  char* _Label,  
                     uint16_t _WHandleUID) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WChLabel(_SelectNum,pGFX,_PosX,_PosY,_charSize,_Label,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }

bool TGTScreen::NewWNumVal(uint8_t _SelectNum,  uint8_t _PosX,  uint8_t _PosY, 
                    uint8_t _VDataType,  uint16_t _WHandleUID, 
                    uint16_t _Vmin,  uint16_t _Vmax,  
                    const uint16_t* _Omit,  uint8_t _OmitSize) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WNumVal(_SelectNum,pGFX,_PosX,_PosY,_VDataType,_WHandleUID,_Vmin,_Vmax);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }

bool TGTScreen::NewWCharVal(uint8_t _SelectNum,  uint8_t _PosX,  uint8_t _PosY, 
                     int _charSize,  bool _Settable, 
                     uint16_t _WHandleUID) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WCharVal(_SelectNum, pGFX,_PosX,_PosY,_charSize,_Settable,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }

bool TGTScreen::NewWEnumVal(uint8_t _SelectNum,  uint8_t _PosX,  uint8_t _PosY, 
                     bool _Settable,  
                     uint16_t _WHandleUID) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WEnumVal(_SelectNum,pGFX,_PosX,_PosY,_Settable,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }

bool TGTScreen::AddEnum(uint16_t _WHandleUID,  int _iVal,  const __FlashStringHelper* _Label) {
      for(byte i = 0; i < WIdx; i++) {
        if(Widget[i]->WHandleUID == _WHandleUID) { return Widget[i]->AddEnum(_iVal,_Label); }
      }
      return false;
    }
