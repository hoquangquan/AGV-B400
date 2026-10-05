#include "charge.h"
#include "ESA_Control.h"

uint32_t Charge_counter;
uint32_t Volt_Value[1];
uint32_t Volt_buff;
uint8_t Volt_counter = 0;
float _err_measure;
float _err_estimate;
float _q;
float _current_estimate;
float _last_estimate;
float _kalman_gain;

uint8_t Charge_Enb;

uint8_t status_led = 0;
uint8_t status_charge = 0;
uint8_t status_tool = 0;
uint8_t Charge_status;

extern float  Speed_Set;

bool Charge_start;
uint8_t chargeFull = 0;
extern bool AGV_DisCharge;
extern Mode_Set Control_Mode;
void kalman(float mea_e, float est_e, float q)
{
  _err_measure=mea_e;
  _err_estimate=est_e;
  _q = q;
}

float kalman_update(float mea)
{
  _kalman_gain = _err_estimate/(_err_estimate + _err_measure);
  _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
  _err_estimate =  (1.0 - _kalman_gain)*_err_estimate + fabs(_last_estimate-_current_estimate)*_q;
  _last_estimate=_current_estimate;

  return _current_estimate;
}

float Read_Volt(void)
{
	float  Bat_Volt;
	Volt_buff += kalman_update(Volt_Value[0]);
	Volt_counter++;
	if(Volt_counter == 10)
	{
		Bat_Volt  = (Volt_buff*3.3/40960)*21;
		Volt_counter = Volt_buff = 0;
	}
	return Bat_Volt;
}

uint8_t Charge_Check(void)
{
	if(usSReg_Battery[40] < 20)
	{
		Charge_Enb = 0;
	}
	else if(usSReg_Battery[40] > 20 && usSReg_Battery[40] < 96)
	{
		if(chargeFull == 0)
		{
			Charge_Enb = 1;
		}
		else
		{
			if(usSReg_Battery[40] < 86)
			{
				chargeFull = 0;
			}
		}
	}
	else if(usSReg_Battery[40] >= 95)
	{
		if(chargeFull != 1)
			chargeFull = 1;
		Charge_Enb = 2;
	}
	else Charge_Enb = 3;
}

void Charge_Control(void)
{
	if(Charge_start == true)
	{
		if(HAL_GPIO_ReadPin(AGV_Point_GPIO_Port, AGV_Point_Pin) == 0)	
		{	
			if(chargeFull == 0)
			{
				usSReg_Charge[0] = 1;
				usSReg_Charge[1] = 1;
				usSReg_Charge[2] = 1;
			}
			else
			{
				usSReg_Charge[0] = 0;
				usSReg_Charge[1] = 0;
				usSReg_Charge[2] = 0;
			}
			if(usSReg_Charge[4] == 1)
			{
				HAL_GPIO_WritePin(DK_Sac_GPIO_Port, DK_Sac_Pin, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(DK_Sac_GPIO_Port, DK_Sac_Pin, GPIO_PIN_RESET);
			}
		}
		else 
		{
			if(chargeFull == 0)
			{
				usSReg_Charge[0] = 1;
				usSReg_Charge[1] = 2;
				usSReg_Charge[2] = 0;
			}
			else
			{
				usSReg_Charge[0] = 0;
				usSReg_Charge[1] = 0;
				usSReg_Charge[2] = 0;
			}
			HAL_GPIO_WritePin(DK_Sac_GPIO_Port, DK_Sac_Pin, GPIO_PIN_RESET);
			Charge_start = false;
		}
	}
	if(Charge_Enb != 0)
	{
		AGV_DisCharge = true;
		if(Control_Mode == Auto)
		{
			if(usSReg_System[0] != 0)
			{
				Charge_counter = 0;
			}
		}
		else
		{
			if(usSReg_PLC[14] != 0)
			{
				Charge_counter = 0;
			}
		}
		if(AGV_Running == 1) 
		{
			if(Charge_start == true)
			{
				usSReg_Charge[0] = 0;
				usSReg_Charge[1] = 0;
				usSReg_Charge[2] = 0;
			}
			Charge_start = false;
			//usSReg_Charge[3] = 0;
			usSReg_Charge[4] = 0;
			HAL_GPIO_WritePin(DK_Sac_GPIO_Port, DK_Sac_Pin, GPIO_PIN_RESET);
		}
		else
		{	
			if(Charge_Enb == 2)	Charge_start = false;
			
			if(Charge_start == true && chargeFull == 0)
			{
				usSReg_Charge[0] = 1;
				usSReg_Charge[1] = 1;
				usSReg_Charge[2] = 1;
				usSReg_PLC[6] = 2;
			}
			else
			{
				usSReg_Charge[0] = 0;
				usSReg_Charge[1] = 0;
				usSReg_Charge[2] = 0;
				//usSReg_Charge[3] = 0;
				usSReg_Charge[4] = 0;
				HAL_GPIO_WritePin(DK_Sac_GPIO_Port, DK_Sac_Pin, GPIO_PIN_RESET);
			}
		}
	}
}
