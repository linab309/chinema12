#ifndef _IOPORT_H
#define _IOPORT_H

#include "mcudef.h"

#define _USE_DEFINED_IO

extern void SystemReset(void);
extern void HardwareInit(void);
extern void SetIoStandby(void);
extern void SetIoPowerOn(void);
extern void remote_config(void);

extern void SetIoport0(unsigned long pin, int high);
extern int SensePort0(unsigned long pin);
extern void SetIoport1(unsigned long pin, int high);
extern int SensePort1(unsigned long pin);

//#if 1
//#define debug_pin BIT_17	/*P1.17*/
//#endif

#define Mute 			0
#define UnMute 		1

#define HDMI_Input 	1
#define AD_Input		0

/*Remote Mode*/
enum{
IR_MODE_FRONT = 0,
IR_MODE_BACK,
IR_MODE_ALL,
IR_MODE_NONE,
};

/************************************System Reset********************************************/
#define RESET_PIN	BIT_3	/*P2.3*/ 

void InitResetPin(void);
extern void SetResetPin(int high);
/************************************System Reset*********************************************/

/************************************ Pow And PowLed *****************************************/
#define Pow_Pin		BIT_24           /*P0.24*/
#define Pow_Led_Pin 	BIT_25		/*P0.25*/

extern void InitPower(void);
extern void SystemPower(unsigned char mode);
extern void PowerLed(unsigned char mode);
extern void LedStandby(void);
/************************************ Pow And PowLed *****************************************/

/************************************ HD Radio Power *****************************************/
#define HD_Power 	BIT_19		/*1.19*/

extern void InitHDRaidoPower(void);
extern void HDRadioPower(unsigned char mode);
/************************************ HD Radio Power *****************************************/

/*****************************************  165 **********************************************/
#define IC165_LD		BIT_8 		/*P1.8*/
#define IC165_CLK 	BIT_4 		/*P1.4*/
#define IC165_DATA 	BIT_1		/*P1.1*/  

#ifdef _USE_DEFINED_IO
#define set_ic165_ld(high)     SetIoport1(IC165_LD ,high)
#define set_ic165_clk(high)   SetIoport1(IC165_CLK ,high)
#define sense_ic165_data()   SensePort1(IC165_DATA)
#else
extern void set_ic165_ld(int high);
extern void set_ic165_clk(int high);
extern int sense_ic165_data(void);
#endif
/*****************************************  165 **********************************************/

/***************************************  encoder switch  **************************************/
#define ENCODER_SWITCH_PLUS		BIT_26	/*P0.26*/
#define ENCODER_SWITCH_MINUS		BIT_0	/*P1.0*/

extern int SenseEncoderSwitchPlus(void);
extern int SenseEncoderSwitchMinus(void);
/***************************************  encoder switch  **************************************/

/***************************************  DAC  **********************************************/
//#define   DAC_Reset    	BIT_20	/*P0.20 OUT*/
#define	DAC_MD 		BIT_9	/*P0.9 OUT*/
#define	DAC_MC 		BIT_0	/*P2.0 OUT*/

#define	DAC_CS4		BIT_8	/*P0.8*/   /*Zone1 D/A(SBL/SBR)*/
#define	DAC_CS3		BIT_28	/*P4.28*/ /*Zone1 D/A(CEN/SW)*/
#define	DAC_CS2		BIT_29	/*P4.29*/ /*Zone1 D/A(SL/SR)*/
#define	DAC_CS1		BIT_17	/*P1.17*/ /*Zone1 D/A(FL/FR)*/
#define   DAC_CS0		BIT_16	/*P1.16*/ /*Zone2 D/A*/

#define 	CH4_RELAY		BIT_7	/*P0.7(SBR)*/ 
#define   CH3_RELAY		BIT_6	/*P0.6(SW)*/
#define   CH2_RELAY		BIT_5	/*P0.5(SR)*/ 
#define   CH1_RELAY		BIT_4	/*P0.4(FR)*/ 

#ifdef _USE_DEFINED_IO
//#define SetDacReset(high) 	SetIoport0(DAC_Reset,high)
#define SetDacMd(high) 		SetIoport0(DAC_MD ,high)
#define SetDacMc(high) 		SetIoport2(DAC_MC ,high)
#define SetDacCs4(high) 		SetIoport0(DAC_CS4,high)
#define SetDacCs3(high) 		SetIoport4(DAC_CS3,high)
#define SetDacCs2(high) 		SetIoport4(DAC_CS2,high)
#define SetDacCs1(high) 		SetIoport1(DAC_CS1,high)
#define SetDacCs0(high) 		SetIoport1(DAC_CS0,high)
#define SetCH4RELAY(high)	SetIoport0(CH4_RELAY ,high)
#define SetCH3RELAY(high)	SetIoport0(CH3_RELAY ,high)
#define SetCH2RELAY(high)	SetIoport0(CH2_RELAY ,high)
#define SetCH1RELAY(high)	SetIoport0(CH1_RELAY ,high)
#else
//extern void SetDacReset(int high);
extern void SetDacMd(int high);
extern void SetDacMc(int high);  
extern void SetDacCs4(int high);
extern void SetDacCs3(int high);
extern void SetDacCs2(int high);
extern void SetDacCs1(int high);
extern void SetDacCs0(int high);
extern void SetCH4RELAY(int high);
extern void SetCH3RELAY(int high);
extern void SetCH2RELAY(int high);
extern void SetCH1RELAY(int high);
#endif
/***************************************  DAC  *******************************************/

/**************************************  MAS9116  ****************************************/
#define MAS9116_DATA	 BIT_30	/*P0.30 OUT*/
#define MAS9116_CLK 	 BIT_18	/*P1.18 OUT*/
#define Mas9116_Cs1		 BIT_20	/*P1.20*/ /*2 channel Volume*/
#define Mas9116_Cs2		 BIT_25	/*P1.25*/ /*Zone2 Volume*/
#define Mas9116_Cs3		 BIT_24	/*P1.24*/
#define Mas9116_Cs4		 BIT_23	/*P1.23*/
#define Mas9116_Cs5		 BIT_22	/*P1.22*/
#define Mas9116_Cs6		 BIT_21	/*P1.21*/  

#ifdef _USE_DEFINED_IO
#define SetMas9116Clk(high) 		SetIoport1(MAS9116_CLK ,high)
#define SetMas9116Data(high) 	SetIoport0(MAS9116_DATA ,high)
#else
extern void SetMas9116Clk(int high);
extern void SetMas9116Data(int high);
#endif

//extern void SetMas9116Mute(unsigned char mode);
extern void SetMas9116Cs0(unsigned char select);
extern void SetMas9116Cs1(unsigned char select);
extern void SetMas9116Cs2(unsigned char select);
extern void SetMas9116Cs3(unsigned char select);
extern void SetMas9116Cs4(unsigned char select);
extern void SetMas9116Cs5(unsigned char select);
extern void SelectMas9116(unsigned char choose);
extern void InitMas9116(void);
/**************************************  MAS9116  ****************************************/

/**************************************  MAX335  ****************************************/
#define MAX335_DATA	BIT_25	/*P3.25 OUT*/
#define MAX335_CLK		BIT_29	/*P0.29 OUT*/
#define Max335_Cs1		BIT_26	/*p1.26*/
#define Max335_Cs2		BIT_27	/*p1.27*/
#define Max335_Cs3		BIT_28	/*p1.28*/
#define Max335_Cs4		BIT_29	/*p1.29*/
#define Max335_Cs5		BIT_0	/*p0.0*/
#define Max335_Cs6		BIT_1	/*p0.1*/
#define Max335_Cs7		BIT_10	/*p0.10*/
#define Max335_Cs8		BIT_11	/*P0.11*/

extern void SetMax335Data(int high);
extern void SetMax335Clk(int high);
extern void SetMax335Cs1(unsigned char select);
extern void SetMax335Cs2(unsigned char select);
extern void SetMax335Cs3(unsigned char select);
extern void SetMax335Cs4(unsigned char select);
extern void SetMax335Cs5(unsigned char select);
extern void SetMax335Cs6(unsigned char select);
extern void SetMax335Cs7(unsigned char select);
extern void SetMax335Cs8(unsigned char select);
extern void SelectMax335(unsigned char choose);
/**************************************  MAX335  ****************************************/

/**************************************  Mute Control  ************************************/
#define ch71_Mute		BIT_31	/*P1.31*/
#define Zone2_Mute		BIT_23   	/*P0.23*/
#define Balance_Mute		BIT_30	/*P1.30*/
//#define HP_Mute			BIT_26	/*P3.26*/
#define HP_Mute			BIT_28	/*P0.28*/

extern void Set7CH1Mute(unsigned char mode);
extern void SetZone2Mute(unsigned char mode);
extern void SetBalance_Mute(unsigned char mode);
extern void SetHeadphoneMute(unsigned char mode);
extern void Zone2Mute(unsigned char mode);
extern void InitMuteControl(void);
extern void PowerOffMuteControl(void);
/**************************************  Mute Control  ************************************/

/***************************  Input Port Select ********************************************/
#define DSP_Input_Select		BIT_5	/*P2.2*/   /*jian change it to 2.5*/

extern  void DSP_Input_Stream_Select(unsigned char select);
extern void Init_Dsp_Input_Select(void);
/***************************  Input Port Select ********************************************/

/***************************  DSD Select ************************************************/
#define DSD_Selsect			BIT_2	/*P2.2*/

#define HDMI_Selsect			BIT_4	/*P2.4*/

extern void DSD_Select(unsigned char mode);
extern void Init_DSD_Select(void);
/***************************  DSD Select ************************************************/

/********************************* Analog Signal Detect  **********************************/
#define Analog_Signal_Detect 		BIT_22		/*P0.22*/

extern unsigned char SenseAnalogSignal(void);
/********************************* Anolog Signal Detect  **********************************/

/************************************Triger Setting***************************************/
extern void SetTrigerOut(int high);
/************************************Triger Setting***************************************/




/*************CD DSP CHANNLE SELSECT*//////////////////
#define CD_DSP_Selsect			BIT_1	/*P2.1*/

/************************************IR Control******************************************/
extern unsigned char front_enable_zone2_ir;
extern unsigned char front_enable_zone1_ir ;  /*jian add in 12-11-02*/


extern void Zone1SelectIr(unsigned char mode);
extern void Zone2SelectIr(unsigned char mode);
/************************************IR Control******************************************/

/************************************IEC4094*******************************************/
#define CS_4094			BIT_27	/*P0.27 OUT*/
#define OE_4094			BIT_26    /*P3.26 OUT*/
#define CLK_4094			BIT_29	/*P0.29 OUT*/
#define DATA_4094		BIT_25	/*P3.25 OUT*/

extern void Set4094Cs(int high);
extern void Set4094OE(int high);
extern void Set4094Clk(int high);
extern void SetData4094(int high);
extern void Init4094(void);
/************************************IEC4094*******************************************/

/**************************************DSP*********************************************/
//DSP RESET
#define	DSP_RST 		BIT_6	/*P2.6 OUT*/
extern void SetDspReset(int high);
extern void DSPHwReset(unsigned char mode);

//DSP IRQ/BUSY
#define   DSP_IRQ		BIT_18	/*P0.18 IN*/
#define   DSP_BUSY 		BIT_19	/*P0.19 IN*/
extern unsigned char SenseDspIrq(void);
extern unsigned char SenseDspBusy(void);

//DSP SPI
#define SPI_CS			BIT_8	/*P2.8*/
#define SPI_CLK			BIT_17	/*P0.17*/
#define SPI_MOSI			BIT_7	/*P2.7*/	
#define SPI_MISO			BIT_9	/*P2.9*/
extern void Set_Spi_Cs(int high);
extern void Set_Spi_Clk(int high);
extern void Set_Spi_MOSI(int high);
extern unsigned char Sense_Spi_MISO(void);
/**************************************DSP*********************************************/

/************************************** CS8416 I2C*************************************/
#define I2C_SDA 		BIT_5	/*P2.5*/ 
#define I2C_SCL 		BIT_4	/*P2.4*/

extern void set_sda(int high);
extern void set_scl(int high);
extern int sense_sda(void);
extern int sense_scl(void);
/************************************** CS8416  I2C************************************/

/************************************** VFD  I2C***************************************/
#define I2C_SDA_VFD		BIT_9	/*p1.9*/
#define I2C_SCL_VFD		BIT_10   /*p1.10*/

extern void set_vfd_sda(int high);
extern void set_vfd_scl(int high);
extern int sense_vfd_sda(void);
extern int sense_vfd_scl(void);
/************************************** VFD  I2C***************************************/

/**************************************EEPROM I2C***************************************/
//#define I2C_SDA1 	BIT_28	/*P0.28*/
//#define I2C_SCL1 	BIT_27	/*P0.27*/
#define I2C_SDA1 	BIT_14	/*P1.14*/
#define I2C_SCL1 	BIT_15	/*P1.15*/ 

extern void set_sda1(int high);
extern void set_scl1(int high); 
extern int sense_sda1(void);
extern int sense_scl1(void);
/**************************************EEPROM I2C***************************************/

/**************************************LPC922 I2C****************************************/
#define I2C_SDA2	BIT_14	 /*P1.14*/
#define I2C_SCL2		BIT_15     /*P1.15*/

extern void set_sda2(int high);
extern void set_scl2(int high);
extern int sense_sda2(void);
extern int sense_scl2(void);
/**************************************LPC922 I2C****************************************/

/**************************************HDMI I2C******************************************/
#define HDMI_SDA	BIT_20	/*P0.20*/
#define HDMI_SCL	BIT_21	/*P0.21*/

extern void set_sda_hdmi(int high);
extern void set_scl_hdmi(int high);
extern int sense_sda_hdmi(void);
extern int sense_scl_hdmi(void);
/**************************************HDMI I2C******************************************/

extern void DSP_Select(unsigned char mode); /*OFF  hdmi1   ON DSD*/
void cd_dsp_channle_Select(unsigned char mode);



#endif

