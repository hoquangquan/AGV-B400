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
#ifndef __SPEAK_H
#define __SPEAK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define AGV_Stop				0x00
#define AGV_Start				0x01
#define AGV_Emc		 			0x02
#define AGV_Bamber			0x03
#define AGV_Barrier			0x04
#define AGV_Line				0x05
#define AGV_Wait				0x06
#define SYS_Init				0x07
#define SYS_Done				0x08

/* USER CODE END Includes */
void Speak_out(uint8_t status);


#ifdef __cplusplus
}
#endif

#endif /* __SPEAK_H */
