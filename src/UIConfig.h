#pragma once

// 1.8in ST7735S TFT is 128x160
#define TFT_WIDTH   128
#define TFT_HEIGHT  160

#define FONT_X      8           // Chosen Font xAdvance per character
#define FONT_Y      16          // Chosen Font yAdvance per line
#define TFT_VAL_X   100


// UI Navigation Screens & Navigation Commands
// Nav Screens Dynamic
#define NODEIDXMASK 0x00FF
#define NSTHISNODE  0x0000
#define NSNODEMAX   0x09FF   // Sub-Screen 9 of NodeIdx 255(FF).

// Nav Screens Identity
#define NSNONE        0xFFFF
#define NSNODE       0x0A01
#define NSSETUP      0x0A03
#define NSEDITNODE   0x0A04
#define NSSYSTEM     0x0A05

// Multi-Use Widget Handles
#define WHNONE          0xFFFF
#define WHTITLE         0x0A00
#define WHTITLENN       0x0A01    // Screen Title Node Name

// Node Edit Widget Handles
#define WHNODENAME      0x0A02
#define WHSECNET        0x0A03
#define WHRADIOID       0x0A04
#define WHNETID         0x0A05
#define WHRFADDR        0x0A06

// Setup Screen Widget Handles
#define WHADDRNODE    0x0A07
#define WHDELRNODE    0x0A08
#define WHEDITNODE    0x0A09
#define WHADDDEV      0x0A0A
#define WHDELDEV      0x0A0B
#define WHEDITDEV     0x0A0C
#define WHSYSTEM      0x0A0D

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
#define NAVSTAGEVAL   0xEF
#define NAVSETVAL     0xEE
#define NAVDISPLAY    0xED
#define NAVGETVALS    0xEC
#define NAVGETSETVAL  0xEB
///@}