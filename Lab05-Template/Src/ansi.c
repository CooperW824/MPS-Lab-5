
#include "ansi.h"
#include <stdio.h>

/**
 * AI Attribution: The below functions were filled in using Claude which was provided with a list of the ANSI terminal sequences.
 * Chat Link: https://claude.ai/share/b79cd240-820b-411c-86fd-cc4fe582b86d
 */

void ANSI_ResetTerminal()
{
	char cmd[] = {0x1B, 0x63, 0x00};
	printf("%s", cmd);
}

void ANSI_SetLineWrap(uint8_t enabled)
{
	char cmd[] = {0x1B, 0x5B, 0x37, enabled ? 0x68 : 0x6C, 0x00};
	printf("%s", cmd);
}

void ANSI_CursorHome()
{
	char cmd[] = {0x1B, 0x5B, 0x48, 0x00};
	printf("%s", cmd);
}

void ANSI_SetCursorPosition(uint8_t row, uint8_t col)
{
	// \033[{ROW};{COL}H  -- variable-length digits, so build with snprintf
	printf("\033[%u;%uH", row, col);
}

void ANSI_CursorSavePosition()
{
	char cmd[] = {0x1B, 0x5B, 0x73, 0x00};
	printf("%s", cmd);
}

void ANSI_CursorRestoreToSavedPosition()
{
	char cmd[] = {0x1B, 0x5B, 0x75, 0x00};
	printf("%s", cmd);
}

void ANSI_EraseEndOfLine()
{
	char cmd[] = {0x1B, 0x5B, 0x4B, 0x00};
	printf("%s", cmd);
}

void ANSI_EraseStartOfLine()
{
	char cmd[] = {0x1B, 0x5B, 0x31, 0x4B, 0x00};
	printf("%s", cmd);
}

void ANSI_EraseWholeLine()
{
	char cmd[] = {0x1B, 0x5B, 0x32, 0x4B, 0x00};
	printf("%s", cmd);
}

void ANSI_EraseDown()
{
	char cmd[] = {0x1B, 0x5B, 0x4A, 0x00};
	printf("%s", cmd);
}

void ANSI_EraseUp()
{
	char cmd[] = {0x1B, 0x5B, 0x31, 0x4A, 0x00};
	printf("%s", cmd);
}

void ANSI_EraseScreen()
{
	char cmd[] = {0x1B, 0x5B, 0x32, 0x4A, 0x00};
	printf("%s", cmd);
}

void ANSI_SetScrollAll()
{
	char cmd[] = {0x1B, 0x5B, 0x72, 0x00};
	printf("%s", cmd);
}

void ANSI_SetScrollSection(uint8_t start, uint8_t end)
{
	// \033[{SRT};{END}r
	printf("\033[%u;%ur", start, end);
}

void ANSI_ScrollDown()
{
	char cmd[] = {0x1B, 0x44, 0x00};
	printf("%s", cmd);
}

void ANSI_ScrollUp()
{
	char cmd[] = {0x1B, 0x4D, 0x00};
	printf("%s", cmd);
}

void ANSI_SetAttributes(uint8_t text, uint8_t foreground, uint8_t background)
{
	// \033[{ATR1};{ATR2};{ATR3}m
	printf("\033[%u;%u;%um", text, foreground, background);
}

void ANSI_Beep(){
	printf("\a");
}
