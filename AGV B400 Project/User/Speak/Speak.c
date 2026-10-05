#include "Speak.h"
uint32_t time_speak;
uint8_t old_status = 255,cnt_status = 0;
bool speak_batt;
/*

Speak 1: GPIOI_10
Speak 2: GPIOF_0
Speak 3: GPIOI_11
Speak 4: GPIOI_9
Speak 5: GPIOC_13

Status 			| Speak 1 | Speak 2 | Speak 3 | Speak 4 | Speak 5|
AGV_Stop
AGV_Start
AGV_Emc
AGV_Bamber
AGV_Barrier
AGV_Line
AGV_Wait
SYS_INT
SYS_DONE
	
*/
void Speak_out(uint8_t status)
{
	if(Charge_Enb == 0)
	{
		while(HAL_GetTick() - time_speak > 30000)
		{
			speak_batt = !speak_batt;
			time_speak = HAL_GetTick();
		}
		if(speak_batt == true && Value_Point != 12)
		{
			HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
			speak_batt = false;	
		}
		else
		{
			HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
		}
	}
	else
	{
		switch(status)
		{
			
			case AGV_Stop:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
				break;
			}
			case AGV_Start:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_RESET);
				break;
			}
			case AGV_Emc:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_RESET);
				break;
			}
			case AGV_Bamber:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_RESET);
				break;
			}
			case AGV_Barrier:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
				break;
			}
			case AGV_Line:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
				break;
			}
			case AGV_Wait:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_RESET);
				break;
			}
			case SYS_Init:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
				break;
			}
			case SYS_Done:
			{
				HAL_GPIO_WritePin(Speak1_GPIO_Port,Speak1_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak2_GPIO_Port,Speak2_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak3_GPIO_Port,Speak3_Pin,GPIO_PIN_SET);
				HAL_GPIO_WritePin(Speak4_GPIO_Port,Speak4_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Speak5_GPIO_Port,Speak5_Pin,GPIO_PIN_SET);
				break;
			}
			default:
			break;
		}
	}
}
