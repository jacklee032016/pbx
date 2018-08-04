#include"var_define.h"
#include"etr_init_var.h"

pdata	INT8U	     	codingChannels[8][12];	//_at_ 0x20;		//0x20-3fh
extern	pdata	INT8U	     	codingChannels[8][12];	//_at_ 0x20;		//0x20-3fh

extern	pdata	INT8U	    	status[20][3];	//_at_ 0x00;		//0x00-0x17
pdata	INT8U	    		status[20][3];	//_at_ 0x00;		//0x00-0x17

pdata	unsigned int 	tel_jishi[20]; 	/*_at_ 0x18; in unit of 100ms */
extern	pdata 	unsigned int 	tel_jishi[20]; 	//_at_ 0x18;

