#include "sys.h"
#include "sccb.h"
#include "tim.h"
#include "usart.h"

// Initialize the SCCB interface
void SCCB_Init(void) {
    SCCB_SDA_OUT();
}

void SCCB_Start(void) {
	SCCB_SDA=1;     // Data line high level
    SCCB_SCL=1;	    // Data line goes from high to low when the clock line is high
    delay_us(50);
    SCCB_SDA=0;
    delay_us(50);
    SCCB_SCL=0;	    //Data line back to low level for necessarity of single operation function
}

//SCCB stop signal
//When the clock is high, the low to high of the data line is the SCCB stop signal
//In idle condition, SDA and SCL are high
void SCCB_Stop(void) {
    SCCB_SDA=0;
    delay_us(50);
    SCCB_SCL=1;
    delay_us(50);
    SCCB_SDA=1;
    delay_us(50);
}
//produce NA signal
void SCCB_No_Ack(void) {
    delay_us(50);
    SCCB_SDA=1;
    SCCB_SCL=1;
    delay_us(50);
    SCCB_SCL=0;
    delay_us(50);
    SCCB_SDA=0;
    delay_us(50);
}
//SCCB, write a byte
//return value:0,success;1,failure.
uint8_t SCCB_WR_Byte(uint8_t dat) {
    uint8_t j,res;
    for(j=0; j<8; j++) { // Cycle 8 times to send data
        if(dat&0x80)SCCB_SDA=1;
        else SCCB_SDA=0;
        dat<<=1;
        delay_us(50);
        SCCB_SCL=1;
        delay_us(50);
        SCCB_SCL=0;
    }
    SCCB_SDA_IN();		// Set SDA as input
    delay_us(50);
    SCCB_SCL=1;			//Receive the ninth bit to determine if the transmission was successful
    delay_us(50);
    if(SCCB_READ_SDA)res=1;  //SDA=1 send failure, return 1
    else res=0;         //SDA=0 send success, return 0
    SCCB_SCL=0;
    SCCB_SDA_OUT();
		if(res) printf("Cannot write byte!\r\n");
    return res;
}
//SCCB reads a byte
//At the rising edge of SCL, the data is latched
//return value: read data
uint8_t SCCB_RD_Byte(void) {
    uint8_t temp=0,j;
    SCCB_SDA_IN();		// Set SDA as input
    for(j=8; j>0; j--) {	// Cycle 8 times to receive data
        delay_us(50);
        SCCB_SCL=1;
        temp=temp<<1;
        if(SCCB_READ_SDA)temp++;
        delay_us(50);
        SCCB_SCL=0;
    }
    SCCB_SDA_OUT();	
	if(!temp) printf("Readbyte = 0!\r\n");
    return temp;
}
//write register
//return 0 for success and 1 for error
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data) {
    uint8_t res=0;
    SCCB_Start(); 					//Start SCCB transmission
    if(SCCB_WR_Byte(SCCB_ID))res=1;	//Write Device ID
    delay_us(100);
    if(SCCB_WR_Byte(reg))res=1;		//Write Regaddr
    delay_us(100);
    if(SCCB_WR_Byte(data))res=1; 	//Write data
    SCCB_Stop();
		if(res) printf("Cannot write register!\r\n");
    return res;
}
//Return the read register value
uint8_t SCCB_RD_Reg(uint8_t reg) {
    uint8_t val=0;
    SCCB_Start(); 				//Start SCCB transmission
    SCCB_WR_Byte(SCCB_ID);		//Write Device ID
    delay_us(100);
    SCCB_WR_Byte(reg);			//Write Regaddr
    delay_us(100);
    SCCB_Stop();
    delay_us(100);
    //Set the register address and then read
    SCCB_Start();
    SCCB_WR_Byte(SCCB_ID|0X01);	//send read command
    delay_us(100);
    val=SCCB_RD_Byte();		 	//read data
    SCCB_No_Ack();
    SCCB_Stop();
		if(!val) printf("Readreg = 0!\r\n");
    return val;
}
