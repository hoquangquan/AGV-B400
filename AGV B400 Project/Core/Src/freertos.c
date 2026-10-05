/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "ESA_Control.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
modbus_t Control[7];

uint16_t time_delay = 100;
/* Definitions myTaskDefault */
osThreadId_t myTaskDefaultHandle;
const osThreadAttr_t myTaskDefault_attributes = {
  .name = "myTaskDefaultHandle",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for myTaskMaster */
osThreadId_t myTaskMasterHandle;
const osThreadAttr_t myTaskMaster_attributes = {
  .name = "myTaskMaster",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskmyTaskSlave */
osThreadId_t TaskmyTaskSlaveHandle;
const osThreadAttr_t TaskmyTaskSlave_attributes = {
  .name = "TaskmyTaskSlave",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void StartTaskDefault(void *argument);
void StartTaskMaster(void *argument);
void StartTasSlave(void *argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
	
	/* creation of myTaskDefault */
  myTaskDefaultHandle = osThreadNew(StartTaskDefault, NULL, &myTaskDefault_attributes);
	
  /* creation of myTaskMaster */
  myTaskMasterHandle = osThreadNew(StartTaskMaster, NULL, &myTaskMaster_attributes);

  /* creation of TaskmyTaskSlave */
  TaskmyTaskSlaveHandle = osThreadNew(StartTasSlave, NULL, &TaskmyTaskSlave_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskDefault */
/**
* @brief Function implementing the TaskmyTaskDefault thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTasSlave */
void StartTaskDefault(void *argument)
{
  /* USER CODE BEGIN StartTaskDefault */
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_TogglePin(Led_Run_GPIO_Port, Led_Run_Pin);
		HAL_GPIO_TogglePin(Led_Err_GPIO_Port, Led_Err_Pin);
		time_delay --;
		if(time_delay < 10 || time_delay > 200) time_delay = 200; 
		osDelay(time_delay);
  }
  /* USER CODE END StartTaskDefault */
}

/* USER CODE BEGIN Header_StartTaskMaster */
/**
* @brief Function implementing the myTaskMaster thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskMaster */
void StartTaskMaster(void *argument)
{
  /* USER CODE BEGIN StartTaskMaster */
	
	/*Read Control*/
	Control[0].u8id 			 = Control_ID; // slave address
	Control[0].u8fct 		 	 = 0x04; // function code (this one is registers read)
	Control[0].u16RegAdd   = Start_Control_Read; // start address in slave
	Control[0].u16CoilsNo  = 49; // number of elements (coils or registers) to read
	Control[0].u16reg 		 = usSReg_Control_Read; // pointer to a memory array
	
	/*Write Control*/
	Control[1].u8id 			 = Control_ID; // slave address
	Control[1].u8fct 		 	 = 0x10; // function code (this one is registers read)
	Control[1].u16RegAdd   = Start_Control_Write; // start address in slave
	Control[1].u16CoilsNo  = 19; // number of elements (coils or registers) to read
	Control[1].u16reg 		 = usSReg_Control_Write; // pointer to a memory array
	
	Control[2].u8id 			 = 1; // slave address
	Control[2].u8fct 		 	 = 0x03; // function code (this one is registers read)
	Control[2].u16RegAdd   = 0; // start address in slave
	Control[2].u16CoilsNo  = 41; // number of elements (coils or registers) to read
	Control[2].u16reg 		 = usSReg_Battery; // pointer to a memory array
	
    /* Infinite loop */
    for(;;)
    {
    	 ModbusQuery(&Control_Port, Control[0]); // make a query
			 ulTaskNotifyTake(pdTRUE, 1); // block until query finishes or timeout
    	 osDelay(1);
    	 ModbusQuery(&Control_Port, Control[1]); // make a query
    	 ulTaskNotifyTake(pdTRUE, 1); // block until query finishes or timeout
    	 osDelay(1);
			 ModbusQuery(&Battery_Port, Control[2]); // make a query
    	 ulTaskNotifyTake(pdTRUE, 1); // block until query finishes or timeout
    	 osDelay(1);
    	 
    }
  /* USER CODE END StartTaskMaster */
}

/* USER CODE BEGIN Header_StartTasSlave */
/**
* @brief Function implementing the TaskmyTaskSlave thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTasSlave */
void StartTasSlave(void *argument)
{
  /* USER CODE BEGIN StartTasSlave */
  /* Infinite loop */
  for(;;)
  {
		xSemaphoreTake(System_Port.ModBusSphrHandle, portMAX_DELAY);
		xSemaphoreGive(System_Port.ModBusSphrHandle);
		osDelay(1);
		xSemaphoreTake(PLC_Port.ModBusSphrHandle, portMAX_DELAY);
		xSemaphoreGive(PLC_Port.ModBusSphrHandle);
		osDelay(1);
		xSemaphoreTake(Charge_Port.ModBusSphrHandle, portMAX_DELAY);
		xSemaphoreGive(Charge_Port.ModBusSphrHandle);
		osDelay(1);
  }
  /* USER CODE END StartTasSlave */
}

/* Private application code --------------------------------------------------*/
/* USER CODE END Application */

