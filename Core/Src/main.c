/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ov2640.h"
#include "sccb.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t *imgbuf;
const uint16_t jpeg_img_size_tbl[][2] =
	{
		176, 144,	// QCIF
		160, 120,	// QQVGA
		352, 288,	// CIF
		320, 240,	// QVGA
		640, 480,	// VGA
		800, 600,	// SVGA
		1024, 768,	// XGA
		1280, 1024, // SXGA
		1600, 1200, // UXGA
};
#define LEN 13 * 1024
// FATFS fs;
// FIL fil;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*const uint8_t Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const uint16_t monDays[12] = {0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

uint32_t time2Stamp(RTC_DateTypeDef date, RTC_TimeTypeDef time)
{
	uint32_t result;
	uint16_t Year=date.Year+2000;
	result = (Year - 1970) * 365 * 24 * 3600 + (monDays[date.Month-1] + date.Date - 1) * 24 * 3600 + (time.Hours-8) * 3600 + time.Minutes * 60 + time.Seconds;
	result += (date.Month>2 && (Year % 4 == 0) && (Year % 100 != 0 || Year % 400 == 0))*24*3600;
	Year -= 1970;
	result += (Year/4 - Year/100 + Year/400)*24 * 3600;
	return result;
}*/

/*void InitFatFs(void) {
	int retSD = f_mount(&fs, "0:", 1);
	if(retSD)  {
		printf("mount error: %d \r\n",retSD);
		Error_Handler();
	} else printf("mount success\r\n!\r\n");

}

void WriteFsFile(char *fn, const uint8_t *bytes, int len) {
	uint32_t retlen = 0;
	//////// Escribir
	int retSD = f_open(&fil, fn, FA_CREATE_NEW|FA_WRITE);
	if(retSD == FR_OK) {
		printf("open file success\r\n");
		retSD = f_write(&fil, bytes, len, &retlen);
		if(retSD) printf("write file error code: %d\r\n", retSD);
		else printf("write file success\r\n");
		HAL_Delay(80);
		retSD = f_close(&fil);
		if(retSD) printf("cannot close file stream!\r\n");
	} else {
		printf("open file error code: %d\r\n", retSD);
	}
}*/

void SendJPEG(uint8_t *buf, int siz)
{
	// RTC_DateTypeDef fecha;
	// RTC_TimeTypeDef tiempo;
	// char fn[128];
	// HAL_UART_Transmit_DMA(&huart3, buf, siz);
	HAL_UART_Transmit(&huart3, buf, siz, 5000);
	// HAL_RTC_GetDate(&hrtc, &fecha, RTC_FORMAT_BCD);
	// HAL_RTC_GetTime(&hrtc, &tiempo, RTC_FORMAT_BCD);
	// sprintf(fn, "0:/captura_%d.jpg", time2Stamp(fecha, tiempo));
	// WriteFsFile(fn, buf, siz);
	printf("Transimitted %d bytes\r\n", siz);
}

void KeepCapture(void)
{
	int i = 0, j = 0, ilen = 0;
	if (HAL_GPIO_ReadPin(STOPCAP_KEY_GPIO_Port, STOPCAP_KEY_Pin) == RESET)
	{
		printf("Now stop capture and halt the system!\r\n");
		// f_mount(NULL, "0:", 1);
		Error_Handler();
	}
	// skip initial frames that are not ready
	for (i = 0; i < 3; i++)
	{
		while (OV2640_VSYNC == 1)
			;
		while (OV2640_VSYNC == 0)
			;
	}
	while (OV2640_VSYNC)
	{
		while (OV2640_HREF)
		{
			while (!OV2640_PCLK)
				;
			imgbuf[ilen] = OV2640_DATA;
			while (OV2640_PCLK)
				;
			ilen++;
		}
	}
	for (i = 0; i < ilen; i++)
	{
		if (imgbuf[i] == 0xff && imgbuf[i + 1] == 0xd8)
			break;
	}
	if (i == ilen)
		printf("No valid JPEG header found!\r\n");
	else
	{
		printf("Found valid JPEG header!\r\n");
		for (j = i; j < ilen; j++)
		{
			if (imgbuf[j] == 0xd9 && imgbuf[j - 1] == 0xff)
			{
				printf("Found valid JPEG EOF!\r\n");
				SendJPEG(imgbuf + i, j - i + 1);
				break;
			}
		}
	}
}

/*void TestSDCard(void) {
	HAL_SD_CardCIDTypeDef SDCard_CID;
	HAL_SD_CardInfoTypeDef SDCard_INFO;
	uint64_t CardCap;
	HAL_StatusTypeDef sta;
	sta = HAL_SD_GetCardCID(&hsd,&SDCard_CID);
	if(sta != HAL_OK) printf("Cannot get CID: %d\r\n", sta);
	sta = HAL_SD_GetCardInfo(&hsd, &SDCard_INFO);
	if(sta != HAL_OK) printf("Cannot get SDInfo: %d\r\n", sta);
	switch(SDCard_INFO.CardType)
	{
		case CARD_SDSC:
			if(SDCard_INFO.CardVersion == CARD_V1_X){
				printf("Card Type: SDSC V1\r\n");
			}
			else if(SDCard_INFO.CardVersion == CARD_V2_X){
				printf("Card Type: SDSC V2\r\n");
			}
			break;
		case CARD_SDHC_SDXC:
			printf("Card Type: SDHC\r\n");
			break;
	}

	CardCap = (uint64_t)(SDCard_INFO.LogBlockNbr)*(uint64_t)(SDCard_INFO.LogBlockSize)/1024/1024;
	printf("Card ManufacturerID:%d\r\n",SDCard_CID.ManufacturerID);
	printf("Card RCA:%d\r\n",SDCard_INFO.RelCardAdd);
	printf("LogBlockNbr:%d\r\n",SDCard_INFO.LogBlockNbr);
	printf("LogBlockSize:%d\r\n",SDCard_INFO.LogBlockSize);
	printf("Card Capacity:%d MB\r\n",(uint32_t)CardCap);
	printf("Card BlockSize:%d\r\n",SDCard_INFO.BlockSize);
}*/
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
	MX_TIM6_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */
	// Disable SWD And JTAG
	/*__HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_AFIO_REMAP_SWJ_DISABLE();
	__HAL_AFIO_REMAP_SWJ_NOJTAG();*/
	imgbuf = malloc(LEN);
	if (imgbuf == NULL)
	{
		printf("ALLOC FAILED!\r\n");
		Error_Handler();
	}
	while (OV2640_Init())
	{
	}
	HAL_Delay(1000);
	SCCB_WR_Reg(0XFF, 0X00);
	SCCB_WR_Reg(0XD3, 15); // PCLK division
	SCCB_WR_Reg(0XFF, 0X01);
	SCCB_WR_Reg(0X11, 3); // CLK division
	OV2640_JPEG_Mode();
	OV2640_OutSize_Set(jpeg_img_size_tbl[3][0], jpeg_img_size_tbl[3][1]); // 320*240 try
	printf("Registered!\r\n");
	// TestSDCard();
	// InitFatFs();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		KeepCapture();
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
