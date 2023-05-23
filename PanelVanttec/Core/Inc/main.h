/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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
#define EXTRA_4_Pin GPIO_PIN_3
#define EXTRA_4_GPIO_Port GPIOC
#define L1D_Pin GPIO_PIN_0
#define L1D_GPIO_Port GPIOA
#define L2D_Pin GPIO_PIN_1
#define L2D_GPIO_Port GPIOA
#define L3D_Pin GPIO_PIN_2
#define L3D_GPIO_Port GPIOA
#define L4D_Pin GPIO_PIN_3
#define L4D_GPIO_Port GPIOA
#define L5D_Pin GPIO_PIN_4
#define L5D_GPIO_Port GPIOA
#define EXTRA_5_Pin GPIO_PIN_5
#define EXTRA_5_GPIO_Port GPIOA
#define DEBUG_6_Pin GPIO_PIN_4
#define DEBUG_6_GPIO_Port GPIOC
#define DEBUG_5_Pin GPIO_PIN_5
#define DEBUG_5_GPIO_Port GPIOC
#define DEBUG_4_Pin GPIO_PIN_0
#define DEBUG_4_GPIO_Port GPIOB
#define DEBUG_3_Pin GPIO_PIN_1
#define DEBUG_3_GPIO_Port GPIOB
#define DEBUG_2_Pin GPIO_PIN_2
#define DEBUG_2_GPIO_Port GPIOB
#define STMTB1_Pin GPIO_PIN_15
#define STMTB1_GPIO_Port GPIOB
#define STMTB2_Pin GPIO_PIN_6
#define STMTB2_GPIO_Port GPIOC
#define STMTB3_Pin GPIO_PIN_7
#define STMTB3_GPIO_Port GPIOC
#define STMTB4_Pin GPIO_PIN_8
#define STMTB4_GPIO_Port GPIOC
#define STMTB5_Pin GPIO_PIN_9
#define STMTB5_GPIO_Port GPIOC
#define DEBUG_1_Pin GPIO_PIN_8
#define DEBUG_1_GPIO_Port GPIOA
#define EXTRA_1_Pin GPIO_PIN_2
#define EXTRA_1_GPIO_Port GPIOD
#define EXTRA_2_Pin GPIO_PIN_4
#define EXTRA_2_GPIO_Port GPIOB
#define EXTRA_3_Pin GPIO_PIN_5
#define EXTRA_3_GPIO_Port GPIOB
#define EXTRA_8_Pin GPIO_PIN_8
#define EXTRA_8_GPIO_Port GPIOB
#define EXTRA_9_Pin GPIO_PIN_9
#define EXTRA_9_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
