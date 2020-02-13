#ifndef _HMC830_H_
#define _HMC830_H_

#include "main.h"

//HMC830


void WR_data(unsigned char Addr,long Data);
unsigned long RD_data(unsigned char Addr);

void HMC830(void);
void HMC829(void);

unsigned char Get_LockStatus(void);

void Init_HMC830(void);
void HMC830_Task(void);
#endif //_HMC830_H_
