#include "ESA_Control.h"
#include "Speak.h"

bool Line_Status;
extern bool lock;
bool count_tool_en = false,up_down = false,up_down2 = false;
uint8_t count_tool = 0;
uint8_t test = 0,test2 = 0,test3 = 0;


uint8_t Taget_status;
bool AGV_Enable;
uint8_t ID_Can;
uint8_t counter_can;
uint8_t counter_bat;
uint8_t counter_sys;
extern uint8_t chargeFull;

uint8_t counter_detect_trolley = 0;
uint8_t detect_trolley = 0;
uint16_t RFID_Point[13][4] = {{0, 0, 0, 0},								//AGV Stop
															{1, NULL, 11, NULL},			  //point 1
															{2, 21, 22, 23},						//point 2
															{3, 31, 32, 33},						//point 3
															{4, 41, 42, 43},						//point 4
															{5, 51, 52, 53},						//point 5
															{6, 61, 62, 63},						//point 6
															{7, 71, NULL, NULL},				//point 7
															{8, 81, NULL, NULL},				//point 8
															{9, 91, NULL, NULL},				//point 9
															{10, 210, 310, 410},				//point warehouse
															{11, 211, NULL, NULL},			//point 11
															{12, 212, 312, NULL}};			//point charge
 
uint16_t Value_RFID = 0;
uint16_t Value_Point = 0;
uint16_t Speed_AGV;
uint8_t Speed_buff[4];
														
uint8_t Emc_status;
uint8_t Bamber_status;
uint8_t Tool_status = 0;
bool tool_up;
Motor_State AGV_Dir;
ERR_Value AGV_Err;
Control_Value AGV_Value;
Mode_Set Control_Mode;
uint16_t	Max_Speed = 0;
uint16_t	Min_Speed	= 0;
float  Speed_Set = 0;

uint32_t time_tool;
uint32_t time_run;
bool tool_bool;
bool tool_status_up;
extern bool stop;

bool AGV_DisCharge;

bool AGV_Back;
uint32_t Back_counter;
void AGV_Init(void)
{	
	Speak_out(AGV_Stop);
	AGV_Dir = Forward;
	tool_up = false;
	Taget_status = 1;
	AGV_Enable = false;
	Charge_Enb = 1;
	Charge_start = false;
	AGV_DisCharge = true;
//	while(Tool_Control(0) == false){}
	HAL_GPIO_WritePin(Led_Err_GPIO_Port, Led_Err_Pin, GPIO_PIN_SET);
	if(HAL_GPIO_ReadPin(Emc_GPIO_Port, Emc_Pin) == 0)	AGV_Running = 0x00;
	else AGV_Running = 0x03;
	AGV_Run_save = 0x00;
	usSReg_Control_Write[5] = 0x00;
	counter_can = 0;
	Back_counter = 0;
	AGV_Back = false;
	time_run = HAL_GetTick();
	time_tool = HAL_GetTick();
	time_speak = HAL_GetTick();
//	HAL_ADC_Start_DMA(&hadc3,	Volt_Value,  1 );
}
void AGV_Read_Err(void)
{
	if(usSReg_Control_Read[0] == 0x00)
	{
		AGV_Err = Driver_Err;
	}
	else if(usSReg_Control_Read[1] > 200)
	{
		AGV_Err = Overload_Err;
	}
	else if(usSReg_Control_Read[14] == 0x00)
	{
		AGV_Err = MGS_F_Err;
	}
	else if(usSReg_Control_Read[16] == 0x00)
	{
		AGV_Err = MGS_R_Err;
	}
	else if(counter_can > 150)
	{
		if(ID_Can == 1)	AGV_Err = Lidar_F_Err;
		else if(ID_Can == 2)	AGV_Err = Lidar_R_Err;
	}
	else if(counter_bat > 15)
	{
		AGV_Err = Battery_Err;
	}
	else if(counter_sys > 15)
	{
		AGV_Err = SYS_Err;
	}
	else 	AGV_Err = No_Err;	
}
void Read_Control(void)
{ 
	
	if((Charge_Enb == 0 && AGV_Enable == false) || Charge_counter > 1500)/*bat dau ve sac pin*/
	{
//		if(Value_Point == 6 && test2 == 0)
//		{
//			//Charge_counter = 0;/*da sua cho nay*/
//		}
//		else
//		{
			if(HAL_GPIO_ReadPin(Tool_IN1_GPIO_Port, Tool_IN1_Pin) == 0)
			{
				if(Charge_start == false)
				{
					test2 = 1;
					AGV_DisCharge = false;
					if(chargeFull == 0)
					{
						usSReg_System[4] = 1;
					}
					AGV_Enable = true;
					AGV_Value.Point_control 	= RFID_Point[12][0];
					AGV_Value.Speed_up				= RFID_Point[12][1];
					AGV_Value.Speed_down 			= RFID_Point[12][2];
					AGV_Value.Direct_control 	= RFID_Point[12][3];
				}
			}
			
			
//		}
			
		
	}
	if(test2 == 1 && Value_Point == 12)
	{
		test2 = 0;
	}
	else if(AGV_DisCharge == true && Charge_counter < 1500)
	{
		if(Control_Mode == Auto)
		{
			if(usSReg_System[0] != 0)
			{
				AGV_Enable = true;
				usSReg_System[4] = 1;/*xoa duoc ko?*/
				if(Taget_status == 1)
				{
					Tool_status = usSReg_System[1];
					AGV_Value.Point_control 	= RFID_Point[usSReg_System[0]][0];
					AGV_Value.Speed_up				= RFID_Point[usSReg_System[0]][1];
					AGV_Value.Speed_down 			= RFID_Point[usSReg_System[0]][2];
					AGV_Value.Direct_control 	= RFID_Point[usSReg_System[0]][3];
				}
				else if(Taget_status == 2)
				{
					AGV_Enable = true;
					AGV_Value.Point_control 	= RFID_Point[usSReg_System[2]][0];
					AGV_Value.Speed_up				= RFID_Point[usSReg_System[2]][1];
					AGV_Value.Speed_down 			= RFID_Point[usSReg_System[2]][2];
					AGV_Value.Direct_control 	= RFID_Point[usSReg_System[2]][3];
					if(Value_Point < 10)
					{
						if(Value_RFID == (AGV_Value.Point_control*10))
						{						
							time_tool = HAL_GetTick();
							Tool_status = 0;
							Taget_status = 3; /*sua cho nay*/
							AGV_Enable = false;			
						}
					}
					else
					{
						if(Value_RFID == (AGV_Value.Point_control + 100))
						{
							time_tool = HAL_GetTick();
							Tool_status = 0;
							Taget_status = 3;/*sua cho nay*/
							AGV_Enable = false;
						}
					}
				}
			}
			else	
			{
				AGV_Enable = false;
				if(lock == false)
					usSReg_System[4] = 0;
			}
		}
		else 
		{
			if(usSReg_PLC[14] != 0)
			{
				if(Taget_status == 1)
				{
					AGV_Enable = true;
					Tool_status = 1;
					AGV_Value.Point_control 	= RFID_Point[usSReg_PLC[11]][0];
					AGV_Value.Speed_up 				= RFID_Point[usSReg_PLC[11]][1];
					AGV_Value.Speed_down		  = RFID_Point[usSReg_PLC[11]][2];
					AGV_Value.Direct_control 	= RFID_Point[usSReg_PLC[11]][3];
				}
				else if(Taget_status == 2)
				{
					AGV_Enable = true;
					AGV_Value.Point_control 	= RFID_Point[usSReg_PLC[12]][0];
					AGV_Value.Speed_up 				= RFID_Point[usSReg_PLC[12]][1];
					AGV_Value.Speed_down		  = RFID_Point[usSReg_PLC[12]][2];
					AGV_Value.Direct_control 	= RFID_Point[usSReg_PLC[12]][3];
					if(Value_Point < 10)
					{
						if(Value_RFID == (AGV_Value.Point_control*10))
						{						
							time_tool = HAL_GetTick();
							Tool_status = 0;
							Taget_status = 3;
							usSReg_PLC[9] = Taget_status;
							AGV_Enable = false;			
						}
					}
					else
					{
						if(Value_RFID == (AGV_Value.Point_control + 100))
						{
							time_tool = HAL_GetTick();
							Tool_status = 0;
							Taget_status = 3;
							usSReg_PLC[9] = Taget_status;
							AGV_Enable = false;
						}
					}				
				}
				else	AGV_Enable = false;
			}
			else	
			{
				Taget_status = 1;
				Tool_status = 0;
				AGV_Enable = false;
				AGV_Value.Point_control 	= 0;
				AGV_Value.Speed_up 				= 0;
				AGV_Value.Speed_down		  = 0;
				AGV_Value.Direct_control 	= 0;
			}
		}
	}
	
	
	if(AGV_Value.Point_control != 12)
	{
		if(Value_Point != 0)
		{
			if(Value_Point < AGV_Value.Point_control) 
			{
				AGV_Dir = Reviece;
				if(Value_Point != 12 && Value_Point != 11)
				{
					float sub = 0;
					if(Value_Point == 8)
					{
						sub = Max_Speed*0.4;
					}
					if(Value_Point == 9) 
					{
						AGV_Back = false;/*da sua lai cho nay*/
						sub = Max_Speed * 0.5;
					}
					if(Speed_Set < Max_Speed - sub)
					{
						osDelay(500);
						Speed_Set += 20;
					}
					else Speed_Set = Max_Speed - sub;
				}
				else
				{
					if(Max_Speed > 50)
					{
						if(AGV_Value.Point_control == 5)
							Speed_Set = 600;
						else
							Speed_Set = 800;
					}
					else
					{
						Speed_Set = 0;
					}
				}
					
			}
			else if(Value_Point > AGV_Value.Point_control) 
			{
				if(Value_Point > 10) AGV_Dir = R_Left;
				else AGV_Dir = Forward;

				if(Value_Point != 12 && Value_Point != 11)
				{
					float sub = 0;
					if(Value_Point == 8)
					{
						sub = Max_Speed*0.4;
						
					}
					if(Value_Point == 9)
					{
						sub = Max_Speed*0.5;
					}
					if(Value_Point == 6 && Taget_status == 2)
					{
						sub = Max_Speed*0.5;
					}
					
					if(Speed_Set < Max_Speed - sub)
					{
						osDelay(500);
						Speed_Set += 20;
					}
					else Speed_Set = Max_Speed - sub;
				}
				else
				{
					if(Max_Speed > 50)
					{
						if(AGV_Value.Point_control == 5)
							Speed_Set = 400;
						else
							Speed_Set = 800;
					}
					else
					{
						Speed_Set = 0;
					}
				}
			}
			else if(Value_Point == AGV_Value.Point_control)	
			{
				if(Taget_status == 1)
				{
					if(Value_Point == 10)
					{
						if(AGV_Back == false)
						{
							Back_counter = 0;
							AGV_Back = true;
						}
						if(Back_counter > 300)	
						{
							AGV_Dir = Forward;
		
							/*************************/
							
							
							if(HAL_GPIO_ReadPin(SS1_GPIO_Port, SS1_Pin) == GPIO_PIN_SET && test3 == 0)/*npn vs pnp*/
							{
								osDelay(700); //giam do tre
								usSReg_Control_Write[4] = 0;
								tool_up = true;
								Taget_status = 2;
								time_tool = HAL_GetTick();
								test3 = 1;
							}
							/*************************/
						}
						else
						{
							if(Speed_Set > 400)
							{
								Speed_Set -= 80;
								osDelay(100);
							}
							else Speed_Set = 400;
							if(Value_RFID < AGV_Value.Direct_control)
							{						
								AGV_Dir = Reviece;		
							}
							else if(Value_RFID > AGV_Value.Direct_control)
							{						
								AGV_Dir = Forward;		
							}
						}
					}
					else
					{///////////////
						/*khong keo trolley trong thang may*/
						if(Value_RFID == AGV_Value.Direct_control)	
						{
							
							if(AGV_Dir != Forward)
							{
								counter_detect_trolley++;
								if(counter_detect_trolley > 4 && Taget_status != 2 && lock == false) /*co the la cho nay*/
								{
									usSReg_PLC[6] = 2;
									counter_detect_trolley = 0;
									usSReg_System[0] = 0;
									usSReg_System[1] = 0;
									usSReg_System[2] = 0;
									usSReg_System[3] = 0;
									usSReg_System[4] = 0;
									detect_trolley = 1;
									Taget_status = 1;
									AGV_Running = 0;
									lock = false;
									Charge_counter = 1000;
									
								}
								
							}
							AGV_Dir = Forward;
							
							if(HAL_GPIO_ReadPin(SS1_GPIO_Port, SS1_Pin) == GPIO_PIN_SET && test3 == 0)/*npn va pnp*/
							{
								osDelay(700); //giam do tre
								usSReg_Control_Write[4] = 0;
								tool_up = true;
								Taget_status = 2;
								time_tool = HAL_GetTick();
								test3 = 1;
							}
							/**********/
						}
						else
						{
							if(Speed_Set > 400)
							{
								Speed_Set -= 80;
								osDelay(100);
							}
							else Speed_Set = 400;
							if(Value_RFID < AGV_Value.Direct_control)
							{
								AGV_Dir = Reviece;
							}
							else if(Value_RFID > AGV_Value.Direct_control)
							{						
								AGV_Dir = Forward;		
							}
						}
					}
				}
				else if(Taget_status == 2)
				{
					AGV_Back = false;
					
					if(Value_Point == AGV_Value.Point_control)	
					{
						if(Speed_Set > 200)
						{
							Speed_Set -= 50;
							osDelay(100);
						}
						else
						{
							Speed_Set = 200;
							
						}
					}
				}
			}
		}
		else
		{
			if(Taget_status == 1)
			{
				if(AGV_Dir == Forward)	AGV_Dir = Reviece;
				else if(AGV_Dir == Reviece) AGV_Dir = Forward;
				else 
				{		
					Speed_Set = 400;/*sua cho nay*/
					AGV_Dir = R_Right;
				}
			}
			else
			{
				
				AGV_Running = 3;
				AGV_Err = Unknown_Err;
			}
		}
	}
	
	else if(AGV_Value.Point_control == 12) /*doan can sua de agv quat*/
	{
		if(Value_Point != 0)
		{
			if(Value_Point != AGV_Value.Point_control)	
			{
				if(Value_RFID > 50) 
				{
					AGV_Dir = Forward;
					if(Value_Point != 12 && Value_Point != 11)
					{
						if(Speed_Set < Max_Speed)
						{
							osDelay(500);
							Speed_Set += 20;
						}
						else Speed_Set = Max_Speed;
					}
					else
					{
						if(Max_Speed > 50)
						{
							if(AGV_Value.Point_control == 5)
								Speed_Set = 400;
							else
								Speed_Set = 800;
						}
						else
						{
							Speed_Set = 0;
						}
					}
				}
				else if(Value_RFID == 50)
				{
					AGV_Dir = F_Right;
					if(Value_Point != 12 && Value_Point != 11)
					{
//						if(Speed_Set < Max_Speed)
//						{
//							osDelay(500);
//							Speed_Set += 20;
//						}
//						else Speed_Set = Max_Speed;
						if(Speed_Set > 800)
						{
							osDelay(500);
							Speed_Set -= 5;
						}
						else
							Speed_Set = 800;
					}
					else
					{
						if(Max_Speed > 50)
						{
							if(AGV_Value.Point_control == 5)
								Speed_Set = 400;
							else
								Speed_Set = 800;
						}
						else
						{
							Speed_Set = 0;
						}
					}
				}
				else
				{
					AGV_Dir = Reviece;
					if(Value_Point != 12 && Value_Point != 11)
					{
						if(Speed_Set < Max_Speed)
						{
							osDelay(500);
							Speed_Set += 20;
						}
						else Speed_Set = Max_Speed;
						
					}
					else
					{
						if(Max_Speed > 50)
						{
							if(AGV_Value.Point_control == 5)
								Speed_Set = 400;
							else
								Speed_Set = 800;
						}
						else
						{
							Speed_Set = 0;
						}
					}
				}
			}
			else 
			{
				if(Charge_start == false)
				{
					if(HAL_GPIO_ReadPin(AGV_Point_GPIO_Port, AGV_Point_Pin) == 0)	
					{
						if(chargeFull == 0)
						{
							usSReg_Charge[0] = 1;
							usSReg_Charge[1] = 1;
						}
						else
						{
							usSReg_Charge[0] = 0;
							usSReg_Charge[1] = 0;
						}
						Charge_start = true;
						AGV_Running = 0;
						Taget_status = 3;
						usSReg_PLC[9] = Taget_status;
						AGV_Enable = false;	
					}
					else
					{
						if(chargeFull == 0)
						{
							usSReg_Charge[0] = 1;
							usSReg_Charge[1] = 1;
						}
						else
						{
							usSReg_Charge[0] = 0;
							usSReg_Charge[1] = 0;
						}
						if(Speed_Set > 400)//////////////////////////////////////////////////////
						{
							Speed_Set -= 80;
							osDelay(50);
						}
						else Speed_Set = 400;
						AGV_Dir = Forward;
					}
				}
			}
		}
		else
		{
			Speed_Set = 400;/*sua cho nay*/
			AGV_Dir = R_Right;
		}
	}
}

void EMC_Control(void)
{
	Emc_status = HAL_GPIO_ReadPin(Emc_GPIO_Port, Emc_Pin);
	if(Emc_status == 1 || usSReg_System[40] == 1)	
	{
		Speak_out(AGV_Emc);
		usSReg_PLC[6] = 8;
		/*test*/
	}
	if(Bamber_status == 0x00) 
	{
		usSReg_PLC[6] = 8;
		Speak_out(AGV_Bamber);
		/*test*/
	}
	
	HAL_GPIO_WritePin(Y7_GPIO_Port, Y7_Pin, !HAL_GPIO_ReadPin(Start_GPIO_Port, Start_Pin));
	HAL_GPIO_WritePin(Y9_GPIO_Port, Y9_Pin, !HAL_GPIO_ReadPin(Stop_GPIO_Port, Stop_Pin));
}

bool Tool_Control(uint8_t status)
{	
	if(status == 0) // ha
	{
		/**************/
		HAL_GPIO_WritePin(Tool_OUT1_GPIO_Port, Tool_OUT1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Tool_OUT2_GPIO_Port, Tool_OUT2_Pin, GPIO_PIN_SET);
		while(HAL_GPIO_ReadPin(Tool_IN2_GPIO_Port,Tool_IN2_Pin) == GPIO_PIN_RESET)
		{
			osDelay(10);
		}
		
		
		HAL_GPIO_WritePin(Tool_OUT1_GPIO_Port, Tool_OUT1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Tool_OUT2_GPIO_Port, Tool_OUT2_Pin, GPIO_PIN_RESET);
		/************/
		tool_up = false;
		usSReg_PLC[6] = 2;
		test3 = 0;
		/**************/
	}
	else if(status == 1)
	{
		if(tool_up == true)
		{
			HAL_GPIO_WritePin(Tool_OUT1_GPIO_Port, Tool_OUT1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Tool_OUT2_GPIO_Port, Tool_OUT2_Pin, GPIO_PIN_RESET);
			osDelay(690);
			Speed_Set = 400;//
			tool_up = false;
			HAL_GPIO_WritePin(Tool_OUT1_GPIO_Port, Tool_OUT1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Tool_OUT2_GPIO_Port, Tool_OUT2_Pin, GPIO_PIN_RESET);
		}
	}
}
void PLC_Protocol(void)
{
	/*Read data*/
	Control_Mode = usSReg_PLC[10];
	if(usSReg_PLC[13] < 50) Max_Speed = 0;
	else if(usSReg_PLC[13] > 2500) Max_Speed = 2500;
	else Max_Speed = usSReg_PLC[13];
	 
	
	/*Write data PLC*/	
	usSReg_PLC[0] = Value_Point;
	usSReg_PLC[1] = usSReg_Battery[40];
	usSReg_PLC[2] = (usSReg_Battery[37]/10) - 273;
	floatToByte(Speed_buff, (Speed_AGV*0.471)/3240);
	usSReg_PLC[3] = Speed_buff[1]<<8|Speed_buff[0];
	usSReg_PLC[4] = Speed_buff[3]<<8|Speed_buff[2];
	usSReg_PLC[5] = AGV_Err - 400;
//	usSReg_PLC[6] = AGV_Running;
	usSReg_PLC[7] = AGV_Dir;
	if(AGV_Running == 0x01) {usSReg_PLC[8]  = !Rx_CAN[1]|!Rx_CAN[2]|!Rx_CAN[3];}
	else	{usSReg_PLC[8] = 0x00;} 
	if(Control_Mode == Manual)
	{
		if(Taget_status == 3)
		{
			if(usSReg_PLC[9] == 1)
			{
				Taget_status = usSReg_PLC[9];
			}
			else usSReg_PLC[9] = Taget_status;
		}
		else usSReg_PLC[9] = Taget_status;
	}
	else
	{
		if(Taget_status == 3)
		{
			if(Value_Point == 12 && lock == false)
			{
				usSReg_System[4] = 0;
				Taget_status = 1;
			}
			
		}
	}
}

void System_Protocol(void)
{
	Speed_AGV 	= usSReg_Control_Read[4] + usSReg_Control_Read[5];
	
	if(AGV_Dir == Forward)	Line_Status = usSReg_Control_Read[14];
	else if(AGV_Dir == Reviece)	Line_Status = usSReg_Control_Read[16];
	
	if(Value_RFID != usSReg_Control_Read[38])
	{
		Value_RFID  = usSReg_Control_Read[38];
		if(Value_RFID > 100)	
		{
			Value_Point = Value_RFID%100;
		}
		else Value_Point = Value_RFID/10;
	}
	if(Value_RFID == 112 && AGV_Dir == Forward)
		Value_Point = 5;
	/*co the sua cho nay*/
	/*Write Control System*/
	usSReg_System[30] = Value_Point;
//	usSReg_System[31]	= AGV_Err;
	usSReg_System[31] = No_Err;
	usSReg_System[32] = usSReg_Battery[40];
	usSReg_System[33]	=	Speed_AGV/2;
	
	/*Read Control System*/

}

void AGV_Control(void)
{
	if(AGV_Enable == false) 
	{
		if(AGV_Running != 0x03)
		{
			AGV_Running = 0;
			Speak_out(AGV_Stop); //AGV dung
		}
	}
	else 
	{
		if(AGV_Running != 0x03)
		{
			AGV_Running = AGV_Run_save;
			if(AGV_Running == 0) 
			{
				Speak_out(AGV_Stop);
			}
			else 
			{
				if(usSReg_PLC[8] != 1)
				{
					Speak_out(AGV_Start);
				}
			}
		}
	}
	usSReg_Control_Write[0] = AGV_Enable;
	if(usSReg_Control_Write[1] != AGV_Running)
	{		
		if(AGV_Running == 1)
		{
			if(Rx_CAN[3] == 1)	
			{
				Speak_out(AGV_Wait);
			}
			osDelay(5000);		
			usSReg_Control_Write[1] = AGV_Running;
			
		}
		else	
		{
			Speak_out(AGV_Stop);
			usSReg_Control_Write[1] = AGV_Running;
			
		}
			
	}
	//Tool_Control(Tool_status);
	
	if(Tool_status == 0)
	{
		count_tool_en = true;
		if(count_tool >= 60)
		{
			count_tool = 0;
			count_tool_en = false;
			
			if(up_down)
			{
				usSReg_Control_Write[4] = 0;
				up_down = false;
			}
			Tool_Control(Tool_status);
			if(Taget_status == 3 && lock == false)
			{
				usSReg_System[0] = 0;/*xoa diem lay*/
				usSReg_System[1] = 0;
				usSReg_System[2] = 0;
				usSReg_System[3] = 0;
				usSReg_System[4] = 0;
				Taget_status = 1;
			}
			if(up_down2)
			{
				AGV_Running = 0x00;
				osDelay(200);
				AGV_Running = 0x00;
				osDelay(200);
				AGV_Running = 0x00;
				up_down2 = false;
			}
		}
	}
	else
	{
		Tool_Control(Tool_status);
		count_tool_en = false;
		count_tool = 0;
		up_down = true;
		up_down2 = true;
	}
	/*xac  dinh huong cua agv*//*thay doi goc quet cua lidar*/
	if(AGV_Dir == Reviece)
	{
		//AGV di thang lui ve sau
		if(!Charge_start && AGV_Running != 3 && Speed_AGV != 0 && !stop && AGV_Running != 0)
		{
			usSReg_PLC[6] = 13;
		}
		if(Value_Point != 12)
		{
			/*
			if(Value_Point == 0)
			
			*/
			ID_Can = 2;
			Channel_designation(2, 1);
		}
		else
		{
			Rx_CAN[1] = 1;
			counter_can = 0;
			
		}
		usSReg_Control_Write[2] = 0x00;
		usSReg_Control_Write[3] = 0x00;
	}
	else if(AGV_Dir == R_Right)
	{	
		if(!Charge_start && AGV_Running != 3 && !stop && AGV_Running != 0)
		{
			usSReg_PLC[6] = 11;
		}
		ID_Can = 2;
		Channel_designation(2, 1);
		usSReg_Control_Write[2] = 0x00;
		usSReg_Control_Write[3] = 0x01;	
	}
	else if(AGV_Dir == R_Left)
	{	
		if(!Charge_start && AGV_Running != 3&& !stop && AGV_Running != 0)
		{
			usSReg_PLC[6] = 11;
		}
		ID_Can = 2;
		Channel_designation(2, 1);
		usSReg_Control_Write[2] = 0x00;
		usSReg_Control_Write[3] = 0x02;
	}
	else if(AGV_Dir == Forward)
	{	
		//AGV di thang tien
		if(!Charge_start && AGV_Running != 3 && Speed_AGV != 0 && !stop && AGV_Running != 0)
		{
				usSReg_PLC[6] = 13;
		}
		ID_Can = 1;
//		Channel_designation(1, 1);
		if(Taget_status == 2)/*dang keo trolley*/
		{
			Channel_designation(1, 0);
		}
		else
		{
			Channel_designation(1, 1);
		}
		usSReg_Control_Write[2] = 0x01;
		usSReg_Control_Write[3] = 0x00;
	}
	else if(AGV_Dir == F_Right)
	{	
		if(!Charge_start && AGV_Running != 3 && !stop && AGV_Running != 0)
		{
			usSReg_PLC[6] = 11;
		}
		ID_Can = 1;
//		Channel_designation(1, 1);
		if(Taget_status == 2)/*dang keo trolley*/
		{
			Channel_designation(1, 0);
		}
		else
		{
			Channel_designation(1, 1);
		}
		usSReg_Control_Write[2] = 0x01;
		usSReg_Control_Write[3] = 0x02;
	}
	else if(AGV_Dir == F_Left)
	{	
		if(!Charge_start && AGV_Running != 3 && !stop && AGV_Running != 0)
		{
			usSReg_PLC[6] = 11;
		}
		ID_Can = 1;
//		Channel_designation(1, 1);
		if(Taget_status == 2)/*dang keo trolley*/
		{
			Channel_designation(1, 0);
		}
		else
		{
			Channel_designation(1, 1);
		}
		usSReg_Control_Write[2] = 0x01;
		usSReg_Control_Write[3] = 0x01;
	}
	
	if(Rx_CAN[1] == 0)
	{
		
		//kiem tra xem co gap vat can ko
		if(AGV_Running == 0x01)Speak_out(AGV_Barrier);
		if(Rx_CAN[2] == 0)
		{
			if(Rx_CAN[3] == 0)	
			{
				usSReg_Control_Write[1] = 3;
				usSReg_Control_Write[4] = 0;
			}
			else	
			{
				if(usSReg_Control_Write[1] != AGV_Running)
				{		
					if(AGV_Running == 1)
					{
						
						osDelay(3000);		
						usSReg_Control_Write[1] = AGV_Running;
					}
					else	
					{
						usSReg_Control_Write[1] = AGV_Running;
					}
			
				}
				if(usSReg_Charge[3] == 1)
				{
					usSReg_Control_Write[4] = 0;
					Speed_Set = 0;
				}
				else
				{
					usSReg_Control_Write[4] = Speed_Set/3;
				}
			}
		}
		else 
		{
			usSReg_Control_Write[1] = AGV_Running;
			/*sua cho nay*/
			if(usSReg_Charge[3] == 1)
			{
				usSReg_Control_Write[4] = 0;
			}
			else
			{
				usSReg_Control_Write[4] = Speed_Set/2;
			}
		}
		if(!Charge_start && AGV_Running != 0 && AGV_Running != 3)
			usSReg_PLC[6] = 9;
	}
	else 
	{
		/*sua cho nay*/
		if(usSReg_Charge[3] == 1)
		{
			usSReg_Control_Write[4] = 0;
		}
		else
		{
			usSReg_Control_Write[4] = Speed_Set;
		}
		usSReg_Control_Write[1] = AGV_Running;
		
	}
}

void floatToByte(uint8_t* bytes, float f)
{
  int length = sizeof(float);

  for(int i = 0; i < length; i++){
    bytes[i] = ((uint8_t*)&f)[i];
  }
}




