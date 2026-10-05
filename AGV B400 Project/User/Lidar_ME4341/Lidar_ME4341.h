#ifndef __LIDAR_ME4341_H
#define __LIDAR_ME4341_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
	
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Private void */

/*Control ME-4341 Init*/
void ME_4341_Init(void);

/*CAN-HS Write data*/
void Channel_designation(uint8_t ID, uint8_t Channel_number);
void Smart_channel(uint8_t ID, uint16_t angle_value, uint16_t speed_value, uint8_t group_number);

/* USER CODE END Private void */

#ifdef __cplusplus
}
#endif

#endif /* __ME4341_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
