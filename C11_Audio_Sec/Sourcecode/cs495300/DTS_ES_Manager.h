typedef enum{	/*AN246DC pdf P3*/
DTS_ES_Matrix_6dot1_OFF=0, 
DTS_ES_Matrix_6dot1_ON,
DTS_ES_Matrix_6dot1_Autodetect,
}Matrix_6dot1_Enable;

extern void DTS_ES_Control(unsigned char Matrix_6dot1,unsigned char Discrete_6dot1_Enalbe);

