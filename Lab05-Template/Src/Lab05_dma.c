//--------------------------------
// Lab 5 - Task 1 Template
//--------------------------------
//
//

#include "init.h"
#define ARRAY_SIZE 256
#define TRANSFER_SIZE 4

void Init_DMA();
void Transfer_Complete(DMA_HandleTypeDef *handle);

DMA_HandleTypeDef dma_handle;
volatile uint8_t flag;
char fromArr[ARRAY_SIZE], toArr[ARRAY_SIZE];

int main(void) {
	Sys_Init();
	Init_DMA();

	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("\033[0m\033[44;33m\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);
	uint32_t cycles = DWT->CYCCNT; // Store the cycle counter

	DWT->CYCCNT = 0; // Clear the cycle counter
	printf("Clearing and Moving the Cursor to Home took %lu CPU cycles\r\n",
			cycles);
	fflush(stdout);
	cycles = DWT->CYCCNT; // Store the cycle counter

	printf("And printing the above took %lu CPU cycles\r\n", cycles);
	fflush(stdout);

	strcpy(fromArr, "Cooper & Andrew are doing this lab right now!");
	strcpy(toArr, "What are they doing right now?");
	printf("FromArr: %s\r\nToArr: %s\r\n", fromArr, toArr);
	fflush(stdout);
	HAL_DMA_Start_IT(&dma_handle, (uint32_t) fromArr, (uint32_t) toArr,
	ARRAY_SIZE / TRANSFER_SIZE);
	flag = 0;
	DWT->CYCCNT = 0;

	while (!flag)
		;
	cycles = DWT->CYCCNT;
	printf("Cycles: %ld\r\nFromArr: %s\r\nToArr: %s\r\n", cycles, fromArr,
			toArr);
	fflush(stdout);

	while (1)
		;
}

void Init_DMA() {

	__HAL_RCC_DMA2_CLK_ENABLE();

	dma_handle.Instance = DMA2_Stream0;
	dma_handle.Init = (DMA_InitTypeDef ) { .Channel = DMA_CHANNEL_0,
					.Direction = DMA_MEMORY_TO_MEMORY,
					.MemInc = DMA_MINC_ENABLE, .PeriphInc = DMA_PINC_ENABLE,
					.Mode = DMA_NORMAL, .Priority = DMA_PRIORITY_VERY_HIGH,
					.FIFOMode = DMA_FIFOMODE_DISABLE, .MemBurst =
					DMA_MBURST_INC16, .PeriphBurst = DMA_PBURST_INC16 };

	switch (TRANSFER_SIZE) {
	case 1:
		dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		break;
	case 2:
		dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		break;
	case 4:
		dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		break;
	default:
		printf("You darn screwed up! Transfer size can be 1, 2, or 4. \r\n");
		while(1);
		break;
	}

	dma_handle.XferCpltCallback = Transfer_Complete;
	HAL_DMA_Init(&dma_handle);
//	__HAL_LINKDMA();

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

void DMA2_Stream0_IRQHandler() {
	HAL_DMA_IRQHandler(&dma_handle);
}

void Transfer_Complete(DMA_HandleTypeDef *handle) {
	flag = 1;
}
