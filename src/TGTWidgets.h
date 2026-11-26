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

#include <Adafruit_ST7735.h>
#include <TGT6KeypadDefs.h>

// TGTWidgets configuration (formerly TGTWidgetsConfig.h)
// 1.8in ST7735S TFT is 128x160
#define TFT_WIDTH   128
#define TFT_HEIGHT  160

#define FONT_X      8           // Chosen Font xAdvance per character
#define FONT_Y      16          // Chosen Font yAdvance per line
#define TFT_VAL_X   100

// Nav Screens Identity
#define NSNONE        0xFFFF    // Reserved. Used in UIBase.h

// Multi-Use Widget Handles
#define WHNONE          0xFFFF  // Reserved. Used in UIBase.h

// Color Scheme
#define UI_FILLSCR_COLOR        ST77XX_BLACK
#define UI_SELCHAR_COLOR        ST77XX_BLUE
#define UI_STDCHAR_COLOR        ST77XX_WHITE
#define UI_STDLINE_COLOR        0x03E0

#define UIKB_NAMEEDIT_BGCOLOR   0x8410
#define UIKB_SELNAMECHAR_COLOR  0x001F
#define UIKB_STDNAMECHAR_COLOR  ST77XX_WHITE
#define UIKB_SELCHAR_COLOR      ST77XX_BLUE
#define UIKB_STDCHAR_COLOR      ST77XX_WHITE
#define GFXYELLOW 0xFFDE
 /*
 // Some ready-made 16-bit ('565') color settings:
 #define ST77XX_BLACK 0x0000
 #define ST77XX_WHITE 0xFFFF
 #define ST77XX_RED 0xF800
 #define ST77XX_GREEN 0x07E0
 #define ST77XX_BLUE 0x001F
 #define ST77XX_CYAN 0x07FF
 #define ST77XX_MAGENTA 0xF81F
 #define ST77XX_YELLOW 0xFFE0
 #define ST77XX_ORANGE 0xFC00
*/

/******************************************************************************//**
* @defgroup NAV Navigate() Event Constants
* @{
*********************************************************************************/
// 0x[E]-NAV Event
#define NAVSETVAL     0xEE  // Reserved. Used in UIBase.h
///@}
#include <avr/pgmspace.h>

#define TFT_ROWS        (TFT_HEIGHT / FONT_Y)
#define TFT_ROW_Y(row)  (((row) + 1) * FONT_Y)  // Position by bottom-left so row 0; actually bottom row 1.
#define TFT_COL_X(col)  (((col) + 0) * FONT_X)
#define CHARSPERROW     (TFT_WIDTH / FONT_X)

// WNumVal DataTypes
#define TBYTE     0b00000000  // Byte             0bxxx0 0xxx
#define TUINT     0b00001000  // Unsigned integer 0bxxx0 1xxx
#define TSINT     0b00010000  // Signed integer   0bxxx1 0xxx
#define THEX      0b00100000  // Hexadecimal      0bxx1x xxxx
#define TDEC      0b00000000  // Decimal          0bxx0x xxxx
#define TNOSET    0b00000000  // Not settable     0b00xx xxxx
#define TSET      0b01000000  // Settable         0b01xx xxxx
#define TSETPD    0b11000000  // Settable, per digit 0b11xx xxxx
#define TSETMSK   0b11000000  // Bits 7–6
#define TBASEMSK  0b00100000  // Bit 5
#define TTYPEMSK  0b00011000  // Bits 4–3

// Master Defines
#define MAXWIDGETSPERSCR 30
#define MAXWIDGETENUMS 10

template<typename T, size_t N>
constexpr size_t ArraySize(T (&)[N]) { return N; }

extern const uint16_t MultiplierDEC[4];
extern const uint16_t MultiplierHEX[4];
//--------------------------------------------------------------------------------------------------------------------------------
class TGTWidget {
public:
TGTWidget(byte _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, bool _Settable, uint16_t _WHandleUID);
  virtual ~TGTWidget() = default;
  //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
  uint8_t SelectNum;
  uint16_t WHandleUID = 0xFFFF;
  // getters
virtual int Value();
virtual bool Value(int _Value);
virtual char* chValue();
  // setters
virtual bool chValue(char* _Value);        // Read/write
virtual bool chValue(const char* _Value); // Read only
virtual bool AddEnum(int _Value, const __FlashStringHelper* _Label);
virtual bool AddchEnum(int _iVal, const char* _Label);
bool Selected();
void Selected(bool _Selected);

virtual void Display();  
virtual uint8_t WidgetNavigate(uint8_t _Button);
  
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
WFLabel(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, const __FlashStringHelper* _Label, uint16_t _WHandleUID = WHNONE );

void Display() override;
  // No Set Mode so Button just returns from TGTWidget
private:
  const __FlashStringHelper* Label;

};
//--------------------------------------------------------------------------------------------------------------------------------
class WChLabel : public TGTWidget {
public:
WChLabel(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY,
           int _charSize, const char* _Label, uint16_t _WHandleUID = WHNONE);
~WChLabel() override;

void Display() override;
  // No Set Mode so Button just returns from TGTWidget
private:
  char* Label = nullptr;
  int charSize;
};
//--------------------------------------------------------------------------------------------------------------------------------
class WCharVal : public TGTWidget {
  public:
WCharVal(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, int _charSize, bool _Settable = false, uint16_t _WHandleUID = WHNONE);
  //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
bool chValue(char* _Value) override;
bool chValue(const char* _Value) override;
char* chValue() override;
  //'''''''''''''''''''''''( Display )'''''''''''''''''''''''''''''''''''''''
void Display() override;
  //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
uint8_t WidgetNavigate(uint8_t _Button) override;
  protected:
    char* Valchar = nullptr;
    int charSize;
};
//--------------------------------------------------------------------------------------------------------------------------------
class WNumVal : public TGTWidget {
  public:
WNumVal(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY,
      uint8_t _VDataType, uint16_t _WHandleUID = WHNONE, uint16_t _Vmin=0, uint16_t _Vmax=0xFFFF);
    //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
int Value() override;
bool Value(int _Value) override;
    //'''''''''''''''''''''''( Display )'''''''''''''''''''''''''''''''''''''''
void Display() override;
    //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
uint8_t WidgetNavigate(uint8_t _Button) override;
  private:
    byte VDataType;
    uint16_t uiValue = 0, Vmin = 0x0000, Vmax = 0xFFFF;
    bool PerDigit = true;
    byte VBase;
    byte maxDigits = 0;
};
//--------------------------------------------------------------------------------------------------------------------------------
class WEnumVal : public TGTWidget {
public:
WEnumVal(uint8_t _SelectNum, Adafruit_GFX* _GFX, uint8_t _PosX, uint8_t _PosY, bool _Settable = false, uint16_t _WHandleUID = WHNONE);
WEnumVal::~WEnumVal();
  //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
bool AddEnum(int _iVal, const __FlashStringHelper* _Label ) override;
bool AddchEnum(int _iVal, const char* _Label ) override;
int Value() override;
bool Value(int _Value) override;
  //'''''''''''''''''''''''( Display )'''''''''''''''''''''''''''''''''''''''  
void Display() override;
  //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
uint8_t WidgetNavigate(uint8_t _Button) override; 

private:
  const __FlashStringHelper* EnumLbls[MAXWIDGETENUMS];
  const char* chEnumLbls[MAXWIDGETENUMS] = {0};
  int EnumVal[MAXWIDGETENUMS];
  byte EnumIdx = 0;
  int iEVal = 0;
  byte MaxChars = 2;
  bool bEValFound = false;
};
//-----------------------------------------------------------------------------------------------------------------------------------------
class TGTScreen {
public:
TGTScreen(Adafruit_GFX& _GFX, uint16_t _NScrIdx);
~TGTScreen();
    //'''''''''''''''''''''''( Get/Set )'''''''''''''''''''''''''''''''''''''''
    uint16_t NScrIdx = NSNONE;
    
void ClearScreen();
virtual void DisplayScreen() { } // Screen Specific
uint16_t SelectedWHandleUID();
char* SelectedWchValue();
uint16_t SelectedWValue();
bool gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, int _Value);
bool gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, char* _Value);
bool gotValue(uint16_t _NScrIdx, uint16_t _WHandleUID, const char* _Value);
    //'''''''''''''''''''''''( Navigate )''''''''''''''''''''''''''''''''''''''
virtual uint8_t Navigate(uint8_t _Button) { return _Button; } // Screen Specific
uint8_t OnScreenNavigate(uint8_t _Button);
    //'''''''''''''''''''''''''''''''''( Add Widgets )''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
bool NewWFLabel(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                    const __FlashStringHelper* _Label,
                    uint16_t _WHandleUID = WHNONE);
bool NewWChLabel(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                     int _charSize, char* _Label, 
                     uint16_t _WHandleUID = WHNONE); 
bool NewWNumVal(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                    uint8_t _VDataType, uint16_t _WHandleUID = WHNONE,
                    uint16_t _Vmin=0, uint16_t _Vmax=0xFFFF, 
                    const uint16_t* _Omit = nullptr, uint8_t _OmitSize = 0);
bool NewWCharVal(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                     int _charSize, bool _Settable = false,
                     uint16_t _WHandleUID = WHNONE);    
bool NewWEnumVal(uint8_t _SelectNum, uint8_t _PosX, uint8_t _PosY,
                     bool _Settable = false, 
                     uint16_t _WHandleUID = WHNONE);
bool AddEnum(uint16_t _WHandleUID, int _iVal, const __FlashStringHelper* _Label);
bool AddchEnum(uint16_t _WHandleUID, int _iVal, const char* _Label);
protected:
    Adafruit_GFX* pGFX = nullptr;
    int LastSelectNum = 0;
    bool ScreenCleared = false;
    TGTWidget* pSelected = nullptr; 
    TGTWidget* Widget[MAXWIDGETSPERSCR] = {nullptr};
    byte WIdx = 0;
};