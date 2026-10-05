#ifndef __ESA_CONTROL_H
#define __ESA_CONTROL_H

#include "main.h"		

typedef enum
{
	Stop 		= 6,
	Forward = 0,
	F_Right = 1,
	F_Left 	= 2,
	Reviece = 3,
	R_Right = 4,
	R_Left 	= 5,
}Motor_State;

typedef enum
{
	No_Err			 = 400,
	MGS_F_Err 	 = 401,
	MGS_R_Err 	 = 402,
	Lidar_F_Err  = 403,
	Lidar_R_Err  = 404,
	Driver_Err 	 = 405,
	SYS_Err 		 = 406,
	Tool_Err 		 = 407,
	Battery_Err  = 408,
	Overload_Err = 409,
	Unknown_Err  = 410
}ERR_Value;

typedef enum
{
	Auto   = 0,
	Manual = 1
} Mode_Set;

typedef struct
{
	uint16_t Point_control;
	uint16_t Speed_up;
	uint16_t Speed_down;
	uint16_t Direct_control;
}Control_Value;

void AGV_Init(void);
void Read_Control(void);
void EMC_Control(void);
void System_Protocol(void);
void PLC_Protocol(void);
bool Tool_Control(uint8_t status);
void AGV_Control(void);
void AGV_Read_Err(void);
void floatToByte(uint8_t* bytes, float f);

#endif


