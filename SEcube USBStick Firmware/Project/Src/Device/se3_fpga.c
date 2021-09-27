/**
  ******************************************************************************
  * File Name          : se3_fpga.c
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

#include <se3_fpga.h>
#include <se3_fpga_bitstream.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "fmc.h"


/*************************************************************
 *                                                            *
 * GLOBAL VARIABLES                                           *
 *                                                            *
 *************************************************************/
int32_t g_iMovingAlgoIndex = 0;	    /*** variable to hold the current index in the algo array ***/
int32_t g_iMovingDataIndex = 0;		/*** variable to hold the current index in the data array ***/
uint16_t g_usDataType = 0x0000; /*** data type register used to hold information ***
                                       **** about the algorithm and data ***/
uint8_t g_cEndDR = 0;		/*** used to hold the ENDDR state. ***/
uint8_t g_cEndIR = 0;		/*** used to hold the ENDIR state. ***/
int16_t g_siHeadDR = 0;		/*** used to hold the header data register ***/
int16_t g_siHeadIR = 0;		/*** used to hold the header instruction register ***/
int16_t g_siTailDR = 0;		/*** used to hold the trailer data register ***/
int16_t g_siTailIR = 0;		/*** used to hold the trailer instruction register ***/

int32_t g_iMainDataIndex = 0;		/*** forward - only index used as a placed holder in the data array ***/
int32_t g_iRepeatIndex = 0;		    /*** Used to point32_t to the location of REPEAT data ***/
int32_t g_iTDIIndex = 0;			/*** Used to point32_t to the location of TDI data ***/
int32_t g_iTDOIndex = 0;			/*** Used to point32_t to the location of TDO data ***/
int32_t g_iMASKIndex = 0;			/*** Used to point32_t to the location of MASK data ***/
uint8_t g_ucCompressCounter = 0; /*** used to indicate how many times 0xFF is repeated ***/

int16_t g_siIspPins = 0x00;   /*** holds the current byte to be sent to the hardware ***/
char g_cCurrentJTAGState = 0;	/*** holds the current state of JTAG state machine ***/

int32_t  g_iLoopIndex = 0;
int32_t  g_iLoopMovingIndex = 0;	/*** Used to point32_t to the location of LOOP data ***/
int32_t  g_iLoopDataMovingIndex = 0;

uint16_t g_usLCOUNTSize	= 0;
uint8_t  g_ucLDELAYState = IDLE;
uint16_t g_ucLDELAYTCK = 0;
uint16_t g_ucLDELAYDelay = 0;
uint16_t m_loopState = 0;



// External Variables

extern int16_t g_siIspPins;

// External Functions

extern void ispVMStateMachine( char a_cNextState );


uint8_t readPort()
{
	uint8_t ucRet = 0;

	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_SET)
			ucRet = 0x01;
	else
			ucRet = 0x00;


	return ( ucRet );
}


void writePort( uint8_t a_ucPins, uint8_t a_ucValue )
{

	if(a_ucValue)
	{
		if(a_ucPins == pinTDI)
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
		else if(a_ucPins == pinTCK)
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
		else if(a_ucPins == pinTMS)
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
	}
	else
	{
		if(a_ucPins == pinTDI)
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
		else if(a_ucPins == pinTCK)
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
		else if(a_ucPins == pinTMS)
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}



void ispVMDelay( uint32_t a_msecDelay )
{
	uint32_t start = HAL_GetTick();

	while ((HAL_GetTick() - start) <= a_msecDelay){

	}
}




void EnableHardware()
{
	ispVMStateMachine(RESET);
}




void DisableHardware()
{
	ispVMStateMachine(RESET);
}






uint32_t ispVMDataSize (void);
int16_t ispVMShiftExec (uint32_t a_uiDataSize);
int16_t ispVMShift (char a_cCommand);
uint8_t GetByte (int32_t a_iCurrentIndex, char a_cAlgo);
void ispVMStateMachine (char a_cNextState);
void ispVMClocks (uint32_t a_usClocks);
void ispVMBypass (uint32_t a_siLength);
void sclock (void);
int16_t ispVMRead (uint32_t a_uiDataSize);
void ispVMSend (uint32_t a_uiDataSize);
void ispVMLCOUNT (uint16_t a_usCountSize);
void ispVMLDELAY (void);

// Overall FPGA programming function
int32_t FPGA_Programming(void);

/*************************************************************
*                                                            *
* EXTERNAL FUNCTION                                          *
*                                                            *
*************************************************************/

extern void ispVMDelay (uint32_t a_usDelay);
extern uint8_t readPort (void);
extern void writePort (uint8_t a_ucPins, uint8_t a_ucValue);



/*************************************************************
*                                                            *
* EXTERNAL VARIABLES                                         *
*                                                            *
*     If the algorithm does not require the data, then       *
*     declare the variables g_pucDataArray and g_iDataSize   *
*     as local variables and set them to NULL and 0,         *
*     respectively.                                          *
*                                                            *
*     Example:                                               *
*          xdata uint8_t * g_pucDataArray = NULL;      *
*          xdata int32_t g_iDataSize = 0;                        *
*                                                            *
*************************************************************/




const struct iState
{
	/*** JTAG state machine transistion table ***/
	uint8_t  CurState;		/*** From this state ***/
	uint8_t  NextState;		/*** Step to this state ***/
	uint8_t  Pattern;			/*** The pattern of TMS ***/
	uint8_t  Pulses;			/*** The number of steps ***/
} iStates[25] =
{
	{ DRPAUSE,	SHIFTDR,	0x80, 2 },
	{ IRPAUSE,	SHIFTIR,	0x80, 2 },
	{ SHIFTIR,	IRPAUSE,	0x80, 2 },
	{ SHIFTDR,	DRPAUSE,	0x80, 2 },
	{ DRPAUSE,	IDLE,		0xC0, 3 },
	{ IRPAUSE,	IDLE,		0xC0, 3 },
	{ RESET,	IDLE,		0x00, 1 },
	{ RESET,	DRPAUSE,	0x50, 5 },
	{ RESET,	IRPAUSE,	0x68, 6 },
	{ IDLE,		RESET,		0xE0, 3 },
	{ IDLE,		DRPAUSE,	0xA0, 4 },
	{ IDLE,		IRPAUSE,	0xD0, 5 },
	{ DRPAUSE,	RESET,		0xF8, 5 },
	{ DRPAUSE,	IRPAUSE,	0xF4, 7 },
	{ DRPAUSE,	DRPAUSE,	0xE8, 6 },  /* 06/14/06 Support POLING STATUS LOOP*/
	{ IRPAUSE,	RESET,		0xF8, 5 },
	{ IRPAUSE,	DRPAUSE,	0xE8, 6 },
	{ IRPAUSE,	SHIFTDR,	0xE0, 5 },
	{ SHIFTIR,	IDLE,		0xC0, 3 },
	{ SHIFTDR,	IDLE,		0xC0, 3 },
	{ RESET,	RESET,		0xFC, 6 },
	{ DRPAUSE,	DRCAPTURE,	0xE0, 4 }, /* 11/15/05 Support DRCAPTURE*/
	{ DRCAPTURE, DRPAUSE,	0x80, 2 },
	{ IDLE,     DRCAPTURE,	0x80, 2 },
	{ IRPAUSE,  DRCAPTURE, 	0xE0, 4 }
};
/*************************************************************
*                                                            *
* ISPPROCESSVME                                              *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     The return value indicates whether the vme was         *
*     processed successfully or not.  A return value equal   *
*     to or greater than 0 is passing, and less than 0 is    *
*     failing.                                               *
*                                                            *
* DESCRIPTION:                                               *
*     This function is the core of the embedded processor.   *
*     It extracts the VME file for the high - level tokens   *
*     such as SIR, SDR, STATE, etc, and calls the            *
*     appropriate functions to process them.                 *
*                                                            *
*************************************************************/

int16_t ispProcessVME()
{
	uint8_t ucOpcode        = 0;
	uint8_t ucState         = 0;
	int16_t siRetCode           = 0;
	static char cProgram          = 0;
	uint32_t uiDataSize       = 0;
	int32_t iLoopCount                = 0;
	uint32_t iMovingAlgoIndex = 0;

	/*************************************************************
	*                                                            *
	* Begin processing the vme algorithm and data files.         *
	*                                                            *
	*************************************************************/

	while ((ucOpcode = GetByte(g_iMovingAlgoIndex++, 1)) != 0xFF)
	{
		/*************************************************************
		*                                                            *
		* This switch statement is the main switch that represents   *
		* the core of the embedded processor.                        *
		*                                                            *
		*************************************************************/

		switch (ucOpcode)
		{
		case STATE:
			/*************************************************************
			*                                                            *
			* Move the state.                                            *
			*                                                            *
			*************************************************************/
			ispVMStateMachine(GetByte(g_iMovingAlgoIndex++, 1));
			break;
		case SIR:
		case SDR:
			/*************************************************************
			*                                                            *
			* Execute SIR/SDR command.                                   *
			*                                                            *
			*************************************************************/
			siRetCode = ispVMShift(ucOpcode);
			break;
		case TCK:
			/*************************************************************
			*                                                            *
			* Pulse TCK signal the specified time.                       *
			*                                                            *
			*************************************************************/
			ispVMClocks(ispVMDataSize());
			break;
		case WAIT:
			/*************************************************************
			*                                                            *
			* Issue delay in specified time.                             *
			*                                                            *
			*************************************************************/
			ispVMDelay(ispVMDataSize());
			break;
		case ENDDR:
			/*************************************************************
			*                                                            *
			* Get the ENDDR state and store in global variable.          *
			*                                                            *
			*************************************************************/
			g_cEndDR = GetByte(g_iMovingAlgoIndex++, 1);
			break;
		case ENDIR:
			/*************************************************************
			*                                                            *
			* Get the ENDIR state and store in global variable.          *
			*                                                            *
			*************************************************************/
			g_cEndIR = GetByte(g_iMovingAlgoIndex++, 1);
			break;
		case HIR:
			g_siHeadIR = (int16_t) ispVMDataSize();
			break;
		case TIR:
			g_siTailIR = (int16_t) ispVMDataSize();
			break;
		case HDR:
			g_siHeadDR = (int16_t) ispVMDataSize();

			break;
		case TDR:
			g_siTailDR = (int16_t) ispVMDataSize();

			break;
		case BEGIN_REPEAT:
			/*************************************************************
			*                                                            *
			* Execute repeat loop.                                       *
			*                                                            *
			*************************************************************/

			uiDataSize = ispVMDataSize();

			switch (GetByte(g_iMovingAlgoIndex++, 1))
			{
			case PROGRAM:
				/*************************************************************
				*                                                            *
				* Set the main data index to the moving data index.  This    *
				* allows the processor to remember the beginning of the      *
				* data.  Set the cProgram variable to true to indicate to    *
				* the VERIFY0 flow later that a programming flow has been     *
				* completed so the moving data index must return to the      *
				* main data index.                                           *
				*                                                            *
				*************************************************************/
				g_iMainDataIndex = g_iMovingDataIndex;
				cProgram = 1;
				break;
			case VERIFY0:
				/*************************************************************
				*                                                            *
				* If the static variable cProgram has been set, then return  *
				* the moving data index to the main data index because this  *
				* is a erase, program, VERIFY0 operation.  If the programming *
				* flag is not set, then this is a VERIFY0 only operation thus *
				* no need to return the moving data index.                   *
				*                                                            *
				*************************************************************/
				if (cProgram)
				{
					g_iMovingDataIndex = g_iMainDataIndex;
					cProgram = 0;
				}
				break;
			}

			/*************************************************************
			*                                                            *
			* Set the repeat index to the first byte in the repeat loop. *
			*                                                            *
			*************************************************************/

			g_iRepeatIndex = g_iMovingAlgoIndex;

			for (; uiDataSize > 0; uiDataSize--)
			{
				/*************************************************************
				*                                                            *
				* Initialize the current algorithm index to the beginning of *
				* the repeat index before each repeat loop.                  *
				*                                                            *
				*************************************************************/

				g_iMovingAlgoIndex = g_iRepeatIndex;

				/*************************************************************
				*                                                            *
				* Make recursive call.                                       *
				*                                                            *
				*************************************************************/

				siRetCode = ispProcessVME();
				if (siRetCode < 0)
				{
					break;
				}
			}
			break;
			case END_REPEAT:
				/*************************************************************
				*                                                            *
				* Exit the current repeat frame.                             *
				*                                                            *
				*************************************************************/
					return siRetCode;
			case LOOP:
				/*************************************************************
				*                                                            *
				* Execute repeat loop.                                       *
				*                                                            *
				*************************************************************/

				g_usLCOUNTSize = (int16_t)ispVMDataSize();

#ifdef VME_DEBUG
				printf( "MaxLoopCount %d\n", g_usLCOUNTSize );
#endif
				/*************************************************************
				*                                                            *
				* Set the repeat index to the first byte in the repeat loop. *
				*                                                            *
				*************************************************************/

				g_iLoopMovingIndex = g_iMovingAlgoIndex;
				g_iLoopDataMovingIndex = g_iMovingDataIndex;


				for ( g_iLoopIndex = 0 ; g_iLoopIndex < g_usLCOUNTSize; g_iLoopIndex++ ) {
					m_loopState = 1;
					/*************************************************************
					*                                                            *
					* Initialize the current algorithm index to the beginning of *
					* the repeat index before each repeat loop.                  *
					*                                                            *
					*************************************************************/

					g_iMovingAlgoIndex = g_iLoopMovingIndex;
					g_iMovingDataIndex = g_iLoopDataMovingIndex;


					/*************************************************************
					*                                                            *
					* Make recursive call.                                       *
					*                                                            *
					*************************************************************/

					siRetCode = ispProcessVME();
					if ( !siRetCode ) {
						/*************************************************************
						*                                                            *
						* Stop if the complete status matched.                       *
						*                                                            *
						*************************************************************/

						break;
					}
				}
				m_loopState = 0;

				if (siRetCode != 0) {
					/*************************************************************
					*                                                            *
					* Return if the complete status error.                       *
					*                                                            *
					*************************************************************/
					return (siRetCode);
				}
				break;
			case ENDLOOP:
				/*************************************************************
				*                                                            *
				* End the current loop.                                      *
				*                                                            *
				*************************************************************/
				if(m_loopState)
					return siRetCode;
				break;
			case ENDVME:
				/*************************************************************
				*                                                            *
				* If the ENDVME token is found and g_iMovingAlgoIndex is     *
				* greater than or equal to g_iAlgoSize, then that indicates  *
				* the end of the chain.  If g_iMovingAlgoIndex is less than  *
				* g_iAlgoSize, then that indicates that there are still more *
				* devices to be processed.                                   *
				*                                                            *
				*************************************************************/
				if (g_iMovingAlgoIndex >= g_iAlgoSize)
				{
					return siRetCode;
				}
				break;
			case LCOUNT:
				/*************************************************************
				*                                                            *
				* Get the Maximum LoopCount and store in global variable.    *
				*                                                            *
				*************************************************************/
				ispVMLCOUNT((uint16_t) ispVMDataSize());
				break;
			case LDELAY:
				/*************************************************************
				*                                                            *
				* Get the State,TCK number and Delay time for the poling loop*
				* and store in global variable.                              *
				*                                                            *
				*************************************************************/
				ispVMLDELAY();
				break;
			case LSDR:
				/*************************************************************
				*                                                            *
				* Execute repeat poling status loop.                         *
				*                                                            *
				*************************************************************/
				iMovingAlgoIndex = g_iMovingAlgoIndex;
				for (iLoopCount = 0; iLoopCount < g_usLCOUNTSize; iLoopCount++)
				{
					siRetCode = ispVMShift(SDR);
					if (!siRetCode)
					{
						break;
					}
					/*************************************************************
					*                                                            *
					* If the status is not done, then move to the setting State  *
					* execute the delay and come back and do the checking again  *
					*                                                            *
					*************************************************************/
					g_iMovingAlgoIndex = iMovingAlgoIndex;
					ispVMStateMachine(DRPAUSE);
					m_loopState = 1;
					ispVMStateMachine(g_ucLDELAYState);
					m_loopState = 0;
					ispVMClocks(g_ucLDELAYTCK);
					ispVMDelay(g_ucLDELAYDelay);
				}
				if (siRetCode != 0)
				{
					return (siRetCode);
				}
				break;
			case signalENABLE:
				/******************************************************************
				* Toggle ispENABLE signal                                         *
				*                                                                 *
				******************************************************************/
				ucState = GetByte(g_iMovingAlgoIndex++, 1);
				if (ucState == 0x01)
					writePort(pinENABLE, 0x01);
				else
					writePort(pinENABLE, 0x00);
				ispVMDelay(1);
				break;
			case signalTRST:
				/******************************************************************
				* Toggle TRST signal                                              *
				*                                                                 *
				******************************************************************/
				ucState = GetByte(g_iMovingAlgoIndex++, 1);
				if (ucState == 0x01)
					writePort(pinTRST, 0x01);
				else
					writePort(pinTRST, 0x00);
				ispVMDelay(1);
				break;
			default:
				/*************************************************************
				*                                                            *
				* Unrecognized opcode.  Return with file error.              *
				*                                                            *
				*************************************************************/
				return ERR_ALGO_FILE_ERROR;
		}

		if (siRetCode < 0)
		{
			return siRetCode;
		}
	}

	return ERR_ALGO_FILE_ERROR;
}

/*************************************************************
*                                                            *
* ISPVMDATASIZE                                              *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     This function returns a number indicating the size of  *
*     the instruction.                                       *
*                                                            *
* DESCRIPTION:                                               *
*     This function returns a number.  The number is the     *
*     value found in SVF commands such as SDR, SIR, HIR, and *
*     etc.  For example:                                     *
*               SDR 200 TDI(FFF..F);                         *
*     The return value would be 200.                         *
*                                                            *
*************************************************************/

uint32_t ispVMDataSize()
{
	uint32_t uiSize = 0;
	uint8_t ucCurrentByte = 0;
	uint8_t ucIndex = 0;

	while ((ucCurrentByte = GetByte(g_iMovingAlgoIndex++, 1)) & 0x80)
	{
		uiSize |=((uint32_t)(ucCurrentByte & 0x7F)) << ucIndex;
		ucIndex += 7;
	}
	uiSize |=((uint32_t)(ucCurrentByte & 0x7F)) << ucIndex;
	return uiSize;
}

/*************************************************************
*                                                            *
* ISPVMSHIFTEXEC                                             *
*                                                            *
* INPUT:                                                     *
*     a_uiDataSize: this holds the size of the command.      *
*                                                            *
* RETURN:                                                    *
*     Returns 0 if passing, -1 if failing.                   *
*                                                            *
* DESCRIPTION:                                               *
*     This function handles the data in the SIR/SDR commands *
*     by either decompressing the data or setting the        *
*     respective indexes to point32_t to the appropriate         *
*     location in the algo or data array.  Note that data    *
*     only comes after TDI, DTDI, TDO, DTDO, and MASK.       *
*                                                            *
*************************************************************/

int16_t ispVMShiftExec(uint32_t a_uiDataSize)
{
	uint8_t ucDataByte = 0;

	/*************************************************************
	*                                                            *
	* Reset the data type register.                              *
	*                                                            *
	*************************************************************/

	g_usDataType &= ~(TDI_DATA + TDO_DATA + MASK_DATA + DTDI_DATA + DTDO_DATA + COMPRESS_FRAME);

	/*************************************************************
	*                                                            *
	* Convert the size from bits to byte.                        *
	*                                                            *
	*************************************************************/

	if (a_uiDataSize % 8)
	{
		a_uiDataSize = a_uiDataSize / 8 + 1;
	}
	else
	{
		a_uiDataSize = a_uiDataSize / 8;
	}

	/*************************************************************
	*                                                            *
	* Begin extracting the command.                              *
	*                                                            *
	*************************************************************/

	while ((ucDataByte = GetByte(g_iMovingAlgoIndex++, 1)) != CONTINUE)
	{
		switch (ucDataByte)
		{
		case TDI:
		/*************************************************************
		*                                                            *
		* Set data type register to indicate TDI data and set TDI    *
		* index to the current algorithm location.                   *
		*                                                            *
		*************************************************************/
			g_usDataType |= TDI_DATA;
			g_iTDIIndex = g_iMovingAlgoIndex;
			g_iMovingAlgoIndex += a_uiDataSize;
			break;
		case DTDI:
		/*************************************************************
		*                                                            *
		* Set data type register to indicate DTDI data and check the *
		* next byte to make sure it's the DATA byte.  DTDI indicates *
		* that the data should be read from the data array, not the  *
		* algo array.                                                *
		*                                                            *
		*************************************************************/
			g_usDataType |= DTDI_DATA;
			if (GetByte(g_iMovingAlgoIndex++, 1) != DATA)
			{
				return ERR_ALGO_FILE_ERROR;
			}

			/*************************************************************
			*                                                            *
			* If the COMPRESS flag is set, read the next byte from the   *
			* data file array.  If the byte is true, then that indicates *
			* the frame was compressable.  Note that even though the     *
			* overall data file was compressed, certain frames may not   *
			* be compressable that is why this byte must be checked.     *
			*                                                            *
			*************************************************************/
			if (g_usDataType & COMPRESS)
			{
				if (GetByte(g_iMovingDataIndex++, 0))
				{
					g_usDataType |= COMPRESS_FRAME;
				}
			}
			break;
		case TDO:
		/*************************************************************
		*                                                            *
		* Set data type register to indicate TDO data and set TDO    *
		* index to the current algorithm location.                   *
		*                                                            *
		*************************************************************/
			g_usDataType |= TDO_DATA;
			g_iTDOIndex = g_iMovingAlgoIndex;
			g_iMovingAlgoIndex += a_uiDataSize;
			break;
		case DTDO:
		/*************************************************************
		*                                                            *
		* Set data type register to indicate DTDO data and check the *
		* next byte to make sure it's the DATA byte.  DTDO indicates *
		* that the data should be read from the data array, not the  *
		* algo array.                                                *
		*                                                            *
		*************************************************************/
			g_usDataType |= DTDO_DATA;
			if (GetByte(g_iMovingAlgoIndex++, 1) != DATA)
			{
				return ERR_ALGO_FILE_ERROR;
			}

			/*************************************************************
			*                                                            *
			* If the COMPRESS flag is set, read the next byte from the   *
			* data file array.  If the byte is true, then that indicates *
			* the frame was compressable.  Note that even though the     *
			* overall data file was compressed, certain frames may not   *
			* be compressable that is why this byte must be checked.     *
			*                                                            *
			*************************************************************/
			if (g_usDataType & COMPRESS)
			{
				if (GetByte(g_iMovingDataIndex++, 0))
				{
					g_usDataType |= COMPRESS_FRAME;
				}
			}
			break;
		case MASK:
		/*************************************************************
		*                                                            *
		* Set data type register to indicate MASK data.  Set MASK    *
		* location index to current algorithm array position.        *
		*                                                            *
		*************************************************************/
			g_usDataType |= MASK_DATA;
			g_iMASKIndex = g_iMovingAlgoIndex;
			g_iMovingAlgoIndex += a_uiDataSize;
			break;
		default:
		/*************************************************************
		*                                                            *
		* Unrecognized or misplaced opcode.  Return error.           *
		*                                                            *
		*************************************************************/
			return ERR_ALGO_FILE_ERROR;
		}
	}

	/*************************************************************
	*                                                            *
	* Reached the end of the instruction.  Return passing.       *
	*                                                            *
	*************************************************************/

	return 0;
}

/*************************************************************
*                                                            *
* ISPVMSHIFT                                                 *
*                                                            *
* INPUT:                                                     *
*     a_cCommand: this argument specifies either the SIR or  *
*     SDR command.                                           *
*                                                            *
* RETURN:                                                    *
*     The return value indicates whether the SIR/SDR was     *
*     processed successfully or not.  A return value equal   *
*     to or greater than 0 is passing, and less than 0 is    *
*     failing.                                               *
*                                                            *
* DESCRIPTION:                                               *
*     This function is the entry point32_t to execute an SIR or  *
*     SDR command to the device.                             *
*                                                            *
*************************************************************/

int16_t ispVMShift(char a_cCommand)
{
	int16_t siRetCode = 0;
	uint32_t uiDataSize = ispVMDataSize();

	/*************************************************************
	*                                                            *
	* Clear any existing SIR/SDR instructions from the data type *
	* register.                                                  *
	*                                                            *
	*************************************************************/

	g_usDataType &= ~(SIR_DATA + SDR_DATA);

	/*************************************************************
	*                                                            *
	* Move state machine to appropriate state depending on the   *
	* command.  Issue bypass if needed.                          *
	*                                                            *
	*************************************************************/

	switch (a_cCommand)
	{
	case SIR:
	/*************************************************************
	*                                                            *
	* Set the data type register to indicate that it's executing *
	* an SIR instruction.  Move state machine to IRPAUSE,        *
	* SHIFTIR.  If header instruction register exists, then      *
	* issue bypass.                                              *
	*                                                           *
	*************************************************************/
		g_usDataType |= SIR_DATA;
		ispVMStateMachine(IRPAUSE);
		ispVMStateMachine(SHIFTIR);
		if (g_siHeadIR > 0)
		{
			ispVMBypass(g_siHeadIR);
			sclock();
		}
		break;
	case SDR:
	/*************************************************************
	*                                                            *
	* Set the data type register to indicate that it's executing *
	* an SDR instruction.  Move state machine to DRPAUSE,        *
	* SHIFTDR.  If header data register exists, then issue       *
	* bypass.                                                    *
	*                                                            *
	*************************************************************/
		g_usDataType |= SDR_DATA;
		ispVMStateMachine(DRPAUSE);
		ispVMStateMachine(SHIFTDR);
		if (g_siHeadDR > 0)
		{
			ispVMBypass(g_siHeadDR);
			sclock();
		}
		break;
	}

	/*************************************************************
	*                                                            *
	* Set the appropriate index locations.  If error then return *
	* error code immediately.                                    *
	*                                                            *
	*************************************************************/

	siRetCode = ispVMShiftExec(uiDataSize);

	if (siRetCode < 0)
	{
		return siRetCode;
	}

	/*************************************************************
	*                                                            *
	* Execute the command to the device.  If TDO exists, then    *
	* read from the device and VERIFY0.  Else only TDI exists     *
	* which must send data to the device only.                   *
	*                                                            *
	*************************************************************/

	if ((g_usDataType & TDO_DATA) ||(g_usDataType & DTDO_DATA))
	{
		siRetCode = ispVMRead(uiDataSize);
		/*************************************************************
		*                                                            *
		* A frame of data has just been read and verified.  If the   *
		* DTDO_DATA flag is set, then check to make sure the next    *
		* byte in the data array, which is the last byte of the      *
		* frame, is the END_FRAME byte.                              *
		*                                                            *
		*************************************************************/
		if (g_usDataType & DTDO_DATA)
		{
			if (GetByte(g_iMovingDataIndex++, 0) != END_FRAME)
			{
				siRetCode = ERR_DATA_FILE_ERROR;
			}
		}
	}
	else
	{
		ispVMSend(uiDataSize);
		/*************************************************************
		*                                                            *
		* A frame of data has just been sent.  If the DTDI_DATA flag *
		* is set, then check to make sure the next byte in the data  *
		* array, which is the last byte of the frame, is the         *
		* END_FRAME byte.                                            *
		*                                                            *
		*************************************************************/
		if (g_usDataType & DTDI_DATA)
		{
			if (GetByte(g_iMovingDataIndex++, 0) != END_FRAME)
			{
				siRetCode = ERR_DATA_FILE_ERROR;
			}
		}
	}

	/*************************************************************
	*                                                            *
	* Bypass trailer if it exists.  Move state machine to        *
	* ENDIR/ENDDR state.                                         *
	*                                                            *
	*************************************************************/

	switch (a_cCommand)
	{
	case SIR:
		if (g_siTailIR > 0)
		{
			sclock();
			ispVMBypass(g_siTailIR);
		}
		ispVMStateMachine(g_cEndIR);
		break;
    case SDR:
		if (g_siTailDR > 0)
		{
			sclock();
			ispVMBypass(g_siTailDR);
		}
		ispVMStateMachine(g_cEndDR);
		break;
	}

	return siRetCode;
}

/*************************************************************
*                                                            *
* GETBYTE                                                    *
*                                                            *
* INPUT:                                                     *
*     a_iCurrentIndex: the current index to access.          *
*                                                            *
*     a_cAlgo: 1 if the return byte is to be retrieved from  *
*     the algorithm array, 0 if the byte is to be retrieved  *
*     from the data array.                                   *
*                                                            *
* RETURN:                                                    *
*     This function returns a byte of data from either the   *
*     algorithm or data array.  It returns -1 if out of      *
*     bounds.                                                *
*                                                            *
*************************************************************/

uint8_t GetByte(int32_t a_iCurrentIndex, char a_cAlgo)
{
	uint8_t ucData = 0;

	if (a_cAlgo)
	{
	/*************************************************************
	*                                                            *
	* If the current index is still within range, then return    *
	* the next byte.  If it is out of range, then return -1.     *
	*                                                            *
	*************************************************************/
		if (a_iCurrentIndex >= g_iAlgoSize)
		{
			return (unsigned char) 0xFF;
		}
		ucData = (unsigned char) __fpga_alg[a_iCurrentIndex];
	}
	else
	{
	/*************************************************************
	*                                                            *
	* If the current index is still within range, then return    *
	* the next byte.  If it is out of range, then return -1.     *
	*                                                            *
	*************************************************************/
		if (a_iCurrentIndex >= g_iDataSize)
		{
			return (unsigned char) 0xFF;
		}
		ucData = (unsigned char) __fpga_data[a_iCurrentIndex];
	}

	return ucData;
}

/*************************************************************
*                                                            *
* SCLOCK                                                     *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     This function applies a HLL pulse to TCK.              *
*                                                            *
*************************************************************/

void sclock()
{
/*************************************************************
*                                                            *
* Set TCK to HIGH, LOW, LOW.                                 *
*                                                            *
*************************************************************/

	writePort(pinTCK, 0x01);
	writePort(pinTCK, 0x00);
	writePort(pinTCK, 0x00);
}

/*************************************************************
*                                                            *
* ISPVMREAD                                                  *
*                                                            *
* INPUT:                                                     *
*     a_uiDataSize: this argument is the size of the         *
*     command.                                               *
*                                                            *
* RETURN:                                                    *
*     The return value is 0 if passing, and -1 if failing.   *
*                                                            *
* DESCRIPTION:                                               *
*     This function reads a data stream from the device and  *
*     compares it to the expected TDO.                       *
*                                                            *
*************************************************************/

int16_t ispVMRead(uint32_t a_uiDataSize)
{
	uint32_t uiIndex = 0;
	uint16_t usErrorCount = 0;
	uint8_t ucTDIByte = 0;
	uint8_t ucTDOByte = 0;
	uint8_t ucMaskByte = 0;
	uint8_t ucCurBit = 0;

	for (uiIndex = 0;uiIndex < a_uiDataSize; uiIndex++)
	{
		if (uiIndex % 8 == 0)
		{
			if ( g_usDataType & TDI_DATA ) {
				/*************************************************************
				*                                                            *
				* If the TDI_DATA flag is set, then grab the next byte from  *
				* the algo array and increment the TDI index.                *
				*                                                            *
				*************************************************************/
				ucTDIByte = GetByte( g_iTDIIndex++, 1 );
			}
			else
			{
				ucTDIByte = 0xFF;
			}
			if (g_usDataType & TDO_DATA)
			{
			/*************************************************************
			*                                                            *
			* If the TDO_DATA flag is set, then grab the next byte from  *
			* the algo array and increment the TDO index.                *
			*                                                            *
			*************************************************************/
				ucTDOByte = GetByte(g_iTDOIndex++, 1);
			}
			else
			{
			/*************************************************************
			*                                                            *
			* If TDO_DATA is not set, then DTDO_DATA must be set.  If    *
			* the compression counter exists, then the next TDO byte     *
			* must be 0xFF.  If it doesn't exist, then get next byte     *
			* from data file array.                                      *
			*                                                            *
			*************************************************************/
				if (g_ucCompressCounter)
				{
					g_ucCompressCounter--;
					ucTDOByte =(uint8_t) 0xFF;
				}
				else
				{
					ucTDOByte = GetByte(g_iMovingDataIndex++, 0);

					/*************************************************************
					*                                                            *
					* If the frame is compressed and the byte is 0xFF, then the  *
					* next couple bytes must be read to determine how many       *
					* repetitions of 0xFF are there.  That value will be stored  *
					* in the variable g_ucCompressCounter.                       *
					*                                                            *
					*************************************************************/
					if ((g_usDataType & COMPRESS_FRAME) &&(ucTDOByte ==(uint8_t) 0xFF))
					{
						g_ucCompressCounter = GetByte(g_iMovingDataIndex++, 0);
						g_ucCompressCounter--;
					}
				}
			}

			if (g_usDataType & MASK_DATA)
			{
				ucMaskByte = GetByte(g_iMASKIndex++, 1);
			}
			else
			{
				ucMaskByte =(uint8_t) 0xFF;
			}
		}

		ucCurBit = readPort();

		if ((((ucMaskByte << uiIndex % 8) & 0x80) ? 0x01 : 0x00))
		{
			if (ucCurBit !=(uint8_t)(((ucTDOByte << uiIndex % 8) & 0x80) ? 0x01 : 0x00))
			{
				usErrorCount++;
			}
		}

		/*************************************************************
		*                                                            *
		* Always shift 0x01 into TDI pin when reading.               *
		*                                                            *
		*************************************************************/

		writePort(pinTDI, (uint8_t) (((ucTDIByte << uiIndex % 8) & 0x80) ? 0x01 : 0x00));

		if (uiIndex < a_uiDataSize - 1)
		{
			sclock();
		}
	}

	if (usErrorCount > 0)
	{
		return -1;
	}

	return 0;
}

/*************************************************************
*                                                            *
* ISPVMSEND                                                  *
*                                                            *
* INPUT:                                                     *
*     a_uiDataSize: this argument is the size of the         *
*     command.                                               *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     This function sends a data stream to the device.       *
*                                                            *
*************************************************************/

void ispVMSend(uint32_t a_uiDataSize)
{
	uint32_t iIndex;
	uint8_t ucCurByte = 0;
	uint8_t ucBitState = 0;

	/*************************************************************
	*                                                            *
	* Begin processing the data to the device.                   *
	*                                                            *
	*************************************************************/

	for (iIndex = 0;iIndex < a_uiDataSize; iIndex++)
	{
		if (iIndex % 8 == 0)
		{
			if (g_usDataType & TDI_DATA)
			{
			/*************************************************************
			*                                                            *
			* If the TDI_DATA flag is set, then grab the next byte from  *
			* the algo array and increment the TDI index.                *
			*                                                            *
			*************************************************************/
				ucCurByte = GetByte(g_iTDIIndex++, 1);
			}
			else
			{
			/*************************************************************
			*                                                            *
			* If TDI_DATA flag is not set, then DTDI_DATA flag must have *
			* already been set.  If the compression counter exists, then *
			* the next TDI byte must be 0xFF.  If it doesn't exist, then *
			* get next byte from data file array.                        *
			*                                                            *
			*************************************************************/
				if (g_ucCompressCounter)
				{
					g_ucCompressCounter--;
					ucCurByte =(uint8_t) 0xFF;
				}
				else
				{
					ucCurByte = GetByte(g_iMovingDataIndex++, 0);

					/*************************************************************
					*                                                            *
					* If the frame is compressed and the byte is 0xFF, then the  *
					* next couple bytes must be read to determine how many       *
					* repetitions of 0xFF are there.  That value will be stored  *
					* in the variable g_ucCompressCounter.                       *
					*                                                            *
					*************************************************************/

					if ((g_usDataType & COMPRESS_FRAME) &&(ucCurByte ==(uint8_t) 0xFF))
					{
						g_ucCompressCounter = GetByte(g_iMovingDataIndex++, 0);
						g_ucCompressCounter--;
					}
				}
			}
		}

		ucBitState =(uint8_t)(((ucCurByte << iIndex % 8) & 0x80) ? 0x01 : 0x00);
		writePort(pinTDI, ucBitState);

		if (iIndex < a_uiDataSize - 1)
		{
			sclock();
		}
	}
}

/*************************************************************
*                                                            *
* ISPVMSTATEMACHINE                                          *
*                                                            *
* INPUT:                                                     *
*     a_cNextState: this is the opcode of the next JTAG      *
*     state.                                                 *
*                                                            *
* RETURN:                                                    *
*     This functions returns 0 when passing, and -1 when     *
*     failure occurs.                                        *
*                                                            *
* DESCRIPTION:                                               *
*     This function is called to move the device into        *
*     different JTAG states.                                 *
*                                                            *
*************************************************************/

void ispVMStateMachine(char a_cNextState)
{
	char cPathIndex, cStateIndex;
	if ((g_cCurrentJTAGState == DRPAUSE) &&(a_cNextState== DRPAUSE) && m_loopState)
	{
	}
	else if ((g_cCurrentJTAGState == a_cNextState) &&(g_cCurrentJTAGState != RESET))
	{
		return;
	}

	for (cStateIndex = 0;cStateIndex < 25; cStateIndex++)
	{
		if ((g_cCurrentJTAGState == iStates[cStateIndex].CurState) &&(a_cNextState == iStates[cStateIndex].NextState))
		{
			break;
		}
	}
	g_cCurrentJTAGState = a_cNextState;
	for (cPathIndex = 0;cPathIndex < iStates[cStateIndex].Pulses; cPathIndex++)
	{
		if ((iStates[cStateIndex].Pattern << cPathIndex) & 0x80)
		{
			writePort(pinTMS, (uint8_t) 0x01);
		}
		else
		{
			writePort(pinTMS, (uint8_t) 0x00);
		}
		sclock();
	}

	writePort(pinTDI, 0x00);
	writePort(pinTMS, 0x00);
}

/*************************************************************
*                                                            *
* ISPVMCLOCKS                                                *
*                                                            *
* INPUT:                                                     *
*     a_usClocks: number of clocks to apply.                 *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*    This procedure applies the specified number of pulses   *
*    to TCK.                                                 *
*                                                            *
*************************************************************/

void ispVMClocks(uint32_t a_uiClocks)
{
	for (; a_uiClocks > 0; a_uiClocks--)
	{
		sclock();
	}
}

/*************************************************************
*                                                            *
* ISPVMBYPASS                                                *
*                                                            *
* INPUT:                                                     *
*     a_siLength: this argument is the length of the         *
*     command.                                               *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     This function takes care of the HIR, HDR, TIR, and TDR *
*     for the purpose of putting the other devices into      *
*     bypass mode.                                           *
*                                                            *
*************************************************************/

void ispVMBypass(uint32_t a_uiLength)
{
/*************************************************************
*                                                            *
* Issue a_siLength number of 0x01 to the TDI pin to bypass.  *
*                                                            *
*************************************************************/

	for (; a_uiLength > 1; a_uiLength--)
	{
		writePort(pinTDI, (char) 0x01);
		sclock();
	}

	writePort(pinTDI, (char) 0x01);
}
/*************************************************************
*                                                            *
* ispVMLCOUNT                                                *
*                                                            *
* INPUT:                                                     *
*     a_usCountSize: The maximum number of loop required to  *
*     poling the status                                      *
*                                                            *
*                                                            *
* DESCRIPTION:                                               *
*     This function is set the maximum loop count            *
*                                                            *
*************************************************************/

void ispVMLCOUNT(uint16_t a_usCountSize)
{
	g_usLCOUNTSize = a_usCountSize;
}
/*************************************************************
*                                                            *
* ispVMLDELAY                                                *
*                                                            *
*                                                            *
* DESCRIPTION:                                               *
*     This function is set the delay state, number of TCK and*
*  the delay time for poling the status                      *
*                                                            *
*************************************************************/
void ispVMLDELAY()
{
	g_ucLDELAYState = IDLE;
	g_ucLDELAYDelay = 0;
	g_ucLDELAYTCK   = 0;
	while (1)
	{
		uint8_t bytedata = GetByte(g_iMovingAlgoIndex++, 1);
		switch (bytedata)
		{
		case STATE: /*step BSCAN state machine to specified state*/
			g_ucLDELAYState = GetByte(g_iMovingAlgoIndex++, 1);
			break;
		case WAIT:  /*opcode to wait for specified time in us or ms*/
			g_ucLDELAYDelay = (int16_t) ispVMDataSize();
			break;
		case TCK:   /*pulse TCK signal the specified time*/
			g_ucLDELAYTCK = (int16_t) ispVMDataSize();
			break;
		case ENDSTATE:
			return;
		}
	}
}




int32_t aaa;

/*************************************************************
*                                                            *
* EXTERNAL FUNCTIONS                                         *
*                                                            *
*************************************************************/

extern uint8_t GetByte (int32_t a_iCurrentIndex, char a_cAlgo );
extern int16_t ispProcessVME (void);
extern void EnableHardware (void);
extern void DisableHardware (void);

/***************************************************************
*
* Supported VME versions.
*
***************************************************************/
char *g_szSupportedVersions[] = { (char *) "_SVME1.1", (char *) "_SVME1.2", (char *) "_SVME1.3", 0 };




/*************************************************************
*                                                            *
* ISPVMENTRYPOINT                                            *
*                                                            *
* INPUT:                                                     *
*     a_pszAlgoFile: this is the name of the algorithm file. *
*                                                            *
*     a_pszDataFile: this is the name of the data file.      *
*     Note that this argument may be empty if the algorithm  *
*     does not require a data file.                          *
*                                                            *
* RETURN:                                                    *
*     The return value will be a negative number if an error *
*     occurred, or 0 if everything was successful            *
*                                                            *
* DESCRIPTION:                                               *
*     This function opens the file pointers to the algo and  *
*     data file.  It intializes global variables to their    *
*     default values and enters the processor.               *
*                                                            *
*************************************************************/

int32_t FPGA_Programming()
{
	char szFileVersion[ 9 ] = { 0 };
	int16_t siRetCode     = 0;
	int16_t iIndex        = 0;
	int16_t cVersionIndex = 0;

	/*************************************************************
	*                                                            *
	* VARIABLES INITIALIZATION                                   *
	*                                                            *
	*************************************************************/

	g_usDataType       = 0;
	g_iMovingAlgoIndex = 0;
	g_iMovingDataIndex = 0;


	if ( GetByte( g_iMovingDataIndex++, 0 ) ) {
		g_usDataType |= COMPRESS;
	}

	/***************************************************************
	*
	* Read and store the version of the VME file.
	*
	***************************************************************/

	for ( iIndex = 0; iIndex < 8; iIndex++ ) {
		szFileVersion[ iIndex ] = GetByte( g_iMovingAlgoIndex++, 1 );
	}

	/***************************************************************
	*
	* Compare the VME file version against the supported version.
	*
	***************************************************************/

	for ( cVersionIndex = 0; g_szSupportedVersions[ cVersionIndex ] != 0; cVersionIndex++ ) {
		for ( iIndex = 0; iIndex < 8; iIndex++ ) {
			if ( szFileVersion[ iIndex ] != g_szSupportedVersions[ cVersionIndex ][ iIndex ] ) {
				siRetCode = ERR_WRONG_VERSION;
				break;
			}
			siRetCode = 0;
		}

		if ( siRetCode == 0 ) {

			/***************************************************************
			*
			* Found matching version, break.
			*
			***************************************************************/

			break;
		}
	}

	if ( siRetCode < 0 ) {

		/***************************************************************
		*
		* VME file version failed to match the supported versions.
		*
		***************************************************************/

		return ERR_WRONG_VERSION;
	}


	/*************************************************************
	*                                                            *
	* Start the hardware.                                        *
	*                                                            *
	*************************************************************/

    EnableHardware();

	/*************************************************************
	*                                                            *
	* Begin processing algorithm and data file.                  *
	*                                                            *
	*************************************************************/

	siRetCode = ispProcessVME();

	/*************************************************************
	*                                                            *
	* Stop the hardware.                                         *
	*                                                            *
	*************************************************************/

    DisableHardware();

	/*************************************************************
	*                                                            *
	* Return the return code.                                    *
	*                                                            *
	*************************************************************/

	return ( siRetCode );
}








/*************************************************************
*                                                            *
* PUBLIC CALLS                                               *
*                                                            *
*************************************************************/


int32_t se3_FPGA_Init (const uint32_t rcc_mcodiv)
{
	/* Set MCO1 output = PLLCLK with prescaler from 1 to 5 */
	__HAL_RCC_MCO1_CONFIG(RCC_MCO1SOURCE_PLLCLK, rcc_mcodiv);

	/* Configure interrupt line */

	/* Set variables used */
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Tell system that you will use PA9 for EXTI_Line9 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource9);

	/* PA9 is connected to EXTI_Line9 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line9;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	/* PA9 is connected to EXTI_Line9, which has EXTI9_5_IRQn vector */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);


	/* Program the FPGA according to the given bitstream */
	int32_t retcode = FPGA_Programming();

	return retcode;

}



void se3_FPGA_SetMux (uint8_t mux)
{
		//	CPU_FPGA_BUS_NE1	PD7	--  MUX0 -- LSB
		//	CPU_FPGA_BUS_NE2	PG9	--  MUX1
		//	CPU_FPGA_CLK		PA8	--  MUX2
		//	CPU_FPGA_INTN		PA9	--  MUX3
		//	CPU_FPGA_RST		PG2	--  MUX4 -- MSB


		if(mux & 0x01)
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

		if(mux & 0x02)
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, GPIO_PIN_RESET);

		if(mux & 0x04)
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

		if(mux & 0x08)
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

		if(mux & 0x10)
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);
}




void se3_FPGA_FpgaCpuGPIO (uint8_t gpioNum, GPIO_PinState set)
{

	switch(gpioNum)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, set);
			break;

		case 1:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, set);
			break;

		case 2:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, set);
			break;

		case 3:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, set);
			break;

		case 4:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, set);
			break;

		case 5:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, set);
			break;

		case 6:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, set);
			break;

		case 7:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, set);
			break;

		case 8:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, set);
			break;

		case 9:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, set);
			break;

		case 10:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, set);
			break;

		case 11:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, set);
			break;

		case 12:
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, set);
			break;

		case 13:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, set);
			break;

		case 14:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, set);
			break;

		case 15:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, set);
			break;

		case 16:
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, set);
			break;

		case 17:
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, set);
			break;

		case 18:
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, set);
			break;

		case 19:
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, set);
			break;

		case 20:
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, set);
			break;

		case 21:
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, set);
			break;

		case 22:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, set);
			break;

		case 23:
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, set);
			break;


		default:
			break;
	}
}


void se3_FPGA_Reset () {

	HAL_GPIO_WritePin(GPIOG, FPGA_RST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, FPGA_RST_Pin, GPIO_PIN_RESET);

}





uint8_t se3_FPGA_Read (uint8_t address, uint16_t *dataPtr)
{
	HAL_StatusTypeDef status;
	uint32_t addr = (FPGA_SRAM_BASE_ADDR + 2*address);
	status = HAL_SRAM_Read_16b(&SRAM_READ, (uint32_t*)addr, dataPtr, 1);
	return status == HAL_OK ? 0 : 1;
}




uint8_t se3_FPGA_Write (uint8_t address, uint16_t *dataPtr)
{
	HAL_StatusTypeDef status;
	uint32_t addr = (FPGA_SRAM_BASE_ADDR + 2*address);
	status = HAL_SRAM_Write_16b(&SRAM_WRITE, (uint32_t*)addr, dataPtr, 1);
	return status == HAL_OK ? 0 : 1;
}

/*************************************************************
*                                                            *
* Define the interrupt handler for the FPGA (left to user).  *
*                                                            *
*************************************************************/

__weak void EXTI9_5_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
        /* Do your stuff when PA9 is changed */

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}
