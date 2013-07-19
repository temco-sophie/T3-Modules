// in order to write to the hex time you have to enter this line in 
//		-> BL51_Locate
//			-> Code:
//	?CO?FLASH_INFO(0100h)

#include <KEIL_VRS1000_regs.H>
#include "LibIO_T3IO.h"


unsigned char code product_model 	= T3_PRODUCT_MODEL;
unsigned int code firmware_version 	= T3_SOFTWARE_VERSION;



