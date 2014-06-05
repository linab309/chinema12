#ifndef _PIN_CONFIG_H
#define _PIN_CONFIG_H

/*
 *PIN config
 */
#define BIT_0	0x00000001L
#define BIT_1	0x00000002L
#define BIT_2	0x00000004L
#define BIT_3	0x00000008L
#define BIT_4	0x00000010L
#define BIT_5	0x00000020L
#define BIT_6	0x00000040L
#define BIT_7	0x00000080L
#define BIT_8	0x00000100L
#define BIT_9	0x00000200L
#define BIT_10	0x00000400L
#define BIT_11	0x00000800L
#define BIT_12	0x00001000L
#define BIT_13	0x00002000L
#define BIT_14	0x00004000L
#define BIT_15	0x00008000L
#define BIT_16	0x00010000L
#define BIT_17	0x00020000L
#define BIT_18	0x00040000L
#define BIT_19	0x00080000L
#define BIT_20	0x00100000L
#define BIT_21	0x00200000L
#define BIT_22	0x00400000L
#define BIT_23	0x00800000L
#define BIT_24	0x01000000L
#define BIT_25	0x02000000L
#define BIT_26	0x04000000L
#define BIT_27	0x08000000L
#define BIT_28	0x10000000L
#define BIT_29	0x20000000L
#define BIT_30	0x40000000L
#define BIT_31	0x80000000L 

#define PIN_CFG_0	0x00000000L
#define PIN_CFG_1	0x00000001L
#define PIN_CFG_2	0x00000002L
#define PIN_CFG_3	0x00000003L

#define BITS_P0_0	0
#define BITS_P0_1	2
#define BITS_P0_2	4
#define BITS_P0_3	6
#define BITS_P0_4	8
#define BITS_P0_5	10
#define BITS_P0_6	12
#define BITS_P0_7	14
#define BITS_P0_8	16
#define BITS_P0_9	18
#define BITS_P0_10	20
#define BITS_P0_11	22
#define BITS_P0_12	24
#define BITS_P0_13	26
#define BITS_P0_14	28
#define BITS_P0_15	30
#define BITS_P0_16	0
#define BITS_P0_17	2
#define BITS_P0_18	4
#define BITS_P0_19	6
#define BITS_P0_20	8
#define BITS_P0_21	10
#define BITS_P0_22	12
#define BITS_P0_23	14
#define BITS_P0_24	16
#define BITS_P0_25	18
#define BITS_P0_26	20
#define BITS_P0_27	22
#define BITS_P0_28	24
#define BITS_P0_29	26
#define BITS_P0_30	28
#define BITS_P0_31	30

#if 0
#define P0_0_GPIO (PIN_CFG_0 << BITS_P0_0)
#define P0_0_TXD0 (PIN_CFG_1 << BITS_P0_0)
#define P0_0_PWM1 (PIN_CFG_2 << BITS_P0_0)

#define P0_1_GPIO (PIN_CFG_0 << BITS_P0_1)
#define P0_1_RXD0 (PIN_CFG_1 << BITS_P0_1)
#define P0_1_PWM3 (PIN_CFG_2 << BITS_P0_1)
#define P0_1_EINT0 (PIN_CFG_3 << BITS_P0_1)

#define P0_2_GPIO (PIN_CFG_0 << BITS_P0_2)
#define P0_2_SCL0 (PIN_CFG_1 << BITS_P0_2)
#define P0_2_CAPTURE0_0 (PIN_CFG_2 << BITS_P0_2)

#define P0_3_GPIO (PIN_CFG_0 << BITS_P0_3)
#define P0_3_SDA0 (PIN_CFG_1 << BITS_P0_3)
#define P0_3_MATCH0_0 (PIN_CFG_2 << BITS_P0_3)
#define P0_3_EINT1 (PIN_CFG_3 << BITS_P0_3)

#define P0_4_GPIO (PIN_CFG_0 << BITS_P0_4)
#define P0_4_SCK0 (PIN_CFG_1 << BITS_P0_4)
#define P0_4_CAPTURE0_1 (PIN_CFG_2 << BITS_P0_4)
#define P0_4_AD0_6 (PIN_CFG_3 << BITS_P0_4)

#define P0_5_GPIO (PIN_CFG_0 << BITS_P0_5)
#define P0_5_MISO0 (PIN_CFG_1 << BITS_P0_5)
#define P0_5_MATCH0_1 (PIN_CFG_2 << BITS_P0_5)
#define P0_5_AD0_7 (PIN_CFG_3 << BITS_P0_5)

#define P0_6_GPIO (PIN_CFG_0 << BITS_P0_6)
#define P0_6_MOSI0 (PIN_CFG_1 << BITS_P0_6)
#define P0_6_CAPTURE0_2 (PIN_CFG_2 << BITS_P0_6)
#define P0_6_AD1_0 (PIN_CFG_3 << 12)	//LPC2138

#define P0_7_GPIO (PIN_CFG_0 << BITS_P0_7)
#define P0_7_SSEL0 (PIN_CFG_1 << BITS_P0_7)
#define P0_7_PWM2 (PIN_CFG_2 << BITS_P0_7)
#define P0_7_EINT2 (PIN_CFG_3 << BITS_P0_7)

#define P0_8_GPIO (PIN_CFG_0 << BITS_P0_8)
#define P0_8_TXD1 (PIN_CFG_1 << BITS_P0_8)
#define P0_8_PWM4 (PIN_CFG_2 << BITS_P0_8)
#define P0_8_AD1_1 (PIN_CFG_3 << BITS_P0_8)	//LPC2138

#define P0_9_GPIO (PIN_CFG_0 << BITS_P0_9)
#define P0_9_RXD1 (PIN_CFG_1 << BITS_P0_9)
#define P0_9_PWM6 (PIN_CFG_2 << BITS_P0_9)
#define P0_9_EINT3 (PIN_CFG_3 << BITS_P0_9)

#define P0_10_GPIO (PIN_CFG_0 << BITS_P0_10)
#define P0_10_RTS1 (PIN_CFG_1 << BITS_P0_10)	//LPC2138
#define P0_10_CAPTURE1_0 (PIN_CFG_2 << BITS_P0_10)
#define P0_10_AD1_2 (PIN_CFG_3 << BITS_P0_10)	//LPC2138

#define P0_11_GPIO (PIN_CFG_0 << BITS_P0_11)
#define P0_11_CTS1 (PIN_CFG_1 << BITS_P0_11)	//LPC2138
#define P0_11_CAPTURE1_1 (PIN_CFG_2 << BITS_P0_11)
#define P0_11_SCL1 (PIN_CFG_3 << BITS_P0_11)

#define P0_12_GPIO (PIN_CFG_0 << BITS_P0_12)
#define P0_12_DSR1 (PIN_CFG_1 << BITS_P0_12)	//LPC2138
#define P0_12_MATCH1_0 (PIN_CFG_2 << BITS_P0_12)
#define P0_12_AD1_3 (PIN_CFG_3 << BITS_P0_12)	//LPC2138

#define P0_13_GPIO (PIN_CFG_0 << BITS_P0_13)
#define P0_13_DTR1 (PIN_CFG_1 << BITS_P0_13)	//LPC2138
#define P0_13_MATCH1_1 (PIN_CFG_2 << BITS_P0_13)
#define P0_13_AD1_4 (PIN_CFG_3 << BITS_P0_13)	//LPC2138

#define P0_14_GPIO (PIN_CFG_0 << BITS_P0_14)
#define P0_14_CD1 (PIN_CFG_1 << BITS_P0_14)	//LPC2138
#define P0_14_EINT1 (PIN_CFG_2 << BITS_P0_14)
#define P0_14_SDA1 (PIN_CFG_3 << BITS_P0_14)

#define P0_15_GPIO (PIN_CFG_0 << BITS_P0_15)
#define P0_15_RI1 (PIN_CFG_1 << BITS_P0_15)	//LPC2138
#define P0_15_EINT2 (PIN_CFG_2 << BITS_P0_15)
#define P0_15_AD1_5 (PIN_CFG_3 << BITS_P0_15)	//LPC2138

#define P0_16_GPIO (PIN_CFG_0 << BITS_P0_16)
#define P0_16_EINT0 (PIN_CFG_1 << BITS_P0_16)
#define P0_16_MATCH0_2 (PIN_CFG_2 << BITS_P0_16)
#define P0_16_CAPTURE0_2 (PIN_CFG_3 << BITS_P0_16)
 
#define P0_17_GPIO (PIN_CFG_0 << BITS_P0_17)
#define P0_17_CAPTURE1_2 (PIN_CFG_1 << BITS_P0_17)
#define P0_17_SCK1 (PIN_CFG_2 << BITS_P0_17)
#define P0_17_MATCH1_2 (PIN_CFG_3 << BITS_P0_17)
 
#define P0_18_GPIO (PIN_CFG_0 << BITS_P0_18)
#define P0_18_CAPTURE1_3 (PIN_CFG_1 << BITS_P0_18)
#define P0_18_MISO1 (PIN_CFG_2 << BITS_P0_18)
#define P0_18_MATCH1_3 (PIN_CFG_3 << BITS_P0_18)
 
#define P0_19_GPIO (PIN_CFG_0 << BITS_P0_19)
#define P0_19_MATCH1_2 (PIN_CFG_1 << BITS_P0_19)
#define P0_19_MOSI1 (PIN_CFG_2 << BITS_P0_19)
#define P0_19_MATCH1_3 (PIN_CFG_3 << BITS_P0_19)
 
#define P0_20_GPIO (PIN_CFG_0 << BITS_P0_20)
#define P0_20_MATCH1_3 (PIN_CFG_1 << BITS_P0_20)
#define P0_20_SSEL1 (PIN_CFG_2 << BITS_P0_20)
#define P0_20_EINT3 (PIN_CFG_3 << BITS_P0_20)

#define P0_21_GPIO (PIN_CFG_0 << BITS_P0_21)
#define P0_21_PWM5 (PIN_CFG_1 << BITS_P0_21)
#define P0_21_AD1_6 (PIN_CFG_2 << BITS_P0_21)	//LPC2138
#define P0_21_CAPTURE1_3 (PIN_CFG_3 << BITS_P0_21)

#define P0_22_GPIO (PIN_CFG_0 << BITS_P0_22)
#define P0_22_AD1_7 (PIN_CFG_1 << BITS_P0_22)	//LPC2138
#define P0_22_CAPTURE0_0 (PIN_CFG_2 << BITS_P0_22)
#define P0_22_MATCH0_0 (PIN_CFG_3 << BITS_P0_22)

#define P0_23_GPIO (PIN_CFG_0 << BITS_P0_23)

#define P0_25_GPIO (PIN_CFG_0 << BITS_P0_25)
#define P0_25_AD0_4 (PIN_CFG_1 << BITS_P0_25)
#define P0_25_AOUT (PIN_CFG_2 << BITS_P0_25)

#define P0_27_GPIO (PIN_CFG_0 << BITS_P0_27)
#define P0_27_AD0_0 (PIN_CFG_1 << BITS_P0_27)
#define P0_27_CAPTURE0_1 (PIN_CFG_2 << BITS_P0_27)
#define P0_27_MATCH0_1 (PIN_CFG_3 << BITS_P0_27)

#define P0_28_GPIO (PIN_CFG_0 << BITS_P0_28)
#define P0_28_AD0_1 (PIN_CFG_1 << BITS_P0_28)
#define P0_28_CAPTURE0_2 (PIN_CFG_2 << BITS_P0_28)
#define P0_28_MATCH0_2 (PIN_CFG_3 << BITS_P0_28)

#define P0_29_GPIO (PIN_CFG_0 << BITS_P0_29)
#define P0_29_AD0_2 (PIN_CFG_1 << BITS_P0_29)
#define P0_29_CAPTURE0_3 (PIN_CFG_2 << BITS_P0_29)
#define P0_29_MATCH0_3 (PIN_CFG_3 << BITS_P0_29)
#endif

/*lpc23xx*/
#define P0_0_GPIO (PIN_CFG_0 << BITS_P0_0)
#define P0_0_RD1   (PIN_CFG_1 << BITS_P0_0)
#define P0_0_TXD3 (PIN_CFG_2 << BITS_P0_0)
#define P0_0_SDA1 (PIN_CFG_3 << BITS_P0_0)

#define P0_1_GPIO (PIN_CFG_0 << BITS_P0_1)
#define P0_1_TD1   (PIN_CFG_1 << BITS_P0_1)
#define P0_1_RXD3 (PIN_CFG_2 << BITS_P0_1)
#define P0_1_SCL1 (PIN_CFG_3 << BITS_P0_1)

#define P0_2_GPIO (PIN_CFG_0 << BITS_P0_2)
#define P0_2_TXD0 (PIN_CFG_1 << BITS_P0_2)

#define P0_3_GPIO (PIN_CFG_0 << BITS_P0_3)
#define P0_3_RXD0 (PIN_CFG_1 << BITS_P0_3)

#define P0_4_GPIO (PIN_CFG_0 << BITS_P0_4)
#define P0_4_I2SRX_CLK   (PIN_CFG_1 << BITS_P0_4)
#define P0_4_RD2 (PIN_CFG_2 << BITS_P0_4)
#define P0_4_CAP20 (PIN_CFG_3 << BITS_P0_4)

#define P0_5_GPIO (PIN_CFG_0 << BITS_P0_5)
#define P0_5_I2SRX_WS   (PIN_CFG_1 << BITS_P0_5)
#define P0_5_TD2    (PIN_CFG_2 << BITS_P0_5)
#define P0_5_CAP21 (PIN_CFG_3 << BITS_P0_5)

#define P0_6_GPIO (PIN_CFG_0 << BITS_P0_6)
#define P0_6_I2SRX_SDA   (PIN_CFG_1 << BITS_P0_6)
#define P0_6_SSEL1 (PIN_CFG_2 << BITS_P0_6)
#define P0_6_MAT20 (PIN_CFG_3 << BITS_P0_6)

#define P0_7_GPIO (PIN_CFG_0 << BITS_P0_7)
#define P0_7_I2STX_CLK   (PIN_CFG_1 << BITS_P0_7)
#define P0_7_SCK1 (PIN_CFG_2 << BITS_P0_7)
#define P0_7_MAT21 (PIN_CFG_3 << BITS_P0_7)

#define P0_8_GPIO (PIN_CFG_0 << BITS_P0_8)
#define P0_8_I2STX_WS   (PIN_CFG_1 << BITS_P0_8)
#define P0_8_MISO1 (PIN_CFG_2 << BITS_P0_8)
#define P0_8_MAT22 (PIN_CFG_3 << BITS_P0_8)

#define P0_9_GPIO (PIN_CFG_0 << BITS_P0_9)
#define P0_9_I2STX_SDA   (PIN_CFG_1 << BITS_P0_9)
#define P0_9_MOSI1 (PIN_CFG_2 << BITS_P0_9)
#define P0_9_MAT23 (PIN_CFG_3 << BITS_P0_9)

#define P0_10_GPIO (PIN_CFG_0 << BITS_P0_10)
#define P0_10_TXD2   (PIN_CFG_1 << BITS_P0_10)
#define P0_10_SDA2 (PIN_CFG_2 << BITS_P0_10)
#define P0_10_MAT30 (PIN_CFG_3 << BITS_P0_10)

#define P0_11_GPIO (PIN_CFG_0 << BITS_P0_11)
#define P0_11_RXD2   (PIN_CFG_1 << BITS_P0_11)
#define P0_11_SCL2 (PIN_CFG_2 << BITS_P0_11)
#define P0_11_MAT31 (PIN_CFG_3 << BITS_P0_11)

#define P0_15_GPIO (PIN_CFG_0 << BITS_P0_15)
#define P0_15_TXD1 (PIN_CFG_1 << BITS_P0_15)
#define P0_15_SCK0 (PIN_CFG_2 << BITS_P0_15)
#define P0_15_SCK   (PIN_CFG_3 << BITS_P0_15)

#define P0_16_GPIO (PIN_CFG_0 << BITS_P0_16)
#define P0_16_RXD1   (PIN_CFG_1 << BITS_P0_16)
#define P0_16_SSEL0 (PIN_CFG_2 << BITS_P0_16)
#define P0_16_SSEL (PIN_CFG_3 << BITS_P0_16)

#define P0_17_GPIO (PIN_CFG_0 << BITS_P0_17)
#define P0_17_CTS1   (PIN_CFG_1 << BITS_P0_17)
#define P0_17_MISO0 (PIN_CFG_2 << BITS_P0_17)
#define P0_17_MISO (PIN_CFG_3 << BITS_P0_17)

#define P0_18_GPIO (PIN_CFG_0 << BITS_P0_18)
#define P0_18_DCD1   (PIN_CFG_1 << BITS_P0_18)
#define P0_18_MOSI0 (PIN_CFG_2 << BITS_P0_18)
#define P0_18_MOSI (PIN_CFG_3 << BITS_P0_18)

#define P0_19_GPIO (PIN_CFG_0 << BITS_P0_19)
#define P0_19_DSR1   (PIN_CFG_1 << BITS_P0_19)
#define P0_19_MCLCLK (PIN_CFG_2 << BITS_P0_19)
#define P0_19_SDA1 (PIN_CFG_3 << BITS_P0_19)

#define P0_20_GPIO (PIN_CFG_0 << BITS_P0_20)
#define P0_20_DTR1   (PIN_CFG_1 << BITS_P0_20)
#define P0_20_MCICMD (PIN_CFG_2 << BITS_P0_20)
#define P0_20_SCL1 (PIN_CFG_3 << BITS_P0_20)

#define P0_21_GPIO (PIN_CFG_0 << BITS_P0_21)
#define P0_21_RI1   (PIN_CFG_1 << BITS_P0_21)
#define P0_21_MCIPWR (PIN_CFG_2 << BITS_P0_21)
#define P0_21_RD1 (PIN_CFG_3 << BITS_P0_21)

#define P0_22_GPIO (PIN_CFG_0 << BITS_P0_22)
#define P0_22_RTS1   (PIN_CFG_1 << BITS_P0_22)
#define P0_22_MCIDAT0 (PIN_CFG_2 << BITS_P0_22)
#define P0_22_TD1 (PIN_CFG_3 << BITS_P0_22)

#define P0_23_GPIO (PIN_CFG_0 << BITS_P0_23)
#define P0_23_AD00   (PIN_CFG_1 << BITS_P0_23)
#define P0_23_I2SRX_CLK (PIN_CFG_2 << BITS_P0_23)
#define P0_23_CAP30 (PIN_CFG_3 << BITS_P0_23)

#define P0_24_GPIO (PIN_CFG_0 << BITS_P0_24)
#define P0_24_AD01   (PIN_CFG_1 << BITS_P0_24)
#define P0_24_I2SRX_WS (PIN_CFG_2 << BITS_P0_24)
#define P0_24_CAP31 (PIN_CFG_3 << BITS_P0_24)

#define P0_25_GPIO (PIN_CFG_0 << BITS_P0_25)
#define P0_25_AD02   (PIN_CFG_1 << BITS_P0_25)
#define P0_25_IWSRX_SDA (PIN_CFG_2 << BITS_P0_25)
#define P0_25_TXD3 (PIN_CFG_3 << BITS_P0_25)

#define P0_26_GPIO (PIN_CFG_0 << BITS_P0_26)
#define P0_26_AD03   (PIN_CFG_1 << BITS_P0_26)
#define P0_26_AOUT (PIN_CFG_2 << BITS_P0_26)
#define P0_26_RXD3 (PIN_CFG_3 << BITS_P0_26)

#define P0_27_GPIO (PIN_CFG_0 << BITS_P0_27)
#define P0_27_SDA0   (PIN_CFG_1 << BITS_P0_27)

#define P0_28_GPIO (PIN_CFG_0 << BITS_P0_28)
#define P0_28_SCL0   (PIN_CFG_1 << BITS_P0_28)

#define P0_29_GPIO (PIN_CFG_0 << BITS_P0_29)
#define P0_29_U1D_ADD   (PIN_CFG_1 << BITS_P0_29)

#define P0_30_GPIO (PIN_CFG_0 << BITS_P0_30)
#define P0_30_U1D_REDUCE   (PIN_CFG_1 << BITS_P0_30)


#define P1_0_GPIO (PIN_CFG_0 << BITS_P0_0)         
#define P1_0_ENET_TXD0   (PIN_CFG_1 << BITS_P0_0)        
                                                   
#define P1_1_GPIO (PIN_CFG_0 << BITS_P0_1)         
#define P1_1_ENET_TXD1   (PIN_CFG_1 << BITS_P0_1)                                             
                                                   
#define P1_4_GPIO (PIN_CFG_0 << BITS_P0_4)         
#define P1_4_ENET_TX_EN   (PIN_CFG_1 << BITS_P0_4)                                                
                                                   
#define P1_8_GPIO (PIN_CFG_0 << BITS_P0_8)         
#define P1_8_ENET_CRS   (PIN_CFG_1 << BITS_P0_8)   
                                                   
#define P1_9_GPIO (PIN_CFG_0 << BITS_P0_9)         
#define P1_9_ENET_RXD0   (PIN_CFG_1 << BITS_P0_9)    
                                                   
#define P1_10_GPIO (PIN_CFG_0 << BITS_P0_10)       
#define P1_10_ENET_RXD1   (PIN_CFG_1 << BITS_P0_10)     
                                                   
#define P1_14_GPIO (PIN_CFG_0 << BITS_P0_14)       
#define P1_14_ENET_RX_ER   (PIN_CFG_1 << BITS_P0_14)     
                                                   
#define P1_15_GPIO (PIN_CFG_0 << BITS_P0_15)       
#define P1_15_ENET_REF_CLK   (PIN_CFG_1 << BITS_P0_15)     
                                                   
#define P1_16_GPIO (PIN_CFG_0 << BITS_P0_16)       
#define P1_16_ENET_MDC   (PIN_CFG_1 << BITS_P0_16)     
                                                   
#define P1_17_GPIO (PIN_CFG_0 << BITS_P0_17)       
#define P1_17_ENET_MDIO   (PIN_CFG_1 << BITS_P0_17)     
                                                   
#define P1_18_GPIO (PIN_CFG_0 << BITS_P0_18)       
#define P1_18_DCD1   (PIN_CFG_1 << BITS_P0_18)     
#define P1_18_MOSI0 (PIN_CFG_2 << BITS_P0_18)      
#define P1_18_MOSI (PIN_CFG_3 << BITS_P0_18)       
                                                   
#define P1_19_GPIO (PIN_CFG_0 << BITS_P0_19)       
#define P1_19_CAP11   (PIN_CFG_1 << BITS_P0_19)     
                                                   
#define P1_20_GPIO (PIN_CFG_0 << BITS_P0_20)       
#define P1_20_PWM12   (PIN_CFG_1 << BITS_P0_20)     
#define P1_20_SCK0 (PIN_CFG_2 << BITS_P0_20)     
                                                                                                      
#define P1_21_GPIO (PIN_CFG_0 << BITS_P0_21)       
#define P1_21_PWM13   (PIN_CFG_1 << BITS_P0_21)      
#define P1_21_SSEL0 (PIN_CFG_2 << BITS_P0_21)     
                                                   
#define P1_22_GPIO (PIN_CFG_0 << BITS_P0_22)       
#define P1_22_MAT10   (PIN_CFG_1 << BITS_P0_22)     
                                                   
#define P1_23_GPIO (PIN_CFG_0 << BITS_P0_23)       
#define P1_23_PWM14   (PIN_CFG_1 << BITS_P0_23)     
#define P1_23_MISO0 (PIN_CFG_2 << BITS_P0_23)  
                                                   
#define P1_24_GPIO (PIN_CFG_0 << BITS_P0_24)       
#define P1_24_PWM15   (PIN_CFG_1 << BITS_P0_24)     
#define P1_24_MOSI0 (PIN_CFG_2 << BITS_P0_24)   
                                                   
#define P1_25_GPIO (PIN_CFG_0 << BITS_P0_25)       
#define P1_25_MAT11   (PIN_CFG_1 << BITS_P0_25)     
                                                   
#define P1_26_GPIO (PIN_CFG_0 << BITS_P0_26)       
#define P1_26_pwm16   (PIN_CFG_1 << BITS_P0_26)     
#define P1_26_cap00 (PIN_CFG_2 << BITS_P0_26)       
                                                   
#define P1_27_GPIO (PIN_CFG_0 << BITS_P0_27)       
#define P1_27_CAP01   (PIN_CFG_1 << BITS_P0_27)     
                                                   
#define P1_28_GPIO (PIN_CFG_0 << BITS_P0_28)       
#define P1_28_CAP10   (PIN_CFG_1 << BITS_P0_28)     
#define P1_28_MAT10   (PIN_CFG_2 << BITS_P0_28)                                                        

#define P1_29_GPIO (PIN_CFG_0 << BITS_P0_29)       
#define P1_29_PCAP11   (PIN_CFG_1 << BITS_P0_29)  
#define P1_29_MAT01   (PIN_CFG_2 << BITS_P0_29)  
                                                   
#define P1_30_GPIO (PIN_CFG_0 << BITS_P0_30)        
#define P1_30_VBUS   (PIN_CFG_1 << BITS_P0_30)
#define P1_30_AD04   (PIN_CFG_2 << BITS_P0_30)

#define P1_31_GPIO (PIN_CFG_0 << BITS_P0_31)        
#define P1_31_SCK1   (PIN_CFG_1 << BITS_P0_31)
#define P1_31_AD05   (PIN_CFG_2 << BITS_P0_31)


#define P2_0_GPIO (PIN_CFG_0 << BITS_P0_0)         
#define P2_0_PWM11   (PIN_CFG_1 << BITS_P0_0)        
#define P2_0_TXD1 (PIN_CFG_2 << BITS_P0_0)         
#define P2_0_TRACECLK (PIN_CFG_3 << BITS_P0_0)         
                                                      
#define P2_1_GPIO (PIN_CFG_0 << BITS_P0_1)         
#define P2_1_PWM12   (PIN_CFG_1 << BITS_P0_1)        
#define P2_1_RXD1 (PIN_CFG_2 << BITS_P0_1)         
#define P2_1_PIPESTAT0 (PIN_CFG_3 << BITS_P0_1)         
                                                      
#define P2_2_GPIO (PIN_CFG_0 << BITS_P0_2)         
#define P2_2_PWM13   (PIN_CFG_1 << BITS_P0_2)       
#define P2_2_CTS1   (PIN_CFG_2 << BITS_P0_2)       
#define P2_2_PIPESTAT1   (PIN_CFG_3 << BITS_P0_2)       
	  
#define P2_3_GPIO (PIN_CFG_0 << BITS_P0_3)         
#define P2_3_PWM14   (PIN_CFG_1 << BITS_P0_3)       
#define P2_3_DCD1   (PIN_CFG_2 << BITS_P0_3)       
#define P2_3_PIPESTAT2   (PIN_CFG_3 << BITS_P0_3)       
                                                      
#define P2_4_GPIO (PIN_CFG_0 << BITS_P0_4)         
#define P2_4_PWM15   (PIN_CFG_1 << BITS_P0_4)  
#define P2_4_DSR1 (PIN_CFG_2 << BITS_P0_4)          
#define P2_4_TRACESYNC (PIN_CFG_3 << BITS_P0_4)        
                                                      
#define P2_5_GPIO (PIN_CFG_0 << BITS_P0_5)         
#define P2_5_PWM16   (PIN_CFG_1 << BITS_P0_5)   
#define P2_5_DTR1    (PIN_CFG_2 << BITS_P0_5)       
#define P2_5_TRACEPKT0 (PIN_CFG_3 << BITS_P0_5)        
                                                      
#define P2_6_GPIO (PIN_CFG_0 << BITS_P0_6)         
#define P2_6_PCAP10   (PIN_CFG_1 << BITS_P0_6)  
#define P2_6_RI1 (PIN_CFG_2 << BITS_P0_6)        
#define P2_6_TRACEPKT1 (PIN_CFG_3 << BITS_P0_6)        
                                                      
#define P2_7_GPIO (PIN_CFG_0 << BITS_P0_7)         
#define P2_7_RD2   (PIN_CFG_1 << BITS_P0_7)  
#define P2_7_RTS1 (PIN_CFG_2 << BITS_P0_7)         
#define P2_7_TRACEPKT2 (PIN_CFG_3 << BITS_P0_7)        
                                                      
#define P2_8_GPIO (PIN_CFG_0 << BITS_P0_8)         
#define P2_8_TD2   (PIN_CFG_1 << BITS_P0_8)   
#define P2_8_TXD2 (PIN_CFG_2 << BITS_P0_8)        
#define P2_8_TRACEPKT3 (PIN_CFG_3 << BITS_P0_8)        
                                                      
#define P2_9_GPIO (PIN_CFG_0 << BITS_P0_9)         
#define P2_9_U1CONNECT   (PIN_CFG_1 << BITS_P0_9)  
#define P2_9_RXD2 (PIN_CFG_2 << BITS_P0_9)        
#define P2_9_EXTIN0 (PIN_CFG_3 << BITS_P0_9)        
                                                      
#define P2_10_GPIO (PIN_CFG_0 << BITS_P0_10)       
#define P2_10_EINT0   (PIN_CFG_1 << BITS_P0_10)     
                                                      
#define P2_11_GPIO (PIN_CFG_0 << BITS_P0_11)       
#define P2_11_EINT1   (PIN_CFG_1 << BITS_P0_11)     
#define P2_11_MCIDAT1 (PIN_CFG_2 << BITS_P0_11)       
#define P2_11_I2STX_CLK (PIN_CFG_3 << BITS_P0_11)      

#define P2_12_GPIO (PIN_CFG_0 << BITS_P0_12)       
#define P2_12_EINT2   (PIN_CFG_1 << BITS_P0_12)     
#define P2_12_MCIDAT2 (PIN_CFG_2 << BITS_P0_12)       
#define P2_12_I2STX_WS (PIN_CFG_3 << BITS_P0_12)   

#define P2_13_GPIO (PIN_CFG_0 << BITS_P0_13)       
#define P2_13_EINT3   (PIN_CFG_1 << BITS_P0_13)     
#define P2_13_MCIDAT3 (PIN_CFG_2 << BITS_P0_13)       
#define P2_13_I2STX_SDA (PIN_CFG_3 << BITS_P0_13)   


#define P3_25_GPIO (PIN_CFG_0 << BITS_P0_25)       
#define P3_25_MAT00   (PIN_CFG_1 << BITS_P0_25)     
#define P3_25_PWM12 (PIN_CFG_2 << BITS_P0_25)       

#define P3_26_GPIO (PIN_CFG_0 << BITS_P0_26)       
#define P3_26_MAT01   (PIN_CFG_1 << BITS_P0_26)     
#define P3_26_PWM13 (PIN_CFG_2 << BITS_P0_26)       
   
#define P4_28_GPIO (PIN_CFG_0 << BITS_P0_28)       
#define P4_28_MAT20   (PIN_CFG_1 << BITS_P0_28)     
#define P4_28_TXD3 (PIN_CFG_2 << BITS_P0_28)       

#define P4_29_GPIO (PIN_CFG_0 << BITS_P0_29)       
#define P4_29_MAT21   (PIN_CFG_1 << BITS_P0_29)     
#define P4_29_RXD3 (PIN_CFG_2 << BITS_P0_29)  

/*
 *VIC Channel
 *中断通道
 */
#define VIC_CH_WDT      0	//Watchdog Interrupt (WDINT)
#define VIC_CH_TIMER0 4	//Match 0 - 3 (MR0, MR1, MR2, MR3)
						//Capture 0 - 3 (CR0, CR1, CR2, CR3)
#define VIC_CH_TIMER1 5	//Match 0 - 3 (MR0, MR1, MR2, MR3)
						//Capture 0 - 3 (CR0, CR1, CR2, CR3)
#define VIC_CH_UART0  6	//Rx Line Status (RLS)
						//Transmit Holding Register Empty (THRE)
						//Rx Data Available (RDA)
						//Character Time-out Indicator (CTI)
#define VIC_CH_UART1  7	//Rx Line Status (RLS)
						//Transmit Holding Register Empty (THRE)
						//Rx Data Available (RDA)
						//Character Time-out Indicator (CTI)
						//Modem Status Interrupt (MSI)	//LPC2138
#define VIC_CH_PWM0   8	//Match 0 - 6 (MR0, MR1, MR2, MR3, MR4, MR5, MR6)
#define VIC_CH_I2C0     9	//SI (state change)
#define VIC_CH_SPI0     10	//SPI Interrupt Flag (SPIF)
						//Mode Fault (MODF)
#define VIC_CH_SPI1     11	//Tx FIFO at least half empty (TXRIS)
						//Rx FIFO at least half full (RXRIS)
						//Receive Timeout condition (RTRIS)
						//Receive overrun (RORRIS)
#define VIC_CH_PLL       12	//PLL Lock (PLOCK)
#define VIC_CH_RTC      13	//Counter Increment (RTCCIF)
						//Alarm (RTCALF)
#define VIC_CH_EINT0  14	//External Interrupt 0 (EINT0)
#define VIC_CH_EINT1  15	//External Interrupt 1 (EINT1)
#define VIC_CH_EINT2  16	//External Interrupt 2 (EINT2)
#define VIC_CH_EINT3  17	//External Interrupt 3 (EINT3)
#define VIC_CH_AD0     18	//A/D Converter 0
#define VIC_CH_I2C1    19	//SI (state change)
#define VIC_CH_BOD     20	//Brown Out Detect
/*jian remove it in 07-01-10*/
#if 0
#define VIC_CH_AD1 21	//A/D Converter 1	//LPC2138 
#else
/*JIAN ADD IN 07-01-10*/
 #define VIC_CH_ETHERNET   21  	//SWAKE UPINT ,Softint,txdneint,txfinesedint,txerrorint,txunderrunint,rxdoneint,
                                              		//rxfinishedint,rxerrorint,rxoverrunint.
#define VIC_CH_USB              22       // USB_INT_REQ_LP,USB_INT_REQ_HP,USB_INT_REQ_DMA                                          
#define VIC_CH_CAN              23      	//CAN COMMON,CAN 0 TX,CAN 0 RX.CAN 1 TX, CAN 1 RX
#define VIC_CH_SDMMC         24      	///SD/MMC INTERFACE
#define VIC_CH_GP_DMA       25   	//INSTATUS OF DMA CHANNEL 0,INTSTATUS F DMA CHANNEL 1 
#define VIC_CH_TIMER2        26  	//TIMER 2
#define VIC_CH_TIMER3        27  	//TIMER 3
#define VIC_CH_UART2         28    	//UART 2
#define VIC_CH_UART3         29   	// UART3
#define VIC_CH_I2C2            30    	//I2C 2
#define VIC_CH_I2S              31   	//I2S
#endif

/*pconp power control for peripherals register

  if a peripheral control bit is 1 ,that peropheral is endabled if a peripheral bit is 0 ,that peripheral's colck is disable(gated off) to conserve power for example
  if bit 19 is 1,the i2c1 interface if endabled if bit 19 is 0 the i2c1 in terface is disabled
*/
#define PCONP_REGISTER 		 0x00000001
#define PCTIM0  		PCONP_REGISTER<<1
#define PCTIM1  		PCONP_REGISTER<<2
#define PCUART0  	PCONP_REGISTER<<3
#define PCUART1  	PCONP_REGISTER<<4
#define PCPWM0  		PCONP_REGISTER<<5
#define PCPWM1  		PCONP_REGISTER<<6
#define PCI2C0  		PCONP_REGISTER<<7
#define PCSPI  		PCONP_REGISTER<<8
#define PCRTC  		PCONP_REGISTER<<9
#define PCSSP1  		PCONP_REGISTER<<10
#define PCEMC     		PCONP_REGISTER<<11
#define PCAD  		PCONP_REGISTER<<12
#define PCAN1  		PCONP_REGISTER<<13
#define PCAN2  		PCONP_REGISTER<<14
#define PCI2C1  		PCONP_REGISTER<<19
#define PCSSP0  		PCONP_REGISTER<<21
#define PCTIM2  		PCONP_REGISTER<<22
#define PCTIM3  		PCONP_REGISTER<<23
#define PCUART2  	PCONP_REGISTER<<24
#define PCUART3  	PCONP_REGISTER<<25
#define PCI2C2  		PCONP_REGISTER<<26
#define PCI2S  		PCONP_REGISTER<<27
#define PCSDC  		PCONP_REGISTER<<28
#define PCGPDMA  	PCONP_REGISTER<<29
#define PCENET  		PCONP_REGISTER<<30
#define PCUSB 		PCONP_REGISTER<<31 /*JIAN ADD IN 07-01-18   */

/*中断使能位*/
#define VICINTENABLE_BIT		0x00000001L
#define VIC_ENABLE_WDT			(VICINTENABLE_BIT << VIC_CH_WDT)
#define VIC_ENABLE_TIMER0		(VICINTENABLE_BIT << VIC_CH_TIMER0)
#define VIC_ENABLE_TIMER1		(VICINTENABLE_BIT << VIC_CH_TIMER1)
#define VIC_ENABLE_UART0		(VICINTENABLE_BIT << VIC_CH_UART0)
#define VIC_ENABLE_UART1		(VICINTENABLE_BIT << VIC_CH_UART1)
#define VIC_ENABLE_PWM0		(VICINTENABLE_BIT << VIC_CH_PWM0)
#define VIC_ENABLE_I2C0			(VICINTENABLE_BIT << VIC_CH_I2C0)
#define VIC_ENABLE_I2C1			(VICINTENABLE_BIT << VIC_CH_I2C1)
#define VIC_ENABLE_SPI0			(VICINTENABLE_BIT << VIC_CH_SPI0)
#define VIC_ENABLE_SPI1			(VICINTENABLE_BIT << VIC_CH_SPI1)
#define VIC_ENABLE_PLL			(VICINTENABLE_BIT << VIC_CH_PLL)
#define VIC_ENABLE_RTC			(VICINTENABLE_BIT << VIC_CH_RTC)
#define VIC_ENABLE_EINT0		(VICINTENABLE_BIT << VIC_CH_EINT0)
#define VIC_ENABLE_EINT1		(VICINTENABLE_BIT << VIC_CH_EINT1)
#define VIC_ENABLE_EINT2		(VICINTENABLE_BIT << VIC_CH_EINT2)
#define VIC_ENABLE_EINT3		(VICINTENABLE_BIT << VIC_CH_EINT3)
#define VIC_ENABLE_AD0			(VICINTENABLE_BIT << VIC_CH_AD0)
//#define VIC_ENABLE_AD1			(VICINTENABLE_BIT << VIC_CH_AD1)
#define VIC_ENABLE_BOD			(VICINTENABLE_BIT << VIC_CH_BOD)
#if 1
/*JIAN ADD IN 07-01-10**************/
#define VIC_ENABLE_ETHERNET   	(VICINTENABLE_BIT << VIC_CH_ETHERNET)
#define VIC_ENABLE_USB         	(VICINTENABLE_BIT << VIC_CH_USB)
#define VIC_ENABLE_CAN         	(VICINTENABLE_BIT << VIC_CH_CAN)
#define VIC_ENABLE_SDMMC       	(VICINTENABLE_BIT << VIC_CH_SDMMC)
#define VIC_ENABLE_GP_DMA      	(VICINTENABLE_BIT << VIC_CH_GP_DMA)
#define VIC_ENABLE_TIMER2      	(VICINTENABLE_BIT << VIC_CH_TIMER2)
#define VIC_ENABLE_TIMER3      	(VICINTENABLE_BIT << VIC_CH_TIMER3)
#define VIC_ENABLE_UART2       	(VICINTENABLE_BIT << VIC_CH_UART2)
#define VIC_ENABLE_UART3       	(VICINTENABLE_BIT << VIC_CH_UART3)
#define VIC_ENABLE_I2C2        	(VICINTENABLE_BIT << VIC_CH_I2C2)
#define VIC_ENABLE_I2S         		(VICINTENABLE_BIT << VIC_CH_I2S)
/*ENN IN JIAN ADD IN 07-01-10*/
#endif

/*外部中断标志寄存器EXINT*/
#define FLAG_EINT0	0x1
#define FLAG_EINT1	0x2
#define FLAG_EINT2	0x4
#define FLAG_EINT3	0x8

/*jian add in 07-01-15*/
enum 
{
VIC_PRIORITY_0=0x00,
VIC_PRIORITY_1,
VIC_PRIORITY_2,
VIC_PRIORITY_3,
VIC_PRIORITY_4,
VIC_PRIORITY_5,
VIC_PRIORITY_6,
VIC_PRIORITY_7,
VIC_PRIORITY_8,
VIC_PRIORITY_9,
VIC_PRIORITY_10,
VIC_PRIORITY_11,
VIC_PRIORITY_12,
VIC_PRIORITY_13,
VIC_PRIORITY_14,
VIC_PRIORITY_15,
};
/*end in 07-01-15*/
#endif

