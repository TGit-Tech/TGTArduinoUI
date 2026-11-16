/*
Notes about Navigate:
- Button Presses should go as far as they can un-touched (i.e. Down to the Widget)
- if Key preforms Widget Event then return NAVKEYNONE or a Specific Event Driver.
- else the Key will return to OnScreen for OnScreen Events.
- therefore; Returned key presses can be used by upper-level navigation.
- Note: The general flow will be processed events will return immediately while unused keys drop to a main Return.
- In this way the KEY in incepted by the object looking-for/using it.
*/
#pragma once

#include "TGTWidgetsConfig.h"
#include <Adafruit_ST7735.h>
#include <avr/pgmspace.h>

#define TFT_ROWS        (TFT_HEIGHT / FONT_Y)
#define TFT_ROW_Y(row)  (((row) + 1) * FONT_Y)  // Position by bottom-left so row 0; actually bottom row 1.
#define TFT_COL_X(col)  (((col) + 0) * FONT_X)
#define CHARSPERROW     (TFT_WIDTH / FONT_X)

// UINum DataTypes
#define VDTBYTE       0
#define VDTUINT       1
#define VDTSINT       2

// Master Defines
#define MAXWIDGETSPERSCR 30
#define MAXWIDGETENUMS 10

template<typename T, size_t N>
constexpr size_t ArraySize(T (&)[N]) { return N; }

static const uint16_t MultiplierDEC[] = {1, 10, 100, 1000};
static const uint16_t MultiplierHEX[] = {1, 16, 256, 4096};
//--------------------------------------------------------------------------------------------------------------------------------
class TGTWidget {
public:
  TGTWidget(byte _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, bool _Settable, uint16_t _WHandleUID)
  : SelectNum(_SelectNum), GFX(_GFX), PosX(_PosX), PosY(_PosY), Settable(_Settable), WHandleUID(_WHandleUID)  {}
  virtual ~TGTWidget() = default;
  //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
  uint8_t SelectNum;
  uint16_t WHandleUID = 0xFFFF;

  virtual int Value() { return 0; }
  virtual bool Value(int _Value) { return false; }
  virtual char* chValue() { return nullptr; }
  virtual bool chValue(char* _Value) { return false; }        // Read/write
  virtual bool chValue(const char* _Value) { return false; } // Read only
  virtual bool AddEnum(int _Value, const __FlashStringHelper* _Label) { return false; }
  bool Selected() { return bSelected; }
  void Selected(bool _Selected) { if (_Selected != bSelected) {bSelected = _Selected;Display();} }

  virtual void Display() {  }  
  virtual uint8_t WidgetNavigate(uint8_t _Button) { return _Button; }
  
protected:
  Adafruit_GFX* GFX = nullptr;
  uint8_t PosX, PosY;
  bool Settable = false;
  bool bSelected = false;
  byte EditPos=0;
  bool EditMode = false;
};
//--------------------------------------------------------------------------------------------------------------------------------
class WFLabel : public TGTWidget {
public:
  WFLabel(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, const __FlashStringHelper* _Label, uint16_t _WHandleUID = WHNONE )
  : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, false, _WHandleUID), Label(_Label) {  }

  void Display() override {
    GFX->setCursor(PosX, PosY);
    if (bSelected) {GFX->setTextColor(UI_SELCHAR_COLOR);} else {GFX->setTextColor(UI_STDCHAR_COLOR);}
    if (Label!=nullptr) GFX->print(Label);
  }
  // No Set Mode so Button just returns from TGTWidget
private:
  const __FlashStringHelper* Label;

};
//--------------------------------------------------------------------------------------------------------------------------------
class WChLabel : public TGTWidget {
public:
  WChLabel(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY,
           int _charSize, const char* _Label, uint16_t _WHandleUID = WHNONE)
  : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, false, _WHandleUID), charSize(_charSize) 
  {
    Label = new char[charSize + 1];
    if(Label != nullptr && _Label != nullptr) { strncpy(Label, _Label, charSize); Label[charSize] = '\0'; }
  }
  ~WChLabel() override { delete[] Label; }

  void Display() override {
    GFX->setCursor(PosX, PosY);
    if(bSelected) { GFX->setTextColor(UI_SELCHAR_COLOR); }
    else { GFX->setTextColor(UI_STDCHAR_COLOR); }
    if(Label != nullptr) GFX->print(Label);
  }
  // No Set Mode so Button just returns from TGTWidget
private:
  char* Label = nullptr;
  int charSize;
};
//--------------------------------------------------------------------------------------------------------------------------------
class WCharVal : public TGTWidget {
  public:
    WCharVal(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, int _charSize, bool _Settable = false, uint16_t _WHandleUID = WHNONE)
    : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, _Settable, _WHandleUID), charSize(_charSize) {}
  //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
  bool chValue(char* _Value) override { Valchar = _Value; Display(); return true; }
  bool chValue(const char* _Value) override { Valchar = _Value; Display(); return true; }
  char* chValue() override { return Valchar; }
  //'''''''''''''''''''''''( Display )'''''''''''''''''''''''''''''''''''''''
  void Display() override {
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
  //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
  uint8_t WidgetNavigate(uint8_t _Button) override {
    if(_Button==NAVKEYSET) {
      if(bSelected && Settable) {  // This Toggles Edit Mode
        if(!EditMode) { EditMode = true; Display(); return NAVKEYNONE; }
        EditMode = false; Display(); return NAVSTAGEVAL;
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
  protected:
    char* Valchar = nullptr;
    int charSize;
};
//--------------------------------------------------------------------------------------------------------------------------------
class WNumVal : public TGTWidget {
  public:
    uint16_t Vmin =0, Vmax =0;
    WNumVal(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY,
      uint8_t _VDataType, uint8_t _VBase = HEX, bool _Settable = false, uint16_t _WHandleUID = WHNONE)
      : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, _Settable, _WHandleUID), VDataType(_VDataType), VBase(_VBase)
    {
      if(VDataType==VDTBYTE) { 
        Vmin=0x0000; Vmax=0x00FF; 
        if(VBase==HEX) maxDigits = 2;
        if(VBase==DEC) maxDigits = 3;
      } else if(VDataType==VDTUINT) {
        Vmin=0x0000; Vmax=0xFFFF;
        if(VBase==HEX) maxDigits = 4;
        if(VBase==DEC) maxDigits = 5;
      } else if(VDataType==VDTSINT) {
        Vmin=(uint16_t)-32768; Vmax=32767;
        if(VBase==HEX) maxDigits = 4;
        if(VBase==DEC) maxDigits = 6;
      }
    }
    //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
    int Value() override { return (int16_t)uiValue; }
    bool Value(int _Value) override {
      uiValue = (uint16_t)_Value;
      if(VDataType==VDTBYTE) uiValue &= 0x00FF;
      Display();
      return true;
    }
    //'''''''''''''''''''''''( Display )'''''''''''''''''''''''''''''''''''''''
    void Display() override {
      GFX->fillRect(PosX, PosY-FONT_Y+4, FONT_X*maxDigits, FONT_Y-2, (EditMode) ? UIKB_NAMEEDIT_BGCOLOR : UI_FILLSCR_COLOR);
      GFX->setCursor(PosX, PosY);

      if(EditMode) {
        GFX->fillRect(PosX+((maxDigits-1-EditPos)*FONT_X), (PosY-FONT_Y)+4, FONT_X, FONT_Y-2, ST77XX_YELLOW);
        GFX->setTextColor(ST77XX_BLACK); }
      else if(bSelected) { GFX->setTextColor(UI_SELCHAR_COLOR); }
      else { GFX->setTextColor(UI_STDCHAR_COLOR); }

      if(VDataType==VDTSINT && VBase==DEC) {
        GFX->print((int16_t)uiValue, DEC);
      } else {
        if(VDataType==VDTBYTE) {
          if((uiValue & 0xF0)==0) GFX->print('0');
        } else {
          if((uiValue & 0xF000)==0) GFX->print('0');
          if((uiValue & 0xFF00)==0) GFX->print('0');
          if((uiValue & 0xFFF0)==0) GFX->print('0');
        }
        GFX->print(uiValue, VBase);
      }
    }
    //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
    uint8_t WidgetNavigate(uint8_t _Button) override {
      if(_Button==NAVKEYSET) {
        if(bSelected && Settable) {  // This Toggles Edit Mode
          if(!EditMode) { EditMode = true; Display(); return NAVKEYNONE; }
          EditMode = false; Display(); return NAVSTAGEVAL;
        }
      }
      if(EditMode) {
        if(_Button==NAVKEYLEFT) { if(EditPos<(maxDigits-1)) { EditPos++; Display(); return NAVKEYNONE; } }
        if(_Button==NAVKEYRIGHT) { if(EditPos>0) { EditPos--; Display(); return NAVKEYNONE; } }
        if(_Button==NAVKEYUP) {
          uint16_t Step = (VBase==HEX) ? MultiplierHEX[EditPos] : MultiplierDEC[EditPos];
          if(((uiValue/Step)%VBase)==(VBase-1)) { uiValue -= (uint16_t)((VBase-1)*Step); }
          else { uiValue += Step; }
          if(VDataType==VDTBYTE) uiValue &= 0x00FF;
          Display(); return NAVKEYNONE;
        }
        if(_Button==NAVKEYDOWN) {
          uint16_t Step = (VBase==HEX) ? MultiplierHEX[EditPos] : MultiplierDEC[EditPos];
          if(((uiValue/Step)%VBase)==0) { uiValue += (uint16_t)((VBase-1)*Step); }
          else { uiValue -= Step; }
          if(VDataType==VDTBYTE) uiValue &= 0x00FF;
          Display(); return NAVKEYNONE;
        }
      }
      return _Button;
    }
  private:
    uint16_t uiValue = 0;
    byte VBase;
    byte VDataType;
    byte maxDigits = 0;
};
//--------------------------------------------------------------------------------------------------------------------------------
class WEnumVal : public TGTWidget {
public:
  WEnumVal(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, bool _Settable = false, uint16_t _WHandleUID = WHNONE)
  : TGTWidget(_SelectNum, _GFX, _PosX, _PosY, _Settable, _WHandleUID) {}

  //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
  bool AddEnum(int _iVal, const __FlashStringHelper* _Label ) override {
    if(EnumIdx>=MAXWIDGETENUMS) { return false; } // Array Guard
    if(_Label==nullptr) return false;
    byte Len = strlen_P((PGM_P)_Label);
    if(Len > MaxChars) MaxChars = Len;
    EnumVal[EnumIdx] = _iVal;
    EnumLbls[EnumIdx] = _Label;
    EnumIdx++;
    return true;
  }
  int Value() override { return iEVal; }
  bool Value(int _Value) override { 
    iEVal = _Value;
    GFX->setCursor(PosX, PosY);
    Display();
    return true; 
  }
  //'''''''''''''''''''''''( Display )'''''''''''''''''''''''''''''''''''''''  
  void Display() override {
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
        else { GFX->print("nullptr");}
        break;
      }
    }
    if(!bEValFound) { GFX->print("noenum"); }
  }
  //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
  uint8_t WidgetNavigate(uint8_t _Button) override {
    if(_Button==NAVKEYSET) {
      DBUIINFO(("WEnumVal::NAVKEYSET"))
      if(bSelected && Settable && EnumIdx>0) {  // This Toggles Edit Mode
        if(!EditMode) { EditMode = true; Display(); return NAVKEYNONE; }
        EditMode = false; Display(); return NAVSTAGEVAL;
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

private:
  const __FlashStringHelper* EnumLbls[MAXWIDGETENUMS];
  int EnumVal[MAXWIDGETENUMS];
  byte EnumIdx = 0;
  int iEVal = 0;
  byte MaxChars = 0;
  bool bEValFound = false;
};
//-----------------------------------------------------------------------------------------------------------------------------------------
class TGTScreen {
public:
    TGTScreen(Adafruit_GFX& _GFX, uint16_t _NScrIdx) : pGFX(&_GFX), NScrIdx(_NScrIdx) {}
    ~TGTScreen() { for (uint8_t i = 0; i < WIdx; i++) { delete Widget[i]; Widget[i] = nullptr; } }
    //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
    uint16_t NScrIdx = NSNONE;
    uint16_t SetValueWH = WHNONE;
    int SetValue = 0;
    char* chSetValue = nullptr;
    
    void ClearScreen() {
      pGFX->fillScreen(UI_FILLSCR_COLOR);
      pGFX->drawFastHLine(0, FONT_Y+2, TFT_WIDTH, UI_STDLINE_COLOR);
      ScreenCleared = true;
    }
    uint16_t SelectedWHandleUID() { 
      if(pSelected!=nullptr) return pSelected->WHandleUID; return WHNONE; 
    }
    bool gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, int _Value) {
      if (_NScrIdx!=NScrIdx) return false;
      for (int i=0; i<WIdx; i++) {
        if(Widget[i]->WHandleUID==_WHandleUID) { Widget[i]->Value(_Value); return true; }
      } return false;
    }
    bool gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, char* _Value) {
      if (_NScrIdx!=NScrIdx) return false;
      for (int i=0; i<WIdx; i++) {
        if(Widget[i]->WHandleUID==_WHandleUID) { Widget[i]->chValue(_Value); return true; }
      } return false; 
    }
    bool gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, const char* _Value) {
      if (_NScrIdx!=NScrIdx) return false;
      for (int i=0; i<WIdx; i++) {
        if(Widget[i]->WHandleUID==_WHandleUID) { Widget[i]->chValue(_Value); return true; }
      } return false;      
    }
    //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
    uint8_t OnScreenNavigate(uint8_t _Button) {
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
      if(NavRtn==NAVSTAGEVAL) {
        DBUIINFO(("TGTScreen::OnScreenNavigate NavRtn==NAVSTAGEVAL Stage Set Value\n"))
        SetValueWH = pSelected->WHandleUID;
        SetValue = pSelected->Value();
        chSetValue = pSelected->chValue();
        return NAVSETVAL;
      }
      return NavRtn;
    }
    //'''''''''''''''''''''''''''''''''( Add Widgets )''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    bool NewWFLabel(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                    const __FlashStringHelper* _Label,
                    uint16_t _WHandleUID = WHNONE) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WFLabel(_SelectNum,pGFX,_PosX,_PosY,_Label,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }
    bool NewWChLabel(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                     int _charSize, char* _Label, 
                     uint16_t _WHandleUID = WHNONE) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WChLabel(_SelectNum,pGFX,_PosX,_PosY,_charSize,_Label,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }
    bool NewWNumVal(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                    uint8_t _VDataType, uint8_t _VBase = HEX, bool _Settable = false,
                    uint16_t _WHandleUID = WHNONE) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WNumVal(_SelectNum,pGFX,_PosX,_PosY,_VDataType,_VBase,_Settable,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }
    bool NewWCharVal(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                     int _charSize, bool _Settable = false,
                     uint16_t _WHandleUID = WHNONE) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WCharVal(_SelectNum, pGFX,_PosX,_PosY,_charSize,_Settable,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }    
    bool NewWEnumVal(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                     bool _Settable = false, 
                     uint16_t _WHandleUID = WHNONE) {

      if(!ScreenCleared) ClearScreen();
      if(WIdx>=MAXWIDGETSPERSCR) return false;
      Widget[WIdx] = new WEnumVal(_SelectNum,pGFX,_PosX,_PosY,_Settable,_WHandleUID);
      if(!pSelected&&_SelectNum) {pSelected = Widget[WIdx]; pSelected->Selected(true);}
      if(_SelectNum>LastSelectNum) LastSelectNum=_SelectNum;
      Widget[WIdx]->Display(); WIdx++; return true;
    }
    bool AddEnum(uint16_t _WHandleUID, int _iVal, const __FlashStringHelper* _Label) {
      for(byte i = 0; i < WIdx; i++) {
        if(Widget[i]->WHandleUID == _WHandleUID) { return Widget[i]->AddEnum(_iVal,_Label); }
      }
      return false;
    }
protected:
    Adafruit_GFX* pGFX = nullptr;
    int LastSelectNum = 0;
    bool ScreenCleared = false;
    TGTWidget* pSelected = nullptr; 
    TGTWidget* Widget[MAXWIDGETSPERSCR] = {nullptr};
    byte WIdx = 0;
};