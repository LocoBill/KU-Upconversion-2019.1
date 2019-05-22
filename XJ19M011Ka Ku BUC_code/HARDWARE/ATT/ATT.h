#ifndef __ATT_H
#define __ATT_H

#include "sys.h"

#define RF_ATT16 				  PBout(15)       //RFÔ­ÀíÍ¼Åª·´ 
#define RF_ATT8 					PCout(6)
#define RF_ATT4 					PCout(7)
#define RF_ATT2 					PCout(8)
#define RF_ATT1 					PCout(9)
#define RF_ATT0_5 				PAout(8)

#define IF_ATT0_25 					PCout(5)
#define IF_ATT0_5 					PCout(4)
#define IF_ATT1 						PAout(7)
#define IF_ATT2 						PAout(6)
#define IF_ATT4 						PAout(5)
#define IF_ATT8 						PAout(4)
#define IF_ATT16 						PAout(3)



void ATT_init(void);

















#endif
