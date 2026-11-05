//--------------------------------
// Lab 5 - Task 1 Template
//--------------------------------
//
//

#include "init.h"
#define ARRAY_SIZE 100
#define DMA_TRANSFER_SIZE 1

#define DAC_MAX_VALUE (1 << 12)
#define PROPER_MOD(operand, modulo) (((operand % modulo) + modulo) % modulo)
#define ADC_TO_FLOAT(input) (((float) input) / 4096.0)
#define FLOAT_TO_DAC(output) ((int)(((float) output) * 4096.0))

DMA_HandleTypeDef dma_handle_adc;
DMA_HandleTypeDef dma_handle_dac;

void configureADC();
void configureDAC();
void Init_Timer();
void applyFilter(uint8_t which_half);

ADC_HandleTypeDef hadc;
ADC_ChannelConfTypeDef adc_channel6_cfg;

DAC_HandleTypeDef hdac;
DAC_ChannelConfTypeDef channel1_cfg;

TIM_Base_InitTypeDef tim6_handle;

uint16_t rawReadings[ARRAY_SIZE] = { 0 };
uint16_t start_minus_2 = 0;
uint16_t start_minus_1 = 0;
float prev_output = 0;
uint16_t writeOuts[ARRAY_SIZE] = { 0 };

uint8_t past_half_flag = 0;
uint8_t half_complete_flag = 0;

int main(void) {
	Sys_Init();
	Init_Timer();
	configureADC();
	configureDAC();

	// Lab 4

	HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, prev_output);

	HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, (uint32_t*) writeOuts, ARRAY_SIZE,
	DAC_ALIGN_12B_R);

	HAL_ADC_Start_DMA(&hadc, (uint32_t*) rawReadings, ARRAY_SIZE);

	// Code goes here
	while (1) {
		while (!half_complete_flag); // Busy wait for readings to fill half of the buffer
		applyFilter(past_half_flag);
		past_half_flag = !past_half_flag; // Toggle
	}
}

void applyFilter(uint8_t which_half) {
	uint16_t i, end;
	if (which_half == 1) {
		// Upper half of the array
		i = ARRAY_SIZE / 2;
		end = ARRAY_SIZE;
		start_minus_2 = rawReadings[ARRAY_SIZE - 2];
		start_minus_2 = rawReadings[ARRAY_SIZE - 1];
	} else {
		i = 0;
		end = ARRAY_SIZE / 2;

	}

	prev_output = 0.296875f * prev_output;
	prev_output += 0.3125f * ADC_TO_FLOAT(start_minus_2);
	prev_output += 0.240385f * ADC_TO_FLOAT(start_minus_1);
	prev_output += 0.3125f * ADC_TO_FLOAT(rawReadings[i]);

	writeOuts[i] = FLOAT_TO_DAC(prev_output);

	prev_output = 0.296875f * prev_output;
	prev_output += 0.3125f * ADC_TO_FLOAT(start_minus_1);
	prev_output += 0.240385f * ADC_TO_FLOAT(rawReadings[i]);
	prev_output += 0.3125f * ADC_TO_FLOAT(rawReadings[i + 1]);

	writeOuts[i + 1] = FLOAT_TO_DAC(prev_output);

	i += 2;

	for (; i < end; i++) {
		prev_output = 0.3125f
				* (ADC_TO_FLOAT(rawReadings[i])
						+ ADC_TO_FLOAT(rawReadings[i - 2]))
				+ 0.240385f * ADC_TO_FLOAT(rawReadings[i - 1])
				+ 0.296875f * prev_output;
	}

	if (which_half == 1) {
		start_minus_2 = rawReadings[ARRAY_SIZE - 2];
		start_minus_1 = rawReadings[ARRAY_SIZE - 1];
	} else {
		start_minus_ = rawReadings[ARRAY_SIZE / 2 - 3];
		start_minus_1 = rawReadings[ARRAY_SIZE / 2 - 2];
	}
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
						.Direction = DMA_MEMORY_TO_PERIPH, .MemInc =
						DMA_MINC_ENABLE, .PeriphInc = DMA_PINC_DISABLE, .Mode =
								DMA_CIRCULAR,
						.Priority = DMA_PRIORITY_VERY_HIGH, .MemDataAlignment =
						DMA_MDATAALIGN_HALFWORD, .PeriphDataAlignment =
						DMA_PDATAALIGN_HALFWORD, .FIFOMode =
						DMA_FIFOMODE_DISABLE, .MemBurst =
						DMA_MBURST_INC16, .PeriphBurst = DMA_PBURST_SINGLE };

		HAL_DMA_Init(&dma_handle_dac);
		__HAL_LINKDMA(&hdac, DMA_Handle, dma_handle_dac);

		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	}
}

void DMA1_Stream5_IRQHandler() { HAL_DMA_IRQHandler(&dma_handle_dac); }

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
						.Direction = DMA_PERIPH_TO_MEMORY, .MemInc =
						DMA_MINC_ENABLE, .PeriphInc = DMA_PINC_DISABLE, .Mode =
								DMA_CIRCULAR,
						.Priority = DMA_PRIORITY_VERY_HIGH, .MemDataAlignment =
						DMA_MDATAALIGN_HALFWORD, .PeriphDataAlignment =
						DMA_PDATAALIGN_HALFWORD, .FIFOMode =
						DMA_FIFOMODE_DISABLE, .MemBurst =
						DMA_MBURST_INC16, .PeriphBurst = DMA_PBURST_SINGLE };

		HAL_DMA_Init(&dma_handle_adc);
		__HAL_LINKDMA(&hdac, DMA_Handle1, dma_handle_dac);

		HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	}
}

void ADC_IRQHandler() { HAL_ADC_IRQHandler(&hadc); }

void DMA1_Stream5_IRQHandler() { HAL_DMA_IRQHandler(&dma_handle_dac); }

void ADC_IRQHandler() { HAL_ADC_IRQHandler(&hadc); }

void DMA1_Stream5_IRQHandler() { HAL_DMA_IRQHandler(&dma_handle_dac); }

void DMA2_Stream0_IRQHandler() {
	HAL_DMA_IRQHandler(&dma_handle_adc);
}

void HAL_ADC_ConvHalfCpltCallback() {
	half_complete_flag = 1;
}



