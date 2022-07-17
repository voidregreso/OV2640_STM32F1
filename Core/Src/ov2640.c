#include "sys.h"
#include "ov2640.h"
#include "ovcfg.h"
#include "usart.h"
#include "sccb.h"

#define SWD_ENABLE 0X01

// Initialize OV2640
// After configuration, the default output is 1600*1200 size picture!!
// return value: 0, success
// other, error code
uint8_t OV2640_Init(void)
{
    uint16_t i = 0;
    uint16_t reg;
    OV2640_PWDN = 0; // POWER ON
    HAL_Delay(10);
    OV2640_RST = 0; // Reset OV2640
    HAL_Delay(10);
    OV2640_RST = 1;                        // End reset
    SCCB_Init();                           // Inicializar el puerto E/S de SCCB
    SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01); // Manipular registros de sensores
    SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80); // soft resetOV2640
    HAL_Delay(50);
    reg = SCCB_RD_Reg(OV2640_SENSOR_MIDH); // Lea high 8 bits de la identificación del fabricante
    reg <<= 8;
    reg |= SCCB_RD_Reg(OV2640_SENSOR_MIDL); // Lea low 8 bits de la identificación del fabricante
    if (reg != OV2640_MID)
    {
        printf("MID:%d\r\n", reg);
        return 1;
    }
    reg = SCCB_RD_Reg(OV2640_SENSOR_PIDH); // Lea high 8 bits de la identificación del fabricante
    reg <<= 8;
    reg |= SCCB_RD_Reg(OV2640_SENSOR_PIDL); // Lea low 8 bits de la identificación del fabricante
    if (reg != OV2640_PID)
    {
        printf("HID:%d\r\n", reg);
        return 2;
    }
    // Inicializar OV2640, usar resolución SXGA (1600*1200)
    for (i = 0; i < sizeof(ov2640_uxga_init_reg_tbl) / 2; i++)
    {
        SCCB_WR_Reg(ov2640_uxga_init_reg_tbl[i][0], ov2640_uxga_init_reg_tbl[i][1]);
    }
    printf("OV2640 Initialization Successfully!\r\n");
    return 0x00; // lo bueno
}

// OV2640 switch to JPEG mode
void OV2640_JPEG_Mode(void)
{
    uint16_t i = 0;
    // set to YUV422 format
    for (i = 0; i < (sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
    {
        SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
    }
    // set to output JPEG data
    for (i = 0; i < (sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
    {
        SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);
    }
}

// OV2640 switch to RGB565 mode
void OV2640_RGB565_Mode(void)
{
    uint16_t i = 0;
    // Set RGB565 output
    for (i = 0; i < (sizeof(ov2640_rgb565_reg_tbl) / 2); i++)
    {
        SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0], ov2640_rgb565_reg_tbl[i][1]);
    }
}

// Automatic exposure setting parameter table which supports 5 levels
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL[5][8] = {

    {

        0xFF,
        0x01,
        0x24,
        0x20,
        0x25,
        0x18,
        0x26,
        0x60,
    },
    {

        0xFF,
        0x01,
        0x24,
        0x34,
        0x25,
        0x1c,
        0x26,
        0x00,
    },
    {

        0xFF,
        0x01,
        0x24,
        0x3e,
        0x25,
        0x38,
        0x26,
        0x81,
    },
    {

        0xFF,
        0x01,
        0x24,
        0x48,
        0x25,
        0x40,
        0x26,
        0x81,
    },
    {

        0xFF,
        0x01,
        0x24,
        0x58,
        0x25,
        0x50,
        0x26,
        0x92,
    },
};

void OV2640_Auto_Exposure(uint8_t level)
{
    uint8_t i;
    uint8_t *p = (uint8_t *)OV2640_AUTOEXPOSURE_LEVEL[level];
    for (i = 0; i < 4; i++)
    {
        SCCB_WR_Reg(p[i * 2], p[i * 2 + 1]);
    }
}

// White balance setting
void OV2640_Light_Mode(uint8_t mode)
{
    uint8_t regccval = 0X5E; // Sunny
    uint8_t regcdval = 0X41;
    uint8_t regceval = 0X54;
    switch (mode)
    {
    case 0: // auto
        SCCB_WR_Reg(0XFF, 0X00);
        SCCB_WR_Reg(0XC7, 0X00); // AWB ON
        return;
    case 2: // cloudy
        regccval = 0X65;
        regcdval = 0X41;
        regceval = 0X4F;
        break;
    case 3: // office
        regccval = 0X52;
        regcdval = 0X41;
        regceval = 0X66;
        break;
    case 4: // home
        regccval = 0X42;
        regcdval = 0X3F;
        regceval = 0X71;
        break;
    }
    SCCB_WR_Reg(0XFF, 0X00);
    SCCB_WR_Reg(0XC7, 0X40); // AWB OFF
    SCCB_WR_Reg(0XCC, regccval);
    SCCB_WR_Reg(0XCD, regcdval);
    SCCB_WR_Reg(0XCE, regceval);
}

// Chroma settings
// 0:-2
// 1:-1
// 2,0
// 3,+1
// 4,+2
void OV2640_Color_Saturation(uint8_t sat)
{
    uint8_t reg7dval = ((sat + 2) << 4) | 0X08;
    SCCB_WR_Reg(0XFF, 0X00);
    SCCB_WR_Reg(0X7C, 0X00);
    SCCB_WR_Reg(0X7D, 0X02);
    SCCB_WR_Reg(0X7C, 0X03);
    SCCB_WR_Reg(0X7D, reg7dval);
    SCCB_WR_Reg(0X7D, reg7dval);
}
// Brightness setting
// 0:(0X00)-2
// 1:(0X10)-1
// 2,(0X20) 0
// 3,(0X30)+1
// 4,(0X40)+2
void OV2640_Brightness(uint8_t bright)
{
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x04);
    SCCB_WR_Reg(0x7c, 0x09);
    SCCB_WR_Reg(0x7d, bright << 4);
    SCCB_WR_Reg(0x7d, 0x00);
}
// Contrast setting
// 0:-2
// 1:-1
// 2,0
// 3,+1
// 4,+2
void OV2640_Contrast(uint8_t contrast)
{
    uint8_t reg7d0val = 0X20; // normal mode is the default configuration
    uint8_t reg7d1val = 0X20;
    switch (contrast)
    {
    case 0: //-2
        reg7d0val = 0X18;
        reg7d1val = 0X34;
        break;
    case 1: //-1
        reg7d0val = 0X1C;
        reg7d1val = 0X2A;
        break;
    case 3: // 1
        reg7d0val = 0X24;
        reg7d1val = 0X16;
        break;
    case 4: // 2
        reg7d0val = 0X28;
        reg7d1val = 0X0C;
        break;
    }
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, 0x04);
    SCCB_WR_Reg(0x7c, 0x07);
    SCCB_WR_Reg(0x7d, 0x20);
    SCCB_WR_Reg(0x7d, reg7d0val);
    SCCB_WR_Reg(0x7d, reg7d1val);
    SCCB_WR_Reg(0x7d, 0x06);
}

// Effects settings
void OV2640_Special_Effects(uint8_t eft)
{
    uint8_t reg7d0val = 0X00; // normal mode is the default configuration
    uint8_t reg7d1val = 0X80;
    uint8_t reg7d2val = 0X80;
    switch (eft)
    {
    case 1: // negative film picture
        reg7d0val = 0X40;
        break;
    case 2: // black and white
        reg7d0val = 0X18;
        break;
    case 3: // Reddish
        reg7d0val = 0X18;
        reg7d1val = 0X40;
        reg7d2val = 0XC0;
        break;
    case 4: // Greenish
        reg7d0val = 0X18;
        reg7d1val = 0X40;
        reg7d2val = 0X40;
        break;
    case 5: // Blueish
        reg7d0val = 0X18;
        reg7d1val = 0XA0;
        reg7d2val = 0X40;
        break;
    case 6: // Retro
        reg7d0val = 0X18;
        reg7d1val = 0X40;
        reg7d2val = 0XA6;
        break;
    }
    SCCB_WR_Reg(0xff, 0x00);
    SCCB_WR_Reg(0x7c, 0x00);
    SCCB_WR_Reg(0x7d, reg7d0val);
    SCCB_WR_Reg(0x7c, 0x05);
    SCCB_WR_Reg(0x7d, reg7d1val);
    SCCB_WR_Reg(0x7d, reg7d2val);
}

/*Color bar test
sw:0,disable colorbar
   1,enable colorbar(Tenga en cuenta que la barra de color del OV2640 se superpone en la parte superior de la imagen)*/
void OV2640_Color_Bar(uint8_t sw)
{
    uint8_t reg;
    SCCB_WR_Reg(0XFF, 0X01);
    reg = SCCB_RD_Reg(0X12);
    reg &= ~(1 << 1);
    if (sw)
        reg |= 1 << 1;
    SCCB_WR_Reg(0X12, reg);
}

/*Set the sensor output window size
sx,sy:start point coordinates
width,h8:(horizontal)and(vertical)*/
void OV2640_Window_Set(uint16_t sx, uint16_t sy, uint16_t width, uint16_t h8)
{
    uint16_t endx;
    uint16_t endy;
    uint8_t temp;
    endx = sx + width / 2; // V*2
    endy = sy + h8 / 2;
    SCCB_WR_Reg(0XFF, 0X01);
    temp = SCCB_RD_Reg(0X03); // Read the value before Vref
    temp &= 0XF0;
    temp |= ((endy & 0X03) << 2) | (sy & 0X03);
    SCCB_WR_Reg(0X03, temp);      // Set the lowest 2 bits of start and end of Vref
    SCCB_WR_Reg(0X19, sy >> 2);   // Set the high 8 bits of Vref's start
    SCCB_WR_Reg(0X1A, endy >> 2); // Set the high 8 bits of Vref's end
    temp = SCCB_RD_Reg(0X32);     // Read the value before Href
    temp &= 0XC0;
    temp |= ((endx & 0X07) << 3) | (sx & 0X07);
    SCCB_WR_Reg(0X32, temp);      // Set the lowest 3 bits of start and end of Href
    SCCB_WR_Reg(0X17, sx >> 3);   // Set the high 8 bits of Href's start
    SCCB_WR_Reg(0X18, endx >> 3); // Set the high 8 bits of Href's end
}

// Set the output image size (i.e. resolution) of the OV2640
// width and h8 must be a multiple of 4
// return 0 for success and otherwise for error
uint8_t OV2640_OutSize_Set(uint16_t width, uint16_t h8)
{
    uint16_t outh;
    uint16_t outw;
    uint8_t temp;
    if (width % 4)
        return 1;
    if (h8 % 4)
        return 2;
    outw = width / 4;
    outh = h8 / 4;
    SCCB_WR_Reg(0XFF, 0X00);
    SCCB_WR_Reg(0XE0, 0X04);
    SCCB_WR_Reg(0X5A, outw & 0XFF); // Set the lower 8 bits of OUTW
    SCCB_WR_Reg(0X5B, outh & 0XFF); // Set the lower 8 bits of OUTH
    temp = (outw >> 8) & 0X03;
    temp |= (outh >> 6) & 0X04;
    SCCB_WR_Reg(0X5C, temp); // Set the high 8 bits of OUTH/OUTW
    SCCB_WR_Reg(0XE0, 0X00);
    return 0;
}

uint8_t OV2640_ImageWin_Set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t h8)
{
    uint16_t hsize;
    uint16_t vsize;
    uint8_t temp;
    if (width % 4)
        return 1;
    if (h8 % 4)
        return 2;
    hsize = width / 4;
    vsize = h8 / 4;
    SCCB_WR_Reg(0XFF, 0X00);
    SCCB_WR_Reg(0XE0, 0X04);
    SCCB_WR_Reg(0X51, hsize & 0XFF); // Set the lower 8 bits of H_SIZE
    SCCB_WR_Reg(0X52, vsize & 0XFF); // Set the lower 8 bits of V_SIZE
    SCCB_WR_Reg(0X53, offx & 0XFF);  // Set the lower 8 bits of offx
    SCCB_WR_Reg(0X54, offy & 0XFF);  // Set the lower 8 bits of offy
    temp = (vsize >> 1) & 0X80;
    temp |= (offy >> 4) & 0X70;
    temp |= (hsize >> 5) & 0X08;
    temp |= (offx >> 8) & 0X07;
    SCCB_WR_Reg(0X55, temp);                // Set the high bits of H_SIZE/V_SIZE/OFFX,OFFY
    SCCB_WR_Reg(0X57, (hsize >> 2) & 0X80); // Set the high bits of H_SIZE/V_SIZE/OFFX,OFFY
    SCCB_WR_Reg(0XE0, 0X00);
    return 0;
}

uint8_t OV2640_ImageSize_Set(uint16_t width, uint16_t h8)
{
    uint8_t temp;
    SCCB_WR_Reg(0XFF, 0X00);
    SCCB_WR_Reg(0XE0, 0X04);
    SCCB_WR_Reg(0XC0, (width) >> 3 & 0XFF); // Set bits[10:3] of HSIZE
    SCCB_WR_Reg(0XC1, (h8) >> 3 & 0XFF);    // Set bits[10:3] of VSIZE
    temp = (width & 0X07) << 3;
    temp |= h8 & 0X07;
    temp |= (width >> 4) & 0X80;
    SCCB_WR_Reg(0X8C, temp);
    SCCB_WR_Reg(0XE0, 0X00);
    return 0;
}
