/**
  ******************************************************************************
  * File Name          : FMC.c
  * Description        : This file provides code for the configuration
  *                      of the FMC peripheral.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "fmc.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SRAM_HandleTypeDef SRAM_READ;
SRAM_HandleTypeDef SRAM_WRITE;

/* FMC initialization function */
void MX_FMC_Init(void)
{
	FMC_NORSRAM_TimingTypeDef Timing;
	FMC_NORSRAM_TimingTypeDef ExtTiming;

	/** Perform the FPGA memory read initialization sequence */
	SRAM_READ.Instance = FMC_NORSRAM_DEVICE;
	SRAM_READ.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

	/* SRAM_READ.Init */
	SRAM_READ.Init.NSBank = FMC_NORSRAM_BANK1;
	SRAM_READ.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
	SRAM_READ.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
	SRAM_READ.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
	SRAM_READ.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
	SRAM_READ.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
	SRAM_READ.Init.WrapMode = FMC_WRAP_MODE_DISABLE;
	SRAM_READ.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
	SRAM_READ.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
	SRAM_READ.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
	SRAM_READ.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
	SRAM_READ.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
	SRAM_READ.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
	SRAM_READ.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;

	// Timing (Read)
	Timing.AccessMode = FMC_ACCESS_MODE_A;
	Timing.AddressSetupTime = 4;
	Timing.AddressHoldTime = 0;
	Timing.DataSetupTime = 4;
	Timing.BusTurnAroundDuration = 0; // don't care
	Timing.CLKDivision = 10; // don't care
	Timing.DataLatency = 2; // don't care

	/** Perform the FPGA memory write initialization sequence */
	SRAM_WRITE.Instance = FMC_NORSRAM_DEVICE;
	SRAM_WRITE.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

	/* SRAM_WRITE.Init */
	SRAM_WRITE.Init.NSBank = FMC_NORSRAM_BANK2;
	SRAM_WRITE.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
	SRAM_WRITE.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
	SRAM_WRITE.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
	SRAM_WRITE.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
	SRAM_WRITE.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
	SRAM_WRITE.Init.WrapMode = FMC_WRAP_MODE_DISABLE;
	SRAM_WRITE.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
	SRAM_WRITE.Init.WriteOperation = FMC_WRITE_OPERATION_DISABLE;
	SRAM_WRITE.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
	SRAM_WRITE.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
	SRAM_WRITE.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
	SRAM_WRITE.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
	SRAM_WRITE.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;

	// ExtTiming (Write)
	ExtTiming.AccessMode = FMC_ACCESS_MODE_A;
	ExtTiming.AddressSetupTime = 4;
	ExtTiming.AddressHoldTime = 0;
	ExtTiming.DataSetupTime = 4;
	ExtTiming.BusTurnAroundDuration = 0; // don't care
	ExtTiming.CLKDivision = 10; // don't care
	ExtTiming.DataLatency = 2; // don't care

	/* Send settings to FMC peripheral */
	HAL_SRAM_Init(&SRAM_READ, &Timing, &ExtTiming);
	HAL_SRAM_Init(&SRAM_WRITE, &Timing, &ExtTiming);

}


static int FMC_Initialized = 0;


static void HAL_FMC_MspInit(void) {
	/* USER CODE BEGIN FMC_MspInit 0 */

	/* USER CODE END FMC_MspInit 0 */
	GPIO_InitTypeDef GPIO_InitStruct;

	if (FMC_Initialized) {
		return;
	}
	FMC_Initialized = 1;

	/* Peripheral clock enable */
	__FMC_CLK_ENABLE();
  
	/** FMC GPIO Configuration
	PF0   ------> FMC_A0
	PF1   ------> FMC_A1
	PF2   ------> FMC_A2
	PF3   ------> FMC_A3
	PF4   ------> FMC_A4
	PF5   ------> FMC_A5
	PE7   ------> FMC_D4
	PE8   ------> FMC_D5
	PE9   ------> FMC_D6
	PE10   ------> FMC_D7
	PE11   ------> FMC_D8
	PE12   ------> FMC_D9
	PE13   ------> FMC_D10
	PE14   ------> FMC_D11
	PE15   ------> FMC_D12
	PD8   ------> FMC_D13
	PD9   ------> FMC_D14
	PD10   ------> FMC_D15
	PD14   ------> FMC_D0
	PD15   ------> FMC_D1
	PD0   ------> FMC_D2
	PD1   ------> FMC_D3
	PD4   ------> FMC_NOE
	PD5   ------> FMC_NWE
	PD7   ------> FMC_NE1
	PG9   ------> FMC_NE2
	*/

	/* USER CODE BEGIN FMC_MspInit 1 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	/* USER CODE END FMC_MspInit 1 */
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef* hsram) {
  /* USER CODE BEGIN NOR_MspInit 0 */

  /* USER CODE END NOR_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN NOR_MspInit 1 */

  /* USER CODE END NOR_MspInit 1 */
}


static int FMC_DeInitialized = 0;



static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __FMC_CLK_DISABLE();
  
	/** FMC GPIO Configuration
	PF0   ------> FMC_A0
	PF1   ------> FMC_A1
	PF2   ------> FMC_A2
	PF3   ------> FMC_A3
	PF4   ------> FMC_A4
	PF5   ------> FMC_A5
	PE7   ------> FMC_D4
	PE8   ------> FMC_D5
	PE9   ------> FMC_D6
	PE10   ------> FMC_D7
	PE11   ------> FMC_D8
	PE12   ------> FMC_D9
	PE13   ------> FMC_D10
	PE14   ------> FMC_D11
	PE15   ------> FMC_D12
	PD8   ------> FMC_D13
	PD9   ------> FMC_D14
	PD10   ------> FMC_D15
	PD14   ------> FMC_D0
	PD15   ------> FMC_D1
	PD0   ------> FMC_D2
	PD1   ------> FMC_D3
	PD4   ------> FMC_NOE
	PD5   ------> FMC_NWE
	PD7   ------> FMC_NE1
	PG9   ------> FMC_NE2
	*/

  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_7|GPIO_PIN_0|GPIO_PIN_5|GPIO_PIN_1 
                          |GPIO_PIN_4|GPIO_PIN_15|GPIO_PIN_10|GPIO_PIN_14 
                          |GPIO_PIN_9|GPIO_PIN_8);

  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5);

  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);

  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_14 
                          |GPIO_PIN_7|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_15 
                          |GPIO_PIN_13);

  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* hsram) {
  /* USER CODE BEGIN NOR_MspDeInit 0 */

  /* USER CODE END NOR_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN NOR_MspDeInit 1 */

  /* USER CODE END NOR_MspDeInit 1 */
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
