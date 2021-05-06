/**
  ******************************************************************************
  * File Name          : se3_fpga.h
  * Description        : FPGA Primitives for programming and communicating
  *                      with the device FPGA, both in bit banging/pin manual
  *                      setting mode, and in FMC/memory mode
  ******************************************************************************
  *
  * Copyright(c) 2016-present Blu5 Group <https://www.blu5group.com>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, see <https://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

#ifndef SE3_FPGA_H_
#define SE3_FPGA_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sram.h"
#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

// Peripheral Base Address (FMC mode)

#define FPGA_SRAM_BASE_ADDR    0x60000000U

// Lattice Cable Pins

#define    pinTDI       1
#define    pinTCK       2
#define    pinTMS       4
#define    pinENABLE    8
#define    pinTRST      16
#define    pinCE        32
#define    pinTDO       64

// Errors

#define ERR_VERIFY0_FAIL			-1
#define ERR_FIND_ALGO_FILE			-2
#define ERR_FIND_DATA_FILE			-3
#define ERR_WRONG_VERSION			-4
#define ERR_ALGO_FILE_ERROR			-5
#define ERR_DATA_FILE_ERROR			-6
#define ERR_OUT_OF_MEMORY			-7

// Registers Bit

#define SIR_DATA		0x0001	/*** Current command is SIR ***/
#define SDR_DATA		0x0002	/*** Current command is SDR ***/
#define TDI_DATA		0x0004	/*** Command contains TDI ***/
#define TDO_DATA		0x0008	/*** Command contains TDO ***/
#define MASK_DATA		0x0010	/*** Command contains MASK ***/
#define DTDI_DATA		0x0020	/*** Verification flow ***/
#define DTDO_DATA		0x0040	/*** Verification flow ***/
#define COMPRESS		0x0080	/*** Compressed data file ***/
#define COMPRESS_FRAME	0x0100	/*** Compressed data frame ***/

// JTAG States

#define RESET      0x00
#define IDLE       0x01
#define IRPAUSE    0x02
#define DRPAUSE    0x03
#define SHIFTIR    0x04
#define SHIFTDR    0x05
#define DRCAPTURE  0x06

// VME Opcodes

#define STATE			0x10
#define SIR				0x11
#define SDR				0x12
#define TCK				0x1B
#define WAIT			0x1A
#define ENDDR			0x02
#define ENDIR			0x03
#define HIR				0x06
#define TIR				0x07
#define HDR				0x08
#define TDR		        0x09
#define TDI				0x13
#define CONTINUE		0x70
#define TDO				0x14
#define MASK			0x15
#define LOOP			0x58
#define ENDLOOP			0x59
#define LCOUNT			0x66
#define LDELAY			0x67
#define LSDR			0x68
#define ENDSTATE		0x69
#define ENDVME			0x7F

#define BEGIN_REPEAT	0xA0
#define END_REPEAT		0xA1
#define END_FRAME		0xA2
#define DATA			0xA3
#define PROGRAM			0xA4
#define VERIFY0			0xA5
#define DTDI			0xA6
#define DTDO			0xA7

#define signalENABLE	0x1C    /*assert the ispEN pin*/
#define signalTMS		0x1D    /*assert the MODE or TMS pin*/
#define signalTCK		0x1E    /*assert the SCLK or TCK pin*/
#define signalTDI		0x1F    /*assert the SDI or TDI pin*/
#define signalTRST		0x20    /*assert the RESET or TRST pin*/
#define signalTDO		0x21    /*assert the RESET or TDO pin*/
#define signalCableEN   0x22    /*assert the RESET or CableEN pin*/



/** \brief Program the FPGA according to the provided bitstream,
 * 	sets the correct clock frequency and connects the interrupt line
 * 	to the NVIC.
 *  Manage the CPU-FPGA JTAG interface in bit-banging to flash the bitstream
 *  into the FPGA. The bitstream is provided into s3_fpga_bitstream.h
 *  The clock frequency should be set according to the maximum frequency
 *  of the design.
 *  \param rcc_mcodiv divider for the clock frequency
 *  Possible values:
 *  RCC_MCODIV_1	CPU freq./1 (180 MHz default)
 *  RCC_MCODIV_2	CPU freq./2 (90 MHz default)
 *  RCC_MCODIV_3	CPU freq./3 (60 MHz default)
 *  RCC_MCODIV_4	CPU freq./4 (45 MHz default)
 *  RCC_MCODIV_5	CPU freq./5 (36 MHz default)
 *  \return 0 on success, else one of the ERR codes listed above
 */
int32_t se3_FPGA_Init (const uint32_t rcc_mcodiv);


/** \brief Set MUXes for fundamentals FPGA pins
 *
 *  \param mux code for the pin (one-hot encoding
 *  \return none
 */
void se3_FPGA_SetMux (uint8_t mux);


/** \brief Set CPU-FPGA interface pins manually
 *
 *  \param mux gpioNum the constant indicating the pin number
 *  \param set the value to be set (GPIO_PIN_SET/GPIO_PIN_RESET)
 *  \remark Pin-constant mapping:
 *  CPU_FPGA_BUS_D(0) 	PD14	0
 *	CPU_FPGA_BUS_D(1) 	PD15	1
 *	CPU_FPGA_BUS_D(2) 	PD0		2
 *	CPU_FPGA_BUS_D(3) 	PD1		3
 *	CPU_FPGA_BUS_D(4) 	PE7		4
 *	CPU_FPGA_BUS_D(5) 	PE8		5
 *	CPU_FPGA_BUS_D(6) 	PE9		6
 *	CPU_FPGA_BUS_D(7) 	PE10	7
 *	CPU_FPGA_BUS_D(8) 	PE11	8
 *	CPU_FPGA_BUS_D(9) 	PE12	9
 *	CPU_FPGA_BUS_D(10)  PE13	10
 *	CPU_FPGA_BUS_D(11)	PE14	11
 *	CPU_FPGA_BUS_D(12)	PE15	12
 *	CPU_FPGA_BUS_D(13)	PD8		13
 *	CPU_FPGA_BUS_D(14)	PD9		14
 *	CPU_FPGA_BUS_D(15)	PD10	15
 *	CPU_FPGA_BUS_A(0) 	PF0		16
 *	CPU_FPGA_BUS_A(1) 	PF1		17
 *	CPU_FPGA_BUS_A(2) 	PF2		18
 *	CPU_FPGA_BUS_A(3) 	PF3		19
 *	CPU_FPGA_BUS_A(4) 	PF4		20
 *	CPU_FPGA_BUS_A(5) 	PF5		21
 *	CPU_FPGA_BUS_NOE  	PD4		22
 *	CPU_FPGA_BUS_NWE  	PD5		23
 *  \return none
 */
void se3_FPGA_FpgaCpuGPIO (uint8_t gpioNum, GPIO_PinState set);


/** \brief Reset the FPGA to bring it to a known state
 */
void se3_FPGA_Reset (void);


/** \brief Read a 16-bit word from the FPGA
 * 	\remark This call only works in FMC/memory mode. FPGA internal design
 * 	should be done in order to be able to correctly interpret memory signals.
 *  \param address memory offset within the FPGA (from 0x01 (1) to 0x3F (63)
 *  according to best possibilities of CPU-FPGA interface)
 *  \param dataPtr pointer to be filled with the data read
 *  \return Returns 0 on success
 */
uint8_t se3_FPGA_Read (uint8_t address, uint16_t *dataPtr);


/** \brief Write a 16-bit word from the FPGA
 * 	\remark This call only works in FMC/memory mode. FPGA internal design
 * 	should be done in order to be able to correctly interpret memory signals.
 *  \param address memory offset within the FPGA (from 0x01 (1) to 0x3F (63)
 *  according to best possibilities of CPU-FPGA interface)
 *  \param dataPtr pointer to to the data to be written in the FPGA
 *  \return Returns 0 on success
 */
uint8_t se3_FPGA_Write (uint8_t address, uint16_t *dataPtr);


#endif /* SE3_FPGA_H_ */
