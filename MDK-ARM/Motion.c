#include "usart.h"
#include "HMC830.h"


uint8_t temp[6]={0};
void HMC830_Task(){
	
	unsigned  long tmp;
	tmp=RD_data(0x12);
	
		temp[0]=0xFF;
		temp[1]=tmp>>24;
		temp[2]=tmp>>16;
		temp[3]=tmp>>8;
		temp[4]=tmp;
		temp[5]=0xAF;

		HAL_UART_Transmit(&huart1 ,temp,6,10);

}

