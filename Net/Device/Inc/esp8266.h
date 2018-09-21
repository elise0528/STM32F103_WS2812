#ifndef _ESP8266_H_
#define _ESP8266_H_

#define REV_OK 0   //������ɱ�־
#define REV_WAIT 1 //����δ��ɱ�־


extern unsigned char esp8266_buf[128];
extern unsigned short esp8266_cnt;
extern unsigned char esp8266_flag;

void ESP8266_Init(void);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif
