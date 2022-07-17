/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STOPCAP_KEY_Pin GPIO_PIN_4
#define STOPCAP_KEY_GPIO_Port GPIOE
#define DATA0_Pin GPIO_PIN_0
#define DATA0_GPIO_Port GPIOC
#define DATA1_Pin GPIO_PIN_1
#define DATA1_GPIO_Port GPIOC
#define DATA2_Pin GPIO_PIN_2
#define DATA2_GPIO_Port GPIOC
#define DATA3_Pin GPIO_PIN_3
#define DATA3_GPIO_Port GPIOC
#define DATA4_Pin GPIO_PIN_4
#define DATA4_GPIO_Port GPIOC
#define DATA5_Pin GPIO_PIN_5
#define DATA5_GPIO_Port GPIOC
#define DATA6_Pin GPIO_PIN_6
#define DATA6_GPIO_Port GPIOC
#define DATA7_Pin GPIO_PIN_7
#define DATA7_GPIO_Port GPIOC
#define SCCB_SCL_Pin GPIO_PIN_3
#define SCCB_SCL_GPIO_Port GPIOD
#define VSYNC_Pin GPIO_PIN_6
#define VSYNC_GPIO_Port GPIOD
#define SCCB_SDA_Pin GPIO_PIN_13
#define SCCB_SDA_GPIO_Port GPIOG
#define HREF_Pin GPIO_PIN_14
#define HREF_GPIO_Port GPIOG
#define RST_Pin GPIO_PIN_15
#define RST_GPIO_Port GPIOG
#define PWDN_Pin GPIO_PIN_3
#define PWDN_GPIO_Port GPIOB
#define PCLK_Pin GPIO_PIN_4
#define PCLK_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_5
#define LED0_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
