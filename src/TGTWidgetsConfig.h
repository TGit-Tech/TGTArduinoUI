#pragma once

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
* @defgroup NAVKEY Navigate() Keypad Constants
* @{
*********************************************************************************/
// 0x[F]-Key Presses
#define NAVKEYNONE        0xFF
#define NAVKEYRIGHT       0xFE
#define NAVKEYLEFT        0xFD
#define NAVKEYUP          0xFC
#define NAVKEYDOWN        0xFB
#define NAVKEYSET         0xFA
#define NAVKEYGET        0xF9
#define KEYPRESSMIN       0xF0
///@}
/******************************************************************************//**
* @defgroup NAV Navigate() Event Constants
* @{
*********************************************************************************/
// 0x[E]-NAV Event
#define NAVSETVAL     0xEE  // Reserved. Used in UIBase.h
///@}