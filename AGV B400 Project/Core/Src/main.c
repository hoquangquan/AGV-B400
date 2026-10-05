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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ESA_Control.h"
bool stop = false,lock = false;
extern bool count_tool_en;
extern uint8_t count_tool;

extern float  Speed_Set;

uint8_t cnt_speak = 0,song = 0;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
modbusHandler_t Control_Port;
uint16_t usSReg_Control_Write[19];
uint16_t usSReg_Control_Read[49];

modbusHandler_t Battery_Port;
uint16_t usSReg_Battery[41];

modbusHandler_t System_Port;
uint16_t usSReg_System[44];

modbusHandler_t PLC_Port;
uint16_t usSReg_PLC[15];

modbusHandler_t Charge_Port;
uint16_t usSReg_Charge[5];


uint8_t AGV_Running;
uint8_t AGV_Run_save;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

CAN_HandleTypeDef hcan1;

TIM_HandleTypeDef htim9;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_uart5_rx;
DMA_HandleTypeDef hdma_uart5_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

/* Definitions for AGV_RunTask */
osThreadId_t AGV_RunTaskHandle;
const osThreadAttr_t AGV_RunTask_attributes = {
  .name = "AGV_RunTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for AGV_Read */
osThreadId_t AGV_ReadHandle;
const osThreadAttr_t AGV_Read_attributes = {
  .name = "AGV_Read",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for AGV_Control */
osThreadId_t AGV_ControlHandle;
const osThreadAttr_t AGV_Control_attributes = {
  .name = "AGV_Control",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for AGV_Emc */
osThreadId_t AGV_EmcHandle;
const osThreadAttr_t AGV_Emc_attributes = {
  .name = "AGV_Emc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Normal */
osThreadId_t Task_NormalHandle;
const osThreadAttr_t Task_Normal_attributes = {
  .name = "Task_Normal",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CAN1_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM9_Init(void);
void StartAGV_RunTask(void *argument);
void AGV_ReadTask(void *argument);
void StartTask03(void *argument);
void StartTask04(void *argument);
void StartTask05(void *argument);

/* USER CODE BEGIN PFP */
void MX_FREERTOS_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_CAN1_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_ADC3_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */
	MY_FLASH_SetSectorAddrs(11, 0x080E0000);
	HAL_TIM_Base_Start_IT(&htim9);
	AGV_Init();
	ME_4341_Init();
	/* Control initialization */
  Control_Port.uModbusType = MB_MASTER;
  Control_Port.port 			 =  &huart3;
  Control_Port.u8id 			 = 0; // For master it must be 0
  Control_Port.u16timeOut  = 1000;
  Control_Port.EN_Port 		 = Control_DR_GPIO_Port;
  Control_Port.EN_Pin  		 = Control_DR_Pin;
  Control_Port.u16regs 		 = usSReg_Control_Read;
  Control_Port.u16regsize  = sizeof(usSReg_Control_Read)/sizeof(usSReg_Control_Read[0]);
  Control_Port.xTypeHW 		 = USART_HW_DMA;
  //Initialize Modbus library
  ModbusInit(&Control_Port);
  //Start capturing traffic on serial Port
  ModbusStart(&Control_Port);
	/***********/
	
	/* SYSTEM initialization */
  System_Port.uModbusType = MB_SLAVE;
  System_Port.port 				= &huart6;
  System_Port.u8id 				= 1; // Formaster it must be 0
  System_Port.u16timeOut  = 1000;
  System_Port.EN_Port 		= NULL;
  System_Port.EN_Pin 			= 0;
  System_Port.u16regs 		= usSReg_System;
  System_Port.u16regsize	= sizeof(usSReg_System)/sizeof(usSReg_System[0]);
  System_Port.xTypeHW 		= USART_HW_DMA;
  //Initialize Modbus library
  ModbusInit(&System_Port);
	
  //Start capturing traffic on serial Port
  ModbusStart(&System_Port);
	/***********/
	
	/* PLC initialization */
  PLC_Port.uModbusType 	= MB_SLAVE;
  PLC_Port.port 				= &huart4;
  PLC_Port.u8id 				= 3; // For master it must be 0
  PLC_Port.u16timeOut 	= 1000;
  PLC_Port.EN_Port 			= PLC_DR_GPIO_Port; //System_DR_GPIO_Port;
  PLC_Port.EN_Pin 			= PLC_DR_Pin;
  PLC_Port.u16regs 			= usSReg_PLC;
  PLC_Port.u16regsize		= sizeof(usSReg_PLC)/sizeof(usSReg_PLC[0]);
  PLC_Port.xTypeHW 			= USART_HW_DMA;
  //Initialize Modbus library
  ModbusInit(&PLC_Port);
  //Start capturing traffic on serial Port
  ModbusStart(&PLC_Port);
	/***********/
	
	/* Charge initialization */
  Charge_Port.uModbusType 	= MB_SLAVE;
  Charge_Port.port 					= &huart1;
  Charge_Port.u8id 					= 27; // For master it must be 0
  Charge_Port.u16timeOut 		= 1000;
  Charge_Port.EN_Port 			= NULL;
  Charge_Port.EN_Pin 				= 0;
  Charge_Port.u16regs 			= usSReg_Charge;
  Charge_Port.u16regsize		= sizeof(usSReg_Charge)/sizeof(usSReg_Charge[0]);
  Charge_Port.xTypeHW 			= USART_HW_DMA;
  //Initialize Modbus library
  ModbusInit(&Charge_Port);
  //Start capturing traffic on serial Port
  ModbusStart(&Charge_Port);
	/***********/
	
	/*Battery initialization */
  Battery_Port.uModbusType 	= MB_MASTER;
  Battery_Port.port 				= &huart5;
  Battery_Port.u8id 				= 0; // For master it must be 0
  Battery_Port.u16timeOut 	= 1000;
  Battery_Port.EN_Port 			= Batterry_DR_GPIO_Port;
  Battery_Port.EN_Pin 			= Batterry_DR_Pin;
  Battery_Port.u16regs 			= usSReg_Battery;
  Battery_Port.u16regsize		= sizeof(usSReg_Battery)/sizeof(usSReg_Battery[0]);
  Battery_Port.xTypeHW 			= USART_HW_DMA;
  //Initialize Modbus library
  ModbusInit(&Battery_Port);
  //Start capturing traffic on serial Port
  ModbusStart(&Battery_Port);
	/***********/
	
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

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
  /* creation of AGV_RunTask */
  AGV_RunTaskHandle = osThreadNew(StartAGV_RunTask, NULL, &AGV_RunTask_attributes);

  /* creation of AGV_Read */
  AGV_ReadHandle = osThreadNew(AGV_ReadTask, NULL, &AGV_Read_attributes);

  /* creation of AGV_Control */
  AGV_ControlHandle = osThreadNew(StartTask03, NULL, &AGV_Control_attributes);

  /* creation of AGV_Emc */
  AGV_EmcHandle = osThreadNew(StartTask04, NULL, &AGV_Emc_attributes);

  /* creation of Task_Normal */
  Task_NormalHandle = osThreadNew(StartTask05, NULL, &Task_Normal_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	MX_FREERTOS_Init();
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
  /* Start scheduler */
	
	Tool_Control(0);
	HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
	HAL_Delay(5000);
	usSReg_System[0] = 0;
	usSReg_System[1] = 0;
	usSReg_System[2] = 0;
	usSReg_System[3] = 0;
	usSReg_System[4] = 0;
	Taget_status = 1;	
  osKernelStart();
	
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		
    /* USER CODE BEGIN 3 */
		
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 12;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 4199;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 1999;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 38400;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 38400;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, Y8_Pin|Y9_Pin|Led_Reset_Pin|Led_Stop_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, DK_Sac_Pin|PLC_DR_Pin|Tool_OUT2_Pin|Tool_OUT1_Pin|Y7_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOI,Speak1_Pin | Speak3_Pin | Speak4_Pin ,GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Speak5_GPIO_Port, Speak5_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */

  HAL_GPIO_WritePin(GPIOF, Led_Run_Pin|Led_Err_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, Control_DR_Pin|Batterry_DR_Pin|System_DR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Led_Start_GPIO_Port, Led_Start_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Y8_Pin Y9_Pin Led_Reset_Pin Led_Stop_Pin */
  GPIO_InitStruct.Pin = Y8_Pin|Y9_Pin|Led_Reset_Pin|Led_Stop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : DK_Sac_Pin Speak4_Pin Speak1_Pin Speak3_Pin
                           Tool_OUT2_Pin Tool_OUT1_Pin Y7_Pin */
  GPIO_InitStruct.Pin = DK_Sac_Pin|Speak4_Pin|Speak1_Pin|Speak3_Pin
                          |Tool_OUT2_Pin|Tool_OUT1_Pin|Y7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : Speak5_Pin */
  GPIO_InitStruct.Pin = Speak5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Speak5_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Speak2_Pin Led_Run_Pin Led_Err_Pin */
  GPIO_InitStruct.Pin = Speak2_Pin|Led_Run_Pin|Led_Err_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : Control_DR_Pin Batterry_DR_Pin System_DR_Pin */
  GPIO_InitStruct.Pin = Control_DR_Pin|Batterry_DR_Pin|System_DR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : Emc_Pin */
  GPIO_InitStruct.Pin = Emc_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Emc_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Start_Pin */
  GPIO_InitStruct.Pin = Start_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Start_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : AGV_Point_Pin */
  GPIO_InitStruct.Pin = AGV_Point_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(AGV_Point_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Stop_Pin */
  GPIO_InitStruct.Pin = Stop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Stop_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Reset_Pin */
  GPIO_InitStruct.Pin = Reset_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Reset_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PLC_DR_Pin */
  GPIO_InitStruct.Pin = PLC_DR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PLC_DR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Tool_IN2_Pin */
  GPIO_InitStruct.Pin = Tool_IN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Tool_IN2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Tool_IN1_Pin SS1_Pin */
  GPIO_InitStruct.Pin = Tool_IN1_Pin|SS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : Bumper_Top_Pin Bumper_Button_Pin */
  GPIO_InitStruct.Pin = Bumper_Top_Pin|Bumper_Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : Led_Start_Pin */
  GPIO_InitStruct.Pin = Led_Start_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Led_Start_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
	
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
	if(GPIO_Pin == Stop_Pin)/*nut dung stop*/
	{
		HAL_GPIO_WritePin(Y9_GPIO_Port, Y9_Pin, !HAL_GPIO_ReadPin(Stop_GPIO_Port, Stop_Pin));
		
		
		if(AGV_Running == 0x00)
		{
			usSReg_System[4] = 1;
			lock = true;
		}
		
		if(HAL_GPIO_ReadPin(Emc_GPIO_Port, Emc_Pin) == 0)	AGV_Running = 0x00;
		
		AGV_Run_save = AGV_Running;
		Emc_status = 0x00;
		Bamber_status = 0x01;
		Charge_start = false;
		usSReg_Charge[0] = 0;
		usSReg_Charge[1] = 0;
		usSReg_Charge[2] = 0;
		usSReg_PLC[6] = 2;
		stop = true;
		
	}
	else if(GPIO_Pin == Start_Pin)/*nut start*/
	{
		if(AGV_Running == 0x04)	tool_status_up = true;
		HAL_GPIO_WritePin(Y7_GPIO_Port, Y7_Pin, !HAL_GPIO_ReadPin(Start_GPIO_Port, Start_Pin));
		if(HAL_GPIO_ReadPin(Emc_GPIO_Port, Emc_Pin) == 0)	AGV_Running = 0x01;
		AGV_Run_save = AGV_Running;
		Emc_status = 0x00;
		Bamber_status = 0x01;
		Speed_Set = 0;/*sua cho nay*/
		stop = false;
		if(lock == true)
		{
			usSReg_System[4] = 0;
			lock = false;
		}
	}
	else if(GPIO_Pin == Reset_Pin)/*nut reset*/
	{
		HAL_GPIO_WritePin(Y8_GPIO_Port, Y8_Pin, GPIO_PIN_SET);
		HAL_Delay(1000);
		NVIC_SystemReset();
	}
	else if(GPIO_Pin == Emc_Pin)/*nut dung khan cap*/
	{
		//Speed_Set = 0;
		AGV_Running = 0x03;
		Emc_status = 0x01;
	}
	else if((GPIO_Pin == Bumper_Top_Pin || GPIO_Pin == Bumper_Button_Pin))
	{
		if(AGV_Running != 0)
		{
			AGV_Running = 0x03;
			Bamber_status = 0x00;
		}
		else
		{
			if(Value_Point != 12)
			{
				AGV_Running = 0x03;
				Bamber_status = 0x00;
			}
		}
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartAGV_RunTask */
/**
  * @brief  Function implementing the AGV_RunTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartAGV_RunTask */
void StartAGV_RunTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		AGV_Control();
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_AGV_ReadTask */
/**
* @brief Function implementing the AGV_Read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AGV_ReadTask */
void AGV_ReadTask(void *argument)
{
  /* USER CODE BEGIN AGV_ReadTask */
  /* Infinite loop */
  for(;;)
  {
		Charge_Check();
		Charge_Control();
		osDelay(1);
  }
  /* USER CODE END AGV_ReadTask */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the AGV_Control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
		Read_Control();
    osDelay(1);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the AGV_Emc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
		EMC_Control();
		AGV_Read_Err();
    osDelay(1);
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the Task_Normal thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void *argument)
{
  /* USER CODE BEGIN StartTask05 */
  /* Infinite loop */
  for(;;)
  {
		System_Protocol();
		PLC_Protocol();
    osDelay(1);
  }
  /* USER CODE END StartTask05 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
	if(htim->Instance == TIM9)
	{
	 counter_bat++;
   if(AGV_Running == 1)
	 {
		counter_can++;
	 }
	 Back_counter++;
	 if(Back_counter > 300)
	 {
		Back_counter = 1000;
	 }
	 if(AGV_Running == 0)
	 {
			Charge_counter++;
	 }
	 if(count_tool_en)
	 {
		 count_tool++;
		 if(count_tool >= 250)
			 count_tool = 255;
	 }
	 if(counter_can > 150) 
	 {
		 AGV_Running = 0x03;
		 counter_can = 170;
	 }
	 if(counter_bat > 15) 
	 {
		 counter_bat = 20;
	 }
	 if(counter_sys > 15) 
	 {
		 counter_sys = 20;
	 }
	 if(Charge_counter > 1500)
	 {
		Charge_counter = 3500;
	 }
	 if(usSReg_System[40] == 1)/*stop emc btn4*/ 
	 {
		 AGV_Running = 0x03;
		 Emc_status = 0x01;
	 }
	 if(usSReg_System[41] == 1)/*btn3*/
	 {
			HAL_GPIO_WritePin(Y9_GPIO_Port, Y9_Pin, !HAL_GPIO_ReadPin(Stop_GPIO_Port, Stop_Pin));
			if(HAL_GPIO_ReadPin(Emc_GPIO_Port, Emc_Pin) == 0)	AGV_Running = 0x00;
		  {
				AGV_Run_save = AGV_Running;
				usSReg_System[40] = 0;
			}
			Emc_status = 0x00;
			Bamber_status = 0x01;
			Charge_start = false;
			usSReg_System[41] = 0;
			usSReg_PLC[6] = 2;
			
	 }
	 if(usSReg_System[42] == 1)/*start btn2*/
	 {
		 if(AGV_Running == 0x04)	tool_status_up = true;
		HAL_GPIO_WritePin(Y7_GPIO_Port, Y7_Pin, !HAL_GPIO_ReadPin(Start_GPIO_Port, Start_Pin));
		if(HAL_GPIO_ReadPin(Emc_GPIO_Port, Emc_Pin) == 0)	
		{
			AGV_Running = 0x01;
		}
		AGV_Run_save = AGV_Running;
		Emc_status = 0x00;
		Bamber_status = 0x01;
		Speed_Set = 0;/*sua cho nay*/
		usSReg_System[42] = 0;
	 }
	 if(usSReg_System[43] == 1)
	 {
		 HAL_GPIO_WritePin(Y8_GPIO_Port, Y8_Pin, GPIO_PIN_SET);
		NVIC_SystemReset();
	 }
//	 if(!Charge_start && AGV_Running != 3 && !stop && AGV_Running != 0)
//	 {
//			
//	 }
//	 else
//	 {
//			HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
//	 }
	 
	 
	}
  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
