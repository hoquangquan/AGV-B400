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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "math.h"

#include "Flash.h"
#include "Modbus.h"
#include "Lidar_ME4341.h"
#include "charge.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define Control_ID	0x01
#define Wifi_ID			0x02
#define PLC_ID			0x03
#define Battery_ID	0x04

#define Start_Control_Write		0x07D0
#define Start_Control_Read 		0x03E8
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
extern UART_HandleTypeDef huart1;
extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef TxHeader;
extern CAN_RxHeaderTypeDef RxHeader;
extern uint8_t Rx_CAN[5];

extern  ADC_HandleTypeDef hadc3;

extern modbusHandler_t Control_Port;
extern uint16_t usSReg_Control_Write[19];
extern uint16_t usSReg_Control_Read[49];

extern modbusHandler_t System_Port;
extern uint16_t usSReg_System[44];

extern modbusHandler_t PLC_Port;
extern uint16_t usSReg_PLC[15];

extern modbusHandler_t Charge_Port;
extern uint16_t usSReg_Charge[5];

extern modbusHandler_t Battery_Port;
extern uint16_t usSReg_Battery[41];

extern bool AGV_Enable;
extern uint8_t AGV_Running;
extern uint8_t AGV_Run_save;
extern uint16_t Value_Point;

extern bool tool_status_up;
extern bool tool_bool;

extern uint16_t Value_RFID;

extern uint8_t Emc_status;
extern uint8_t Bamber_status;
extern uint8_t Taget_status;
extern uint8_t ID_Can;
extern uint8_t counter_can;
extern uint8_t counter_bat;
extern uint8_t counter_sys;

extern uint8_t charge_recieve[6];
extern uint8_t flag_process;

extern uint32_t time_speak;

extern uint8_t Charge_Enb;
extern bool Charge_start;
extern uint32_t Volt_Value[1];
extern uint32_t Charge_counter;

extern uint32_t Back_counter;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Y8_Pin GPIO_PIN_2
#define Y8_GPIO_Port GPIOE
#define Y9_Pin GPIO_PIN_3
#define Y9_GPIO_Port GPIOE
#define DK_Sac_Pin GPIO_PIN_8
#define DK_Sac_GPIO_Port GPIOI
#define Speak5_Pin GPIO_PIN_13
#define Speak5_GPIO_Port GPIOC
#define Speak4_Pin GPIO_PIN_9
#define Speak4_GPIO_Port GPIOI
#define Speak1_Pin GPIO_PIN_10
#define Speak1_GPIO_Port GPIOI
#define Speak3_Pin GPIO_PIN_11
#define Speak3_GPIO_Port GPIOI
#define Speak2_Pin GPIO_PIN_0
#define Speak2_GPIO_Port GPIOF
#define Led_Run_Pin GPIO_PIN_9
#define Led_Run_GPIO_Port GPIOF
#define Led_Err_Pin GPIO_PIN_10
#define Led_Err_GPIO_Port GPIOF
#define Control_TX_Pin GPIO_PIN_8
#define Control_TX_GPIO_Port GPIOD
#define Control_RX_Pin GPIO_PIN_9
#define Control_RX_GPIO_Port GPIOD
#define Control_DR_Pin GPIO_PIN_10
#define Control_DR_GPIO_Port GPIOD
#define Emc_Pin GPIO_PIN_11
#define Emc_GPIO_Port GPIOA
#define Emc_EXTI_IRQn EXTI15_10_IRQn
#define Start_Pin GPIO_PIN_12
#define Start_GPIO_Port GPIOA
#define Start_EXTI_IRQn EXTI15_10_IRQn
#define AGV_Point_Pin GPIO_PIN_13
#define AGV_Point_GPIO_Port GPIOH
#define Stop_Pin GPIO_PIN_14
#define Stop_GPIO_Port GPIOH
#define Stop_EXTI_IRQn EXTI15_10_IRQn
#define Reset_Pin GPIO_PIN_0
#define Reset_GPIO_Port GPIOI
#define Reset_EXTI_IRQn EXTI0_IRQn
#define PLC_DR_Pin GPIO_PIN_3
#define PLC_DR_GPIO_Port GPIOI
#define PLC_TX_Pin GPIO_PIN_10
#define PLC_TX_GPIO_Port GPIOC
#define PLC_RX_Pin GPIO_PIN_11
#define PLC_RX_GPIO_Port GPIOC
#define Tx_Battery_Pin GPIO_PIN_12
#define Tx_Battery_GPIO_Port GPIOC
#define Batterry_DR_Pin GPIO_PIN_0
#define Batterry_DR_GPIO_Port GPIOD
#define Rx_Battery_Pin GPIO_PIN_2
#define Rx_Battery_GPIO_Port GPIOD
#define Tool_IN2_Pin GPIO_PIN_3
#define Tool_IN2_GPIO_Port GPIOD
#define Tool_IN2_EXTI_IRQn EXTI3_IRQn
#define System_TX_Pin GPIO_PIN_5
#define System_TX_GPIO_Port GPIOD
#define Sytem_RX_Pin GPIO_PIN_6
#define Sytem_RX_GPIO_Port GPIOD
#define System_DR_Pin GPIO_PIN_7
#define System_DR_GPIO_Port GPIOD
#define Tool_IN1_Pin GPIO_PIN_9
#define Tool_IN1_GPIO_Port GPIOG
#define SS1_Pin GPIO_PIN_11
#define SS1_GPIO_Port GPIOG
#define Bumper_Top_Pin GPIO_PIN_13
#define Bumper_Top_GPIO_Port GPIOG
#define Bumper_Top_EXTI_IRQn EXTI15_10_IRQn
#define Bumper_Button_Pin GPIO_PIN_15
#define Bumper_Button_GPIO_Port GPIOG
#define Bumper_Button_EXTI_IRQn EXTI15_10_IRQn
#define Led_Start_Pin GPIO_PIN_7
#define Led_Start_GPIO_Port GPIOB
#define Led_Reset_Pin GPIO_PIN_0
#define Led_Reset_GPIO_Port GPIOE
#define Led_Stop_Pin GPIO_PIN_1
#define Led_Stop_GPIO_Port GPIOE
#define Tool_OUT2_Pin GPIO_PIN_5
#define Tool_OUT2_GPIO_Port GPIOI
#define Tool_OUT1_Pin GPIO_PIN_6
#define Tool_OUT1_GPIO_Port GPIOI
#define Y7_Pin GPIO_PIN_7
#define Y7_GPIO_Port GPIOI
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
