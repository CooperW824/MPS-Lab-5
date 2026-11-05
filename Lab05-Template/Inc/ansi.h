#ifndef ANSI_H
#define ANSI_H

#include<stdint.h>

/**
 * AI Attribution: The below definitions were translated to C code using Claude which was provided with a list of the ANSI terminal sequences.
 * Chat Link: https://claude.ai/share/b79cd240-820b-411c-86fd-cc4fe582b86d
 */

#define ANSI_TEXT_RESET       0
#define ANSI_TEXT_BRIGHT      1
#define ANSI_TEXT_DIM         2
#define ANSI_TEXT_ITALICS     3
#define ANSI_TEXT_UNDERSCORE  4
#define ANSI_TEXT_BLINK       5
#define ANSI_TEXT_REVERSE     7
#define ANSI_TEXT_HIDDEN      8

#define ANSI_FG_BLACK    30
#define ANSI_FG_RED      31
#define ANSI_FG_GREEN    32
#define ANSI_FG_YELLOW   33
#define ANSI_FG_BLUE     34
#define ANSI_FG_MAGENTA  35
#define ANSI_FG_CYAN     36
#define ANSI_FG_WHITE    37

#define ANSI_BG_BLACK    40
#define ANSI_BG_RED      41
#define ANSI_BG_GREEN    42
#define ANSI_BG_YELLOW   43
#define ANSI_BG_BLUE     44
#define ANSI_BG_MAGENTA  45
#define ANSI_BG_CYAN     46
#define ANSI_BG_WHITE    47


// The Function definitions were written by human hands

void ANSI_ResetTerminal();
void ANSI_SetLineWrap(uint8_t enabled);

void ANSI_CursorHome();
void ANSI_SetCursorPosition(uint8_t row, uint8_t col);
void ANSI_CursorSavePosition();
void ANSI_CursorRestoreToSavedPosition();

void ANSI_EraseEndOfLine();
void ANSI_EraseStartOfLine();
void ANSI_EraseWholeLine();
void ANSI_EraseDown();
void ANSI_EraseUp();
void ANSI_EraseScreen();

void ANSI_SetScrollAll();
void ANSI_SetScrollSection(uint8_t start, uint8_t end);
void ANSI_ScrollDown();
void ANSI_ScrollUp();

void ANSI_SetAttributes(uint8_t text, uint8_t foreground, uint8_t background);

void ANSI_Beep();

#endif /* ANSI_H */
