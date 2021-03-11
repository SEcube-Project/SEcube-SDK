/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @brief          : Memory management layer
  ******************************************************************************
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
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
#include <se3_sdio.h>
#include "usbd_storage_if.h"
/* USER CODE BEGIN INCLUDE */
#include "se3_communication_core.h"
/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_STORAGE 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_TYPES */
/* USER CODE END PRIVATE_TYPES */ 
/**
  * @}
  */ 

/** @defgroup USBD_STORAGE_Private_Defines
  * @{
  */ 

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */
  
/**
  * @}
  */ 

/** @defgroup USBD_STORAGE_Private_Macros
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_MACRO */
/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */ 

/** @defgroup USBD_STORAGE_IF_Private_Variables
  * @{
  */
/* USB handler declaration */
/* Handle for USB Full Speed IP */

/* Handle for USB High Speed IP */
  USBD_HandleTypeDef  *hUsbDevice_1;
/* USER CODE BEGIN INQUIRY_DATA_HS */ 
/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata_HS[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'E', 'C', 'u', 'b', 'e', ' ', ' ', /* Manufacturer : 8 bytes */
  'S', 'E', 'C', 'u', 'b', 'e', ' ', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1',                     /* Version      : 4 Bytes */
}; 
/* USER CODE END INQUIRY_DATA_HS */ 
/* USER CODE BEGIN PRIVATE_VARIABLES */
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */ 
 
/** @defgroup USBD_STORAGE_IF_Exported_Variables
  * @{
  */ 
//  extern USBD_HandleTypeDef hUsbDeviceHS;
/* USER CODE BEGIN EXPORTED_VARIABLES */
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */ 
  
/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @{
  */

static int8_t STORAGE_Init_HS (uint8_t lun);
static int8_t STORAGE_GetCapacity_HS (uint8_t lun, 
                           uint32_t *block_num, 
                           uint16_t *block_size);
static int8_t  STORAGE_IsReady_HS (uint8_t lun);
static int8_t  STORAGE_IsWriteProtected_HS (uint8_t lun);
static int8_t STORAGE_Read_HS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);
static int8_t STORAGE_Write_HS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_HS (void);
/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */ 
  

USBD_StorageTypeDef USBD_Storage_Interface_fops_HS =
{
  STORAGE_Init_HS,
  STORAGE_GetCapacity_HS,
  STORAGE_IsReady_HS,
  STORAGE_IsWriteProtected_HS,
  STORAGE_Read_HS,
  STORAGE_Write_HS,
  STORAGE_GetMaxLun_HS,
  (int8_t *)STORAGE_Inquirydata_HS,
};

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : STORAGE_Init_HS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Init_HS (uint8_t lun)
{
  /* USER CODE BEGIN 9 */ 
  return (USBD_OK);
  /* USER CODE END 9 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_GetCapacity_HS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_GetCapacity_HS (uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 10 */   
	if (!secube_sdio_capacity(block_num, block_size))
		return USBD_FAIL;

	return USBD_OK;
  /* USER CODE END 10 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_IsReady_HS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsReady_HS (uint8_t lun)
{
  /* USER CODE BEGIN 11 */ 
	if (!secube_sdio_isready())
		return USBD_FAIL;

	return USBD_OK;
  /* USER CODE END 11 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_IsWriteProtected_HS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t  STORAGE_IsWriteProtected_HS (uint8_t lun)
{
  /* USER CODE BEGIN 12 */ 
  return (USBD_OK);
  /* USER CODE END 12 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_Read_HS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Read_HS (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,                       
                        uint16_t blk_len)
{
  /* USER CODE BEGIN 13 */ 
	int32_t r = se3_proto_send(lun, buf, blk_addr, blk_len);
	if(r==SE3_PROTO_BUSY){
		return USBD_BUSY;
	}
	else if(r==SE3_PROTO_OK){
		return USBD_OK;
	}
	return USBD_FAIL;


	//if (_SEcube_device_send_data(lun, buf, blk_addr, blk_len) != PROTOCOL_OK)
	//	return USBD_FAIL;

	//return USBD_OK;
  /* USER CODE END 13 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_Write_HS
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_Write_HS (uint8_t lun, 
                         uint8_t *buf, 
                         uint32_t blk_addr,
                         uint16_t blk_len)
{
  /* USER CODE BEGIN 14 */ 
	if(SE3_PROTO_OK != se3_proto_recv(lun, buf, blk_addr, blk_len))
		return USBD_FAIL;
	return USBD_OK;
	//if (_SEcube_device_receive_data(lun, buf, blk_addr, blk_len) != PROTOCOL_OK)
	//	return USBD_FAIL;

	//return USBD_OK;
  /* USER CODE END 14 */ 
}

/*******************************************************************************
* Function Name  : STORAGE_GetMaxLun_HS
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int8_t STORAGE_GetMaxLun_HS (void)
{
  /* USER CODE BEGIN 15 */ 
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 15 */   
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */ 

/**
  * @}
  */  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
