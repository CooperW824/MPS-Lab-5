//--------------------------------
// Lab 5 - Task 1 Template
//--------------------------------
//
//

#include "init.h"

int main(void)
{
	Sys_Init();

	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("\033[0m\033[44;33m\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);
	uint32_t cycles = DWT->CYCCNT; // Store the cycle counter

	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("Clearing and Moving the Cursor to Home took %lu CPU cycles\r\n",cycles);
	fflush(stdout);
	cycles = DWT->CYCCNT; // Store the cycle counter

	printf("And printing the above took %lu CPU cycles\r\n",cycles);
	fflush(stdout);

	while(1);
}
