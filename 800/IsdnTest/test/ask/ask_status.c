/*
$Id: ask_status.c,v 1.1.1.1 2006/11/30 16:21:59 lizhijie Exp $
*/
#include <errno.h>
#include <string.h>
#include "ask_isdn.h"

/* State values for l1 state machine (status_info_l1_t state field) */
char *strL1SState[] =
{
	"ST_L1_F2",
	"ST_L1_F3",
	"ST_L1_F4",
	"ST_L1_F5",
	"ST_L1_F6",
	"ST_L1_F7",
	"ST_L1_F8",
};


/* State values for l2 state machine (status_info_l2_t state field) */
char *strL2State[] =
{
	"ST_L2_1",
	"ST_L2_2",
	"ST_L2_3",
	"ST_L2_4",
	"ST_L2_5",
	"ST_L2_6",
	"ST_L2_7",
	"ST_L2_8",
};

/* get status info about a LAYER */
int ask_isdn_get_status_info(int fid, int id, void *info, size_t max_len)
{
	iframe_t	ifr;
	int		ret;

	set_wrrd_atomic(fid);
	ret = ask_isdn_write_frame(fid, &ifr, id, MGR_STATUS | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	if (ret) 
	{
		clear_wrrd_atomic(fid);
		return(ret);
	}
	ret = ask_isdn_read_frame(fid, info, max_len,id, MGR_STATUS | CONFIRM, TIMEOUT_1SEC);
	clear_wrrd_atomic(fid);
	return(ret);
}

/* print LAYER info; not complete now */
int  ask_isdn_print_status(FILE *file, status_info_t *si)
{
	int ret=0;
	status_info_l1_t *si1;
	status_info_l2_t *si2;

	fprintf(file," PROTOCOL STATUS INFO : Layer %d\n" ,si->typ );
	
	switch(si->typ)
	{
		case STATUS_INFO_L1:
			si1 = (status_info_l1_t *)si;
			fprintf(file,"\tProtocol \t: %x \t\tStatus \t: %s \n\tState \t\t: %s \t\tFlags \t: %x\n", si1->protocol, (si1->status==1)?"Active":"Deactive",  strL1SState[si1->state], si1->Flags);
			break;
			
		case STATUS_INFO_L2:
			si2 = (status_info_l2_t *)si;
			fprintf(file,"\tProtocol \t: %x \t\tTEI \t: %d \t\tSAPI \t: %d \n\tState \t\t: %s \t\tFlag \t: %x\n", si2->protocol, si2->tei, si2->sapi,strL2State[si2->state], si2->flag);
			break;
			
		default:
			fprintf(file, "unknown status type %d\n", si->typ);
			break;
	}
	return(ret);
}

