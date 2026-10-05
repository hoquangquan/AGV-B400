#include "Lidar_ME4341.h"

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
CAN_FilterTypeDef  sFilterConfig;

uint32_t TxMailbox; 
uint8_t Rx_CAN[5];

void ME_4341_Init(void)
{
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
		
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/*CAN-HS Write data*/
void Channel_designation(uint8_t ID, uint8_t Channel_number)
{
	uint8_t buffer[2] = {0x00, Channel_number};
	TxHeader.StdId = ID;
	TxHeader.ExtId = 0x01;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 2;
  if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, buffer, &TxMailbox) != HAL_OK)	Error_Handler();

}
void Smart_channel(uint8_t ID, uint16_t angle_value, uint16_t speed_value, uint8_t group_number)
{
	uint8_t buffer[8] = {0x01, 0x00, angle_value>>8, (uint8_t)angle_value, speed_value>>8, (uint8_t)speed_value, 0x00, group_number};
	TxHeader.StdId = ID;
	TxHeader.ExtId = 0x01;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 8;
  if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, buffer, &TxMailbox) != HAL_OK)
	{Error_Handler();}
}

