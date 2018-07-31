/*
$Id: ask_test_logger.c,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "ask_isdn.h"
#include "l3ins.h"

#include "ask_user.h"

static char _log_usage[] =
{
	"Call with %s [options] [filename]\r\n\r\n" \
	"\n     Valid options are:\r\n\r\n" \
	"  -?              Usage ; printout this information\n" \
	"  -c<n>           use card number n (default 1)\n" \
	"  -F<n>           use function n (default 0)\n" \
	"                    0 normal logging\r\n\r\n"
};

typedef struct _devinfo {
	int	device;
	int	cardnr;
	int	func;
	char	phonenr[32];
	int	d_stid;
	int	layer1;
	int	layer2;
	int	layer3;
	int	b_stid[2];
	int	b_adress[2];
	int	used_bchannel;
	int	save;
	int	flag;
	int	val;
	int	cr;
	int	si;
	int	bl1_prot;
	int	bl2_prot;
	int	bl3_prot;
} devinfo_t;

#define FLG_SEND_TONE		0x0001
#define FLG_SEND_DATA		0x0002
#define FLG_BCHANNEL_SETUP	0x0010
#define FLG_BCHANNEL_DOACTIVE	0x0020
#define FLG_BCHANNEL_ACTIVE	0x0040
#define FLG_BCHANNEL_ACTDELAYED	0x0080
#define FLG_CALL_ORGINATE	0x0100
#define FLG_BCHANNEL_EARLY	0x0200


#define MAX_REC_BUF		4000
#define MAX_DATA_BUF		1024

static int logVerifyOn=0xff;

static devinfo_t *init_di = NULL;

#define	MsgHead(ptr, cref, mty) \
	*ptr++ = 0x8; \
	if (cref == -1) { \
		*ptr++ = 0x0; \
	} else { \
		*ptr++ = 0x1; \
		*ptr++ = cref^0x80; \
	} \
	*ptr++ = mty
	
int printhexdata(FILE *f, int len, u_char *p)
{
	while(len--) {
		fprintf(f, "%02x", *p++);
		if (len)
			fprintf(f, " ");
	}
	fprintf(f, "\n");
	return(0);
}

int logger_process_dchannel(devinfo_t *di, int len, iframe_t *frm)
{
	write(di->save, frm, len);
	if (frm->prim == (PH_DATA | INDICATION) && (frm->len >0))
	{
		if (logVerifyOn>5)
			printhexdata(stdout, frm->len, (void *)&frm->data.i);
	} 

	return(0);
}

int logger_setup_bchannel(devinfo_t *di)
{
	AS_ISDN_pid_t pid;
	int ret;
	layer_info_t li;

	if ((di->used_bchannel<0) || (di->used_bchannel>1))
	{
		fprintf(stdout, "wrong channel %d\n", di->used_bchannel);
		return(0);
	}

	memset(&li, 0, sizeof(layer_info_t));

	strcpy(&li.name[0], "B L3");
	li.object_id = -1;
	li.extentions = 0;
	li.pid.protocol[3] = di->bl3_prot;
	li.pid.layermask = ISDN_LAYER(3);

	li.st = di->b_stid[di->used_bchannel];
	ret = ask_isdn_new_layer(di->device, &li);

	if (ret<0)
	{
		fprintf(stdout, "new_layer ret(%d)\n", ret);
		return(0);
	}

	if (ret)
	{
		di->b_adress[di->used_bchannel] = ret;
		if (logVerifyOn>2)
			fprintf(stdout,"b_adress%d %08x\n",di->used_bchannel+1, ret);
		
		memset(&pid, 0, sizeof(AS_ISDN_pid_t));
		pid.protocol[1] = di->bl1_prot;
		pid.protocol[2] = di->bl2_prot;
		pid.protocol[3] = di->bl3_prot;
		pid.layermask = ISDN_LAYER(1) | ISDN_LAYER(2)| ISDN_LAYER(3);

		if (di->flag & FLG_CALL_ORGINATE)
			pid.global = 1;

		ret = ask_isdn_set_stack(di->device, di->b_stid[di->used_bchannel], &pid);
		if (ret)
		{
			fprintf(stdout, "set_stack ret(%d)\n", ret);
			return(0);
		}
		ret = di->b_adress[di->used_bchannel];
	}
	return(ret);
}

int logger_activate_bchan(devinfo_t *di)
{
	unsigned char buf[128];
	iframe_t *rfrm;
	int ret;

	ret = ask_isdn_write_frame(di->device, buf, di->b_adress[di->used_bchannel] | IF_DOWN,
		DL_ESTABLISH | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);

	if (logVerifyOn>3)
		fprintf(stdout,"DL_ESTABLISH write ret=%d\n", ret);

	ret = ask_isdn_read(di->device, buf, 128, TIMEOUT_10SEC); 	
	if (logVerifyOn>3)
		fprintf(stdout,"DL_ESTABLISH read ret=%d\n", ret);

	rfrm = (iframe_t *)buf;
	if (ret>0)
	{
		if (rfrm->prim == (DL_ESTABLISH | CONFIRM))
		{
			di->flag |= FLG_BCHANNEL_ACTIVE;
		}
	}
	return(ret);
}

int logger_deactivate_bchan(devinfo_t *di)
{
	unsigned char buf[128];
	int ret;

	ret = ask_isdn_write_frame(di->device, buf, di->b_adress[di->used_bchannel] | IF_DOWN,
		DL_RELEASE | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);

	if (logVerifyOn>3)
		fprintf(stdout,"DL_RELEASE write ret=%d\n", ret);

	ret = ask_isdn_read(di->device, buf, 128, TIMEOUT_10SEC); 	
	if (logVerifyOn>3)
		fprintf(stdout,"DL_RELEASE read ret=%d\n", ret);

	di->flag &= ~FLG_BCHANNEL_ACTIVE;
	di->flag &= ~FLG_BCHANNEL_SETUP;
	ret = ask_isdn_clear_stack(di->device, di->b_stid[di->used_bchannel]);

	if (logVerifyOn>3)
		fprintf(stdout,"clear_stack ret=%d\n", ret);

	return(ret);
}

int logger_read_mutiplexer(devinfo_t *di)
{
	unsigned char	buf[MAX_REC_BUF];
	iframe_t	*rfrm;
	int		timeout = TIMEOUT_10SEC;
	int		ret = 0;

	rfrm = (iframe_t *)buf;
	/* Main loop */
	while (1)
	{
		ret = ask_isdn_read(di->device, buf, MAX_REC_BUF, timeout);
		if (logVerifyOn>3)
			fprintf(stdout,"readloop ret=%d\n", ret);
		
		if (ret == -1)
		{
			fprintf(stdout,"readloop read error\n");
			break;
		}

		if (ret >= 16)
		{
			if (logVerifyOn>4)
				fprintf(stdout,"readloop addr(%x) prim(%x) len(%d)\n", rfrm->addr, rfrm->prim, rfrm->len);

			if (rfrm->addr == (di->b_adress[di->used_bchannel] | IF_DOWN))
			{/* B-Channel related messages */
				if (rfrm->prim == (DL_DATA | INDICATION))
				{/* received data, save it */
					write(di->save, &rfrm->data.i, rfrm->len);
				} 
			}
			else if (rfrm->addr == (di->layer2 | IF_DOWN))
			{/* D-Channel related messages */  
				if (logVerifyOn>4)
					fprintf(stdout,"readloop addr(%x) prim(%x)len(%d)\n", rfrm->addr, rfrm->prim, rfrm->len);
				logger_process_dchannel(di, ret, rfrm);
			}
			else
			{
				if (logVerifyOn)
					fprintf(stdout,"readloop unknown addr(%x) prim((%x)len(%d)\n", rfrm->addr, rfrm->prim, rfrm->len);
			}
			
		}
	}
	return(0);
}


int logger_add_dlayer2(devinfo_t *di, int prot)
{
	layer_info_t li;
	interface_info_t ii;
	int lid, ret;

	memset(&li, 0, sizeof(layer_info_t));
	
	strcpy(&li.name[0], "user L2");
	li.object_id = -1;
	li.extentions = 0;
	li.pid.protocol[2] = prot;
	li.pid.layermask = ISDN_LAYER(2);
	li.st = di->d_stid;
	
	lid = ask_isdn_new_layer(di->device, &li);
	if (lid<0)
		return(12);
	
	di->layer2 = lid;
	if (!di->layer2)
		return(13);
	
	ii.extentions = EXT_IF_CREATE | EXT_IF_EXCLUSIV;
	ii.owner = di->layer2;
	ii.peer = di->layer1;
	ii.stat = IF_DOWN;
	
	ret = ask_isdn_connect(di->device, &ii);
	if (ret)
		return(13);
	
	ii.owner = di->layer2;
	ii.stat = IF_DOWN;
	ret = ask_isdn_get_interface_info(di->device, &ii);
	if (ret != 0)
		return(14);
	
	if (ii.peer == di->layer1)
		fprintf(stdout, "Layer 1 not cloned\n");
	else
		fprintf(stdout, "Layer 1 %08x cloned from %08x\n", ii.peer, di->layer1);
	
	return(0);
}

int logger_do_setup(devinfo_t *di)
{
	unsigned char buf[1024];
	iframe_t *frm = (iframe_t *)buf;
	int i, ret = 0;
	stack_info_t *stinf;
	status_info_t *si;

	di->bl2_prot = ISDN_PID_L2_B_TRANS;
	di->bl3_prot = ISDN_PID_L3_B_TRANS;
	switch (di->func) 
	{
		case 0:
			di->bl1_prot = ISDN_PID_L1_B_64TRANS;
			di->si = 1;
			break;
			
		default:
			fprintf(stdout,"unknown program function %d\n", di->func);
			return(1);
	}
	
	ret = ask_isdn_get_stack_count(di->device);
	if (logVerifyOn>1)
		fprintf(stdout,"%d stacks found\n", ret);
	if (ret < di->cardnr) 
	{
		fprintf(stdout,"cannot config card nr %d only %d cards\n",	di->cardnr, ret);
		return(2);
	}
	
	ret = ask_isdn_get_stack_info(di->device, di->cardnr, buf, 1024);
	if (ret<=0)
	{
		fprintf(stdout,"cannot get stackinfo err: %d\n", ret);
		return(3);
	}
	stinf = (stack_info_t *)&frm->data.p;
	if (logVerifyOn>1)
		ask_isdn_print_stack_info(stdout, stinf);
	
	di->d_stid = stinf->id;
	for (i=0;i<2;i++)
	{
		if (stinf->childcnt > i)
			di->b_stid[i] = stinf->child[i];
		else
			di->b_stid[i] = 0;
	}

	di->layer1 = ask_isdn_get_layerid(di->device, di->d_stid, 1);
	if (di->layer1<0)
	{
		fprintf(stdout,"cannot get layer1\n");
		return(4);
	}
	if (logVerifyOn>1)
		fprintf(stdout,"layer1 id %08x\n", di->layer1);

	di->layer2 = ask_isdn_get_layerid(di->device, di->d_stid, 2);
	if (logVerifyOn>1)
		fprintf(stdout,"layer2 id %08x\n", di->layer2);

	if (!di->layer2)
	{
		fprintf(stdout,"layer 2 has not present\n");
		return(5);
	}

#if 0
	ret = logger_add_dlayer2(di, ISDN_PID_L2_LAPD);
	if (ret)
		return(ret);
#endif

	ret = ask_isdn_get_status_info(di->device, di->layer1, buf, 1024);
	if (ret > AS_ISDN_HEADER_LEN)
	{
		si = (status_info_t *)&frm->data.p;
		ask_isdn_print_status(stdout, si);
	}
	else
		fprintf(stdout,"ask_isdn_get_status_info ret(%d)\n", ret);

	ret = ask_isdn_get_status_info(di->device, di->layer2, buf, 1024);
	if (ret > AS_ISDN_HEADER_LEN)
	{
		si = (status_info_t *)&frm->data.p;
		ask_isdn_print_status(stdout, si);
	}
	else
		fprintf(stdout,"ask_isdn_get_status_info ret(%d)\n", ret);

	sleep(1);
	init_di = di;
	return(0);
}

void logger_close_di(devinfo_t *di)
{
	unsigned char buf[1024];
	int ret = 0;

	init_di = NULL;
	ret = ask_isdn_write_frame(di->device, buf, di->layer3 | IF_DOWN, MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	if (logVerifyOn>3)
		fprintf(stdout,"ret=%d\n", ret);

	ret = ask_isdn_read(di->device, buf, 1024, TIMEOUT_10SEC);
	
	if (logVerifyOn>3)
		fprintf(stdout,"read ret=%d\n", ret);
}

static void logger_term_handler(int sig)
{
	if (init_di)
		logger_close_di(init_di);
}

int main(int argc, char *argv[])
{
	char FileName[200],FileNameOut[200];
	int aidx=1,para=1;
	char sw;
	devinfo_t AS_ISDN;
	int err;

	fprintf(stderr,"TestAS_ISDN 1.0\n");
	strcpy(FileName, "test_file");
	memset(&AS_ISDN, 0, sizeof(AS_ISDN));
	AS_ISDN.cardnr = 1;
	AS_ISDN.func = 0;
	AS_ISDN.phonenr[0] = 0;

	signal(SIGTERM, logger_term_handler);
	signal(SIGINT, logger_term_handler);
	signal(SIGPIPE, logger_term_handler);

	if (argc<1)
	{
		fprintf(stderr,"Error: Not enough arguments please check\n");
		usage(argv[0],_log_usage);
		exit(1);
	}

	do
	{
			if (argv[aidx] && argv[aidx][0]=='-')
			{/* option with '-' */
				sw=argv[aidx][1];
				switch (sw)
				{
					 case 'v':
					 case 'V':
						logVerifyOn=1;
						if (argv[aidx][2])
						{
							logVerifyOn=atol(&argv[aidx][2]);
						}
						break;

					case 'c':
						if (argv[aidx][2])
						{
							AS_ISDN.cardnr=atol(&argv[aidx][2]);
						}
						break;
						
					case 'F':
						if (argv[aidx][2])
						{
							AS_ISDN.func=atol(&argv[aidx][2]);
						}
						break;
						
					case '?' :
						usage(argv[0],_log_usage);
						exit(1);
						break;
						
					default  :
						fprintf(stderr,"Unknown Switch %c\n",sw);
						usage(argv[0],_log_usage);
						exit(1);
						break;
				}
			} 
			else
			{
				if (para==1)
				{
					if (argc > 1)
						strcpy(FileName,argv[aidx]);
					para++;
				}
				else
				{
					fprintf(stderr,"Undefined argument %s\n",argv[aidx]);
					usage(argv[0],_log_usage);
					exit(1);
				}
			}

			aidx++;
	} while (aidx<argc);


	if (0>(AS_ISDN.device = ask_isdn_open()))
	{
		printf("Test ISDN cannot open AS_ISDN due to %s\n", 	strerror(errno));
		return(1);
	}
	
	sprintf(FileNameOut,"%s",FileName);
	if (0>(AS_ISDN.save = open(FileName, O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU)))
	{
		printf("Test ISDN cannot open %s due to %s\n",FileName, strerror(errno));
		close(AS_ISDN.device);
		return(1);
	}
	
	if (logVerifyOn>8)
		fprintf(stdout,"fileno %d/%d\n",AS_ISDN.save, AS_ISDN.device);
	
	err = logger_do_setup(&AS_ISDN);
	if (err)
		fprintf(stdout,"do_setup error %d\n", err);
	else
		logger_read_mutiplexer(&AS_ISDN);
	
	close(AS_ISDN.save);
	err=ask_isdn_close(AS_ISDN.device);
	if (err)
		fprintf(stdout,"ask_isdn_close: error(%d): %s\n", err, strerror(err));
	
	return(0);
}
