//--------------------------------
// Lab 5 - Task 1 Template
//--------------------------------
//
//

#include "init.h"
#define ARRAY_SIZE 100
#define DMA_TRANSFER_SIZE 1

#define DAC_MAX_VALUE (1 << 12)

DMA_HandleTypeDef dma_handle_adc;
DMA_HandleTypeDef dma_handle_dac;

void configureADC();
void configureDAC();
void Init_Timer();
int8_t modulo_i8(int8_t operand, int8_t modulo);

ADC_HandleTypeDef hadc;
ADC_ChannelConfTypeDef adc_channel6_cfg;

DAC_HandleTypeDef hdac;
DAC_ChannelConfTypeDef channel1_cfg;

TIM_Base_InitTypeDef tim6_handle;

uint16_t rawReadings[ARRAY_SIZE] = {0};
uint16_t writeOuts[ARRAY_SIZE] = {0};

uint8_t first_half_flag = 1;

int main(void) {
	Sys_Init();
	Init_Timer();
	configureADC();
	configureDAC();

	// Lab 4

	HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, prev_output);

	HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, (uint32_t*)writeOuts, ARRAY_SIZE, DAC_ALIGN_12B_R);

	HAL_ADC_Start_DMA(&hadc, (uint32_t*)rawReadings, ARRAY_SIZE);

	// Code goes here
	while (1) {
		// Will block until the ADC is ready for conversion
		HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);

		adc_input = (uint16_t) HAL_ADC_GetValue(&hadc);

		samples[ring_buffer_idx] = ((float) adc_input) / 4095.0f;

		// Since modulo in C is truncated (a.k.a. can have negative values), using the compliment positive is easier (e.g. -1 -> +2, -2 -> +1).
		prev_output = 0.3125f*(samples[ring_buffer_idx]+samples[(ring_buffer_idx+1)%3]) + 0.240385f*samples[(ring_buffer_idx+2)%3] + 0.296875f*prev_output;

		dac_out = (uint16_t) ((prev_output) * 4096.0f);

		HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, dac_out );

		ring_buffer_idx += 1;
		ring_buffer_idx %= 3;
	}
}

int8_t modulo_i8(int8_t operand, int8_t modulo) {
	return ((operand % modulo) + modulo) % modulo;
}

void Init_Timer() {
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	__HAL_RCC_TIM6_CLK_ENABLE();
	// HAL Enabled Timer
	tim6_handle.Instance = TIM6;
	tim6_handle.Init.Prescaler = 23;
	tim6_handle.Init.Period = 44; // 0.01 ms period (100 kHz)
	tim6_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_Base_Init(&tim6_handle);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&tim6_handle, &sClockSourceConfig);

	sMasterConfig.MasterOutputTrigger=TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode=TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&tim6_handle, &sMasterConfig);
}

void configureADC() {
//	// Enable the ADC Clock.

	__HAL_RCC_ADC1_CLK_ENABLE();

	hadc.Instance = ADC1;
	hadc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.ContinuousConvMode = ENABLE;
	hadc.Init.DMAContinuousRequests = ENABLE;
	hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T6_TRGO;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;

	HAL_ADC_Init(&hadc);

	adc_channel6_cfg.Channel = ADC_CHANNEL_6;
	adc_channel6_cfg.Rank = ADC_REGULAR_RANK_1;
	adc_channel6_cfg.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	adc_channel6_cfg.Offset = 0;

	HAL_ADC_ConfigChannel(&hadc, &adc_channel6_cfg);

	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}

void configureDAC() {
	__HAL_RCC_DAC_CLK_ENABLE();

	hdac.Instance = DAC1;

	HAL_DAC_Init(&hdac);

	channel1_cfg = (DAC_ChannelConfTypeDef ) { .DAC_Trigger = DAC_TRIGGER_T6_TRGO,
					.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE };

	HAL_DAC_ConfigChannel(&hdac, &channel1_cfg, DAC1_CHANNEL_1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac) {
	if (hdac->Instance == DAC1) {
		// PA4 for the DAC Output
		GPIO_InitTypeDef init = (GPIO_InitTypeDef ) { .Mode = GPIO_MODE_ANALOG,
						.Pin = GPIO_PIN_4, .Pull = GPIO_NOPULL };
		HAL_GPIO_Init(GPIOA, &init);

		// Associated DMA Initialization
		__HAL_RCC_DMA1_CLK_ENABLE();

		dma_handle_dac.Instance = DMA1_Stream5;
		dma_handle_dac.Init = (DMA_InitTypeDef ) { .Channel = DMA_CHANNEL_7,
						.Direction = DMA_MEMORY_TO_PERIPH,
						.MemInc = DMA_MINC_ENABLE, .PeriphInc = DMA_PINC_DISABLE,
						.Mode = DMA_CIRCULAR, .Priority = DMA_PRIORITY_VERY_HIGH,
						.MemDataAlignment = DMA_MDATAALIGN_HALFWORD,
						.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
						.FIFOMode = DMA_FIFOMODE_DISABLE, .MemBurst =
						DMA_MBURST_INC16, .PeriphBurst = DMA_PBURST_SINGLE };

		HAL_DMA_Init(&dma_handle_dac);
		__HAL_LINKDMA(&hdac, DMA_Handle, dma_handle_dac);

		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	}
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc) {

	// GPIO init
	if (hadc->Instance == ADC1) {
		// PA6 for ADC Input
		GPIO_InitTypeDef init = (GPIO_InitTypeDef ) { .Mode = GPIO_MODE_ANALOG,
						.Pin = GPIO_PIN_6, .Pull = GPIO_NOPULL };
		HAL_GPIO_Init(GPIOA, &init);

		// Associated DMA Initialization
		__HAL_RCC_DMA2_CLK_ENABLE();

		dma_handle_adc.Instance = DMA2_Stream0;
		dma_handle_adc.Init = (DMA_InitTypeDef ) { .Channel = DMA_CHANNEL_0,
						.Direction = DMA_PERIPH_TO_MEMORY,
						.MemInc = DMA_MINC_ENABLE, .PeriphInc = DMA_PINC_DISABLE,
						.Mode = DMA_CIRCULAR, .Priority = DMA_PRIORITY_VERY_HIGH,
						.MemDataAlignment = DMA_MDATAALIGN_HALFWORD,
						.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
						.FIFOMode = DMA_FIFOMODE_DISABLE, .MemBurst =
						DMA_MBURST_INC16, .PeriphBurst = DMA_PBURST_SINGLE };

		HAL_DMA_Init(&dma_handle_adc);
		__HAL_LINKDMA(&hdac, DMA_Handle1, dma_handle_dac);

		HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	}
}

void ADC_IRQHandler() { HAL_ADC_IRQHandler(&hadc); }

void DMA1_Stream5_IRQHandler() { HAL_DMA_IRQHandler(&dma_handle_dac); }

void DMA2_Stream0_IRQHandler() { HAL_DMA_IRQHandler(&dma_handle_adc); }

void HAL_ADC_ConvHalfCpltCallback() {
	// Do the filtering from rawReadings to writeOuts
	if(first_half_flag) {
		memcpy(writeOuts, rawReadings, ARRAY_SIZE/2);
		first_half_flag = 0;
	} else {
		memcpy(writeOuts+ARRAY_SIZE/2, rawReadings+ARRAY_SIZE/2, ARRAY_SIZE/2);
		first_half_flag = 1;
	}
}



