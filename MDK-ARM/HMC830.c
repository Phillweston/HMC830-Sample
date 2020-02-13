#include "HMC830.h"
#include "gpio.h"


void _delay_hmc(unsigned char t)
{
	unsigned char i;
	for(i = 0; i < t; i++)
    {
        __NOP();
    }
}

void WR_start()
{
	HMC830_CSN_L;  
	HMC830_MOSI_L;
	HMC830_SCK_L;    //先将三个端口拉低
	
	__NOP();
	HMC830_CSN_H;
  __NOP();
	HMC830_MOSI_L;
	__NOP();     
	HMC830_SCK_H;     /*发送起始信号*/
	__NOP();    /* SCLK to SDI hold time 3ns*/
        
	HMC830_SCK_L;     
	__NOP();    /*SDI to SCLK setup time 3ns*/
}

void WR_stop()
{
	HMC830_SCK_L;	
	__NOP();
	HMC830_SCK_H;
	HMC830_CSN_L;  /*发送接收条件的数据信号*/
	__NOP();
}

void WR_data(unsigned char addr,long dat)
{
	unsigned long dat_temp =0;
	unsigned char dat_index = 0;
	unsigned char addr_index;
	unsigned char addr_temp = 0;
	unsigned char index;

	WR_start();
	addr_temp = addr << 2;
	for( addr_index = 0; addr_index < 6; addr_index++ )
	{
		if((addr_temp<<addr_index)&0x80)
			HMC830_MOSI_H;
		else 
			HMC830_MOSI_L;
		HMC830_SCK_L;
		_delay_hmc(3);
		HMC830_SCK_H;
		_delay_hmc(3);
	}
	dat_temp = dat;
	for(index = 0; index < 3; index++)
	{
		dat_temp = dat_temp <<8;
		for( dat_index = 0; dat_index < 8; dat_index++ )
		{
			if((dat_temp << dat_index) & 0x80000000)
				HMC830_MOSI_H;
			else 
                HMC830_MOSI_L;
			HMC830_SCK_L;
			_delay_hmc(2);
			HMC830_SCK_H;
			_delay_hmc(2);	
		}
	 }
	WR_stop();
}



void RD_start()
{
	HMC830_CSN_L;
	HMC830_MOSI_L;
	HMC830_SCK_L; 

	HMC830_CSN_H;
	HMC830_MOSI_H;
	_delay_hmc(2);
	HMC830_SCK_H;
	_delay_hmc(2);
	HMC830_SCK_L;
	_delay_hmc(2);		
}

void RD_stop()
{
	HMC830_SCK_H;
	_delay_hmc(2);
	HMC830_CSN_L;
	_delay_hmc(2);
	HMC830_SCK_L;		
}

unsigned long RD_data(unsigned char Addr)
{	
	unsigned long   Data;
    unsigned long   data_temp;
    unsigned char   Addr_temp;
	//unsigned char   Addr_index;
	unsigned char   index;
	RD_start();
	Addr_temp =Addr<<2;
		HMC830_SCK_L;//自己改
	//SCK =0;
	for(index=0;index<6;index++ )
	{
		if((Addr_temp<<index) & 0x80)
			HMC830_MOSI_H;
		else 
			HMC830_MOSI_L;
		HMC830_SCK_H;
		_delay_hmc(2);
		HMC830_SCK_L;
		_delay_hmc(2);	
	}
	
	for(index=0;index<24;index++)
	{
		HMC830_SCK_H;
		_delay_hmc(2);
///		data_temp=(unsigned long)LD_SDO;
		data_temp=(unsigned long)HMC830_MISO_GET();;//自己改
		Data = Data|data_temp;
		if(index <23) Data = Data << 1;
		HMC830_SCK_H;
		_delay_hmc(2);
		HMC830_SCK_L;//自己改
	//SCK =0;
		_delay_hmc(2);			
	}
	RD_stop();
	return Data;		
}


void HMC830_2097M(void)//参考40MHz
{
	WR_data(0x01,0x00000002);
	WR_data(0x02,0x00000001);//参考频标预分频寄存器
	WR_data(0x05,0x00001628);//VCO_reg5
	WR_data(0x05,0x000060A0);//VCO_reg4
    WR_data(0x05,0x00000898);//VCO_reg3,单端输出898，差分输出818
	WR_data(0x05,0x0000E090);//VCO_reg2,VCO输出分频为1	
	WR_data(0x05,0x00000000);//VCO_reg0	
    WR_data(0x06,0x00200B4A);//小数模式 Mode B,PFD<50M
    WR_data(0x07,0x0000014D);//锁定窗口设置
	WR_data(0x08,0x00C1BEFF);
    WR_data(0x09,0x00557EFD);//电荷泵电流2.5
	WR_data(0x0A,0x00002046);//
    WR_data(0x0B,0x0007C021);// disable PD DN output
	WR_data(0x0F,0x00000081);//GPO_SPI_RDIV
	WR_data(0x03,0x00000034);//整数部分50
    WR_data(0x04,0x006CCCCD);//小数部分50
}

void HMC830_260M(void)//参考100MHz
{
	WR_data(0x01,0x00000002);
	WR_data(0x02,0x00000005);//参考频标预分频寄存器
	WR_data(0x05,0x00001628);//VCO_reg5
	WR_data(0x05,0x000060A0);//VCO_reg4
    WR_data(0x05,0x00000898);//VCO_reg3,单端输出898，差分输出818
	WR_data(0x05,0x0000E510);//VCO_reg2,fo/10
	WR_data(0x05,0x00000000);//VCO_reg0	
	WR_data(0x06,0x002007CA);//整数模式Mode B,PFD<50M
    WR_data(0x07,0x0000014D);//锁定窗口设置
	WR_data(0x08,0x00C1BEFF);
	WR_data(0x09,0x00557EFD);//电荷泵电流2.5
	WR_data(0x0A,0x00002046);//
    WR_data(0x0B,0x0007C021);// disable PD DN output
	WR_data(0x0F,0x00000081);//GPO_SPI_RDIV
	WR_data(0x03,0x00000082);//整数部分50
}

void HMC829_60M(void)
{
	WR_data(0x01,0x00000002);
	WR_data(0x02,0x00000001);//参考频标预分频寄存器
	WR_data(0x05,0x00001628);//VCO_reg5
	WR_data(0x05,0x000060A0);//VCO_reg4
    WR_data(0x05,0x00000818);//VCO_reg3,单端输出898，差分输出818
    WR_data(0x05,0x00009E10);//VCO_reg2,VCO输出分频为60
	WR_data(0x05,0x00000000);//VCO_reg0	
    WR_data(0x06,0x002007CA);//整数模式Mode B,PFD<50M
    WR_data(0x07,0x0000014D);//锁定窗口设置
	WR_data(0x08,0x00C1BEFF);
    WR_data(0x09,0x00557EFD);//电荷泵电流2.5
	WR_data(0x0A,0x00002046);//
    WR_data(0x0B,0x0007C021);// disable PD DN output
	WR_data(0x0F,0x00000081);//GPO_SPI_RDIV
	WR_data(0x03,0x0000005A);//整数部分50
}


/***********************************************************************
void HMC830_140_340_STEP_2M(void)
1、使用整数模式，参考频率100MHz，PFD=20MHz
2、VCO频率定为2800~3400MHz，输出频率210MHZ为分界点
340*6=3040
140*14=1960
150*14=2100

************************************************************************/
unsigned long Freq830_Integer = 260;//140~340取偶数
//unsigned long []
void HMC830_140_340_STEP_2M(void)//已锁定
{
	WR_data(0x01,0x00000002);//CEN由SPI程序控制
	WR_data(0x02,0x00000005);//R分频器
	WR_data(0x05,0x00001628);//VCO_reg5
	WR_data(0x05,0x000060A0);//VCO_reg4
    WR_data(0x05,0x00000898);//VCO_reg3,单端输出898，差分输出818
	if(Freq830_Integer<=150 & Freq830_Integer>=140)//VCO=2800~3000
    {
        WR_data(0x05,0x0000EA10);//VCO_reg2,fo/20
		Freq830_Integer=Freq830_Integer;
    }
	else if(Freq830_Integer>150 & Freq830_Integer<=300)//VCO=1500~3000
    {
        WR_data(0x05,0x0000E510);//VCO_reg2,fo/10
        Freq830_Integer=Freq830_Integer/2;
    }
	else if(Freq830_Integer>300 & Freq830_Integer<=340)//VCO=1800~2040
	{
		WR_data(0x05,0x0000E510);//VCO_reg2,fo/6
        Freq830_Integer=Freq830_Integer/2;
	}
	WR_data(0x05,0x00000000);//VCO_reg0	
	WR_data(0x06,0x002007CA);//整数模式Mode B,PFD<50M
	WR_data(0x07,0x0000214D);//锁定窗口设置
	WR_data(0x08,0x00C1BEFF);//如果输入参考频率小于200MHz，可以不写
    WR_data(0x09,0x00513264);//CP
	WR_data(0x0A,0x00002046);//
	WR_data(0x0B,0x0007C061);//	
	WR_data(0x0F,0x00000081);//锁定指示输出
	WR_data(0x03,Freq830_Integer);//
}


void HMC829(void)//已锁定
{
	WR_data(0x01,0x00000002);//CEN由SPI程序控制
	WR_data(0x02,0x00000001);//R分频器
	WR_data(0x05,0x00007E28);//VCO_reg5
	WR_data(0x05,0x0000B6A0);//VCO_reg4
    WR_data(0x05,0x00000898);//VCO_reg3,单端输出898，差分输出818	
	WR_data(0x05,0x0000E190);//VCO_reg2,fo/2
	WR_data(0x05,0x00000000);//VCO_reg0	
    WR_data(0x06,0x00030F4A);//小数模式Mode B
	WR_data(0x07,0x0000214D);//锁定窗口设置
	WR_data(0x08,0x00C1BEFF);//如果输入参考频率小于200MHz，可以不写
    WR_data(0x09,0x00513264);//CP
	WR_data(0x0A,0x00002046);//
	WR_data(0x0B,0x0007C061);//	
	WR_data(0x0F,0x00000081);//锁定指示输出
	WR_data(0x03,0x00000068);//
    WR_data(0x04,0x00D99999);//
}


unsigned long Freq_Integer = 210;

//Freq_Integer = Freq_HMC829/
/*使用整数模式，参考频率100MHz，PFD=20MHz
VCO频率定为2800~3400MHz，输出频率210MHZ为分界点
*/
void HMC829_140_340_STEP_2M(void)//已锁定
{
    
	WR_data(0x01,0x00000002);//CEN由SPI程序控制
	WR_data(0x02,0x00000005);//R分频器
	WR_data(0x05,0x00007E28);//VCO_reg5
	WR_data(0x05,0x0000B6A0);//VCO_reg4
    WR_data(0x05,0x00000818);//VCO_reg3,单端输出898，差分输出818	
    if(Freq_Integer<=210 & Freq_Integer>=140)
    {
        WR_data(0x05,0x0000EA10);//VCO_reg2,fo/20
    }
	if(Freq_Integer>210 & Freq_Integer<=340)
    {
        WR_data(0x05,0x0000E510);//VCO_reg2,fo/10
        Freq_Integer=Freq_Integer/2;
    }
	WR_data(0x05,0x00000000);//VCO_reg0	
	WR_data(0x06,0x002007CA);//整数模式Mode B,PFD<50M
	WR_data(0x07,0x0000214D);//锁定窗口设置
	WR_data(0x08,0x00C1BEFF);//如果输入参考频率小于200MHz，可以不写
	WR_data(0x09,0x00513264);//CP
	WR_data(0x0A,0x00002046);//
	WR_data(0x0B,0x0007C061);//	
	WR_data(0x0F,0x00000081);//锁定指示输出
	WR_data(0x03,Freq_Integer);//
}


unsigned char Get_LockStatus(void)
{
	if(HMC830_MISO_GET())
		return 1;
	else
		return 0;
}


uint32_t chipID;
void Init_HMC830(void)
{
	HMC830_CSN_L;
	HMC830_SCK_L;
	HAL_Delay(1);
  HMC830_CSN_H;//OPEN_MODE
	HMC830_SCK_H;
	
	HMC830_2097M();
	HAL_Delay(100);
	chipID=RD_data(0x00);//判断时序是否正确  95198h
}




