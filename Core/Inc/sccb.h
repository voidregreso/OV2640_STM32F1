#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"
#include "main.h"

#define SCCB_SDA_IN()  {GPIOG->CRH&=0XFF0FFFFF;GPIOG->CRH|=0X00800000;}
#define SCCB_SDA_OUT() {GPIOG->CRH&=0XFF0FFFFF;GPIOG->CRH|=0X00300000;}

// funciones de operaciones E/S
#define SCCB_SCL    		PDout(3)	 	//SCL
#define SCCB_SDA    		PGout(13) 		//SDA

#define SCCB_READ_SDA    	PGin(13)  		//Input SDA
#define SCCB_ID   			0X60  			//OV2640 ID

///
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);
#endif
