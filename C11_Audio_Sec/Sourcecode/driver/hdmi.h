enum{
ADDR_HDMI_SELECT = 1, 	/*HDMI 端口的地址*/
ADDR_BIT_STREAM, 		/*HDMI 输入流选择的地址*/
ADDR_AUTHENTICATION, 	/*HDMI 是否认证的地址*/
ADDR_SPDIF, 			/*SPDIF 开关选择地址*/
ADDR_HDMI_BUG, 		/*HDMI Bug*/
ADDR_HDMI_AUDIO_OUT=8, /*HDMI AUDIO OUT*/
ADDR_HDMI_ARC_OUT=9, /*HDMI AUDIO OUT*/
ADDR_HDMI_POWER_OFF=10, /*HDMI AUDIO OUT*/
};

enum{
STREAM_AUTO=0,
STREAM_SPDIF , 		/*HDMI 端口的地址*/
STREAM_I2S, 			/*HDMI 输入流选择的地址*/
#if 0
STREAM_DSD, 			/*HDMI 是否认证的地址*/
#endif
};

void HDMI_secret_loop(void); // jian add in 10-09-09
//extern void HDMI_SPDIF_ON(int on);
extern unsigned char HdmiReadByte(unsigned int addr);
extern void HdmiSendByte(unsigned char addr,unsigned char value);
extern void HDMI_Port_Select(int Port_Select);
extern void HDMI_Stream_Select(int stream);
extern void HDMI_Stream_Bug(unsigned char *buffer);
extern void HDMI_DSD_Loop(void);

