//----------------------------------
// Lab 3 - Part 3: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"
#include "ansi.h"

// If needed:
#include <stdio.h>
//#include <stdlib.h>

#define BUFFER_SIZE_BYTES 512

SPI_HandleTypeDef hspi;
DMA_HandleTypeDef txDMA;
DMA_HandleTypeDef rxDMA;

uint8_t tx_buffer[BUFFER_SIZE_BYTES];
uint8_t rx_buffer[BUFFER_SIZE_BYTES];
uint16_t buffer_length = 0;
volatile uint8_t tx_cplt = 0;
uint8_t rx_row = 14;

void configureSPI();
void configureDMA();

int main(void) {
	Sys_Init();

	// For convenience
	configureDMA();
	configureSPI();

	setbuf(stdout, NULL);

	ANSI_EraseScreen();
	ANSI_CursorHome();

	printf("---Transmitted Characters----------------------------------\r\n");
	ANSI_CursorSavePosition();
	ANSI_SetCursorPosition(13, 0);
	printf("---Received Characters-------------------------------------\r\n");

	while (1) {
		HAL_UART_Receive(&USB_UART, &tx_buffer[buffer_length], 1,
		HAL_MAX_DELAY);
		buffer_length++;

		if (tx_buffer[buffer_length - 1] != '\r') {
			ANSI_CursorRestoreToSavedPosition();
			printf("%c", tx_buffer[buffer_length - 1]);
			ANSI_CursorSavePosition();
			continue;
		}

		if (buffer_length == 1) {
			buffer_length = 0;
			continue;
		}

		// Here we know that we got a newline meaning we need to handle
		// transmission and receive

		HAL_SPI_TransmitReceive_DMA(&hspi, tx_buffer, rx_buffer, buffer_length-1);

		while (!tx_cplt)
			;

		ANSI_CursorRestoreToSavedPosition();
		printf("\r\n");
		ANSI_CursorSavePosition();

		tx_buffer[buffer_length-1] = '\0';
		rx_buffer[buffer_length-1] = '\0';

		ANSI_SetCursorPosition(rx_row, 1);
		printf("%s\n", rx_buffer);
		rx_row++;
		buffer_length = 0;
		tx_cplt = 0;
	}

// See 76x Description of HAL drivers.pdf, Ch. 64.2 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
}

/*
 * For convenience, configure the SPI handler here
 */
// See 76x Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
void configureSPI() {

	hspi.Instance = SPI1;
	hspi.Init = (SPI_InitTypeDef ) { .Mode = SPI_MODE_MASTER, .Direction =
			SPI_DIRECTION_2LINES, .DataSize = SPI_DATASIZE_8BIT, .CLKPolarity =
			SPI_POLARITY_LOW, .CLKPhase =
			SPI_PHASE_1EDGE, .NSS = SPI_NSS_HARD_OUTPUT, .TIMode =
			SPI_TIMODE_DISABLED, .CRCCalculation =
			SPI_CRCCALCULATION_DISABLE, .BaudRatePrescaler =
			SPI_BAUDRATEPRESCALER_16 };

	HAL_SPI_Init(&hspi);

	HAL_NVIC_EnableIRQ(SPI1_IRQn);
	HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);

	// Note: HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
	//
	// HAL_SPI_Init() will call HAL_SPI_MspInit() after reading the properties of
	// the passed SPI_HandleTypeDef. After finishing the MspInit call, it will set
	// the SPI property bits. This is how all HAL_[peripheral]_Init() functions work.
}

void configureDMA() {

	__HAL_RCC_DMA2_CLK_ENABLE();

	txDMA.Instance = DMA2_Stream3;
	txDMA.Init = (DMA_InitTypeDef ) { .Channel = DMA_CHANNEL_3, .Direction =
			DMA_MEMORY_TO_PERIPH, .MemInc = DMA_MINC_ENABLE, .PeriphInc =
			DMA_PINC_DISABLE, .Mode = DMA_NORMAL, .Priority =
			DMA_PRIORITY_VERY_HIGH, .FIFOMode =
			DMA_FIFOMODE_DISABLE, .MemBurst =
			DMA_MBURST_INC4, .PeriphBurst = DMA_PBURST_INC4, .MemDataAlignment =
			DMA_MDATAALIGN_HALFWORD, .PeriphDataAlignment =
			DMA_PDATAALIGN_HALFWORD };

	HAL_DMA_Init(&txDMA);
	__HAL_LINKDMA(&hspi, hdmatx, txDMA);

	rxDMA.Instance = DMA2_Stream2;
	rxDMA.Init = (DMA_InitTypeDef ) { .Channel = DMA_CHANNEL_3, .Direction =
			DMA_PERIPH_TO_MEMORY, .MemInc = DMA_MINC_ENABLE, .PeriphInc =
			DMA_PINC_DISABLE, .Mode = DMA_NORMAL, .Priority =
			DMA_PRIORITY_VERY_HIGH, .FIFOMode =
			DMA_FIFOMODE_DISABLE, .MemBurst =
			DMA_MBURST_INC4, .PeriphBurst = DMA_PBURST_INC4, .MemDataAlignment =
			DMA_MDATAALIGN_HALFWORD, .PeriphDataAlignment =
			DMA_PDATAALIGN_HALFWORD };

	HAL_DMA_Init(&rxDMA);
	__HAL_LINKDMA(&hspi, hdmarx, rxDMA);

	HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
}

void SPI1_IRQHandler() {
	HAL_SPI_IRQHandler(&hspi);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	tx_cplt = 1;
}

void DMA2_Stream2_IRQHandler(void) { HAL_DMA_IRQHandler(&rxDMA); }
void DMA2_Stream3_IRQHandler(void) { HAL_DMA_IRQHandler(&txDMA); }

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
// Do NOT change the name of an MspInit function; it needs to override a
// __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
	// SPI GPIO initialization structure here

	if (hspi->Instance == SPI1) {
		// SPI1_SCK: PA5
		GPIO_InitTypeDef gpio_init = (GPIO_InitTypeDef ) { .Mode =
				GPIO_MODE_AF_PP, .Pin = GPIO_PIN_5, .Speed =
				GPIO_SPEED_HIGH, .Alternate = GPIO_AF5_SPI1, .Pull =
				GPIO_PULLUP };

		HAL_GPIO_Init(GPIOA, &gpio_init);

		// SPI1_CIPO: PA6
		gpio_init.Pin = GPIO_PIN_6;

		HAL_GPIO_Init(GPIOA, &gpio_init);

		//SPI1_COPI: PA7
		gpio_init.Pin = GPIO_PIN_7;

		HAL_GPIO_Init(GPIOA, &gpio_init);

		// SPI1_CS: PD14
		gpio_init.Pin = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOD, &gpio_init);

		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
	}
}
