#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "ask_isdn.h"
#include "l3ins.h"

#include "ask_test.h"

#define B_RAWDEV			128//AS_ISDN_RAW_DEVICE

int dev_play_msg(devinfo_t *dev)
{
	unsigned char buf[PLAY_SIZE+AS_ISDN_HEADER_LEN];
	iframe_t *frm = (iframe_t *)buf;
	int len, ret ;
	static int length = 0;
	
	if (dev->play<0)
		return(0);

	len = read(dev->play, buf + AS_ISDN_HEADER_LEN, PLAY_SIZE);
	if (len<PLAY_SIZE )
	{
		printf("read file end,exit now %d: \"%s\"\n", errno, strerror(errno));
		close(dev->play);
		dev->play = -1;
		exit(1);
	}
	length += len;
	printf("read file  %d bytes\n", length );


	frm->addr = dev->b_adress[dev->used_bchannel] | IF_DOWN;
	frm->prim = DL_DATA | REQUEST;
	frm->dinfo = 0;
	frm->len = len;

	ret = ask_isdn_write(dev->fd, buf, len+AS_ISDN_HEADER_LEN, 8000);
	fprintf(stdout,"b Address %x(chan %d)\n",dev->b_adress[dev->used_bchannel] , dev->used_bchannel);

	if (ret < 0)
		fprintf(stdout,"play write error %d %s\n", errno, strerror(errno));
	else if (dev->debug>3)
		fprintf(stdout,"play write ret=%d\n", ret);

	return(ret);
}


int dev_send_data(devinfo_t  *dev)
{
	unsigned char buf[MAX_DATA_BUF+AS_ISDN_HEADER_LEN];
	iframe_t *frm = (iframe_t *)buf;
	unsigned char *data;
	int len, ret;
	
	if (dev->play<0 || !dev->fplay)
		return(0);
	
	if (!(data = fgets(buf + AS_ISDN_HEADER_LEN, MAX_DATA_BUF, dev->fplay)))
	{
		close(dev->play);
		dev->play = -1;
		data = buf + AS_ISDN_HEADER_LEN;
		data[0] = 4; /* ctrl-D */
		data[1] = 0;
	}
	
	len = strlen(data);
	if (len==0)
	{
		close(dev->play);
		dev->play = -1;
		data[0] = 4; /* ctrl-D */
		len = 1;
	}
	
	frm->addr = dev->b_adress[dev->used_bchannel] | IF_DOWN;
	frm->prim = DL_DATA | REQUEST;
	frm->dinfo = 0;
	frm->len = len;
	ret = ask_isdn_write(dev->fd, buf, len+AS_ISDN_HEADER_LEN, 100000);
	if (ret < 0)
		fprintf(stdout,"send_data write error %d %s\n", errno, strerror(errno));
	else if (dev->debug>3)
		fprintf(stdout,"send_data write ret=%d\n", ret);
	
	return(ret);
}

int dev_setup_bchannel(devinfo_t *dev, int index)
{
	int ret =0;

	if (( index<0) || (index>1))
	{
		fprintf(stdout, "wrong channel %d\n", dev->used_bchannel);
		return(0);
	}

	if(dev->isRaw)
	{
		char devfilename[128];
		sprintf(devfilename, "/dev/astel/%d", B_RAWDEV+index+(dev->cardnr-1)*2);
//		sprintf(devfilename, "/dev/astel/%d", B_RAWDEV+index);
		fprintf(stdout, "device file for raw B channel is %s\n", devfilename );	
		
		assert( (dev->bfd[index]= open(devfilename, O_RDWR))>0 );
	}

	return(ret);
}

static int dev_add_dstack_layer3_activate(devinfo_t *dev, int prot)
{
	unsigned char buf[1024];
	iframe_t *frm = (iframe_t *)buf;
	layer_info_t li;
	stack_info_t si;
	interface_info_t ii;
	int lid, ret;

#if 1
	if (dev->layer3)
	{
		memset(&si, 0, sizeof(stack_info_t));
		si.extentions = EXT_STACK_CLONE;
		si.mgr = -1;
		si.id = dev->d_stid;
		ret = ask_isdn_new_stack(dev->fd, &si);
		if (ret <= 0) 
		{
			fprintf(stdout, "clone stack failed ret(%d)\n", ret);
			return(11);
		}

		fprintf(stdout, "clone stack ID = %x\n", ret);
		dev->cloned = 1,
		dev->d_stid = ret;
	}
#endif	
	memset(&li, 0, sizeof(layer_info_t));
	strcpy(&li.name[0], "userspace-L3U");
	li.object_id = -1;
	li.extentions = 0;
	li.pid.protocol[3] = prot;
	li.pid.layermask = ISDN_LAYER(3);
	li.st = dev->d_stid;
	
	lid = ask_isdn_new_layer(dev->fd, &li);
	assert (lid>0);
	
	dev->layer3 = lid;
	assert (dev->layer3);
	fprintf(stdout, "D Channel Layer Address = %x\n", dev->layer3);
	
	ii.extentions = EXT_IF_CREATE | EXT_IF_EXCLUSIV;
	ii.owner = dev->layer3;
	ii.peer = dev->layer2;
	ii.stat = IF_DOWN;
	ret = ask_isdn_connect(dev->fd, &ii);
	if (ret)
		return(13);
	
	ii.owner = dev->layer3;
	ii.stat = IF_DOWN;
	assert(! ask_isdn_get_interface_info(dev->fd , &ii) );
	
	if (ii.peer == dev->layer2 )
		fprintf(stdout, "Layer 2 not cloned\n");
	else
		fprintf(stdout, "Layer 2 %08x cloned from %08x\n",ii.peer, dev->layer2);
	
	dev->layer2 = ii.peer;


	ret = ask_isdn_write_frame(dev->fd, buf, (dev->layer3&IF_IADDRMASK) | IF_DOWN,
		DL_ESTABLISH | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
/*
	ret = ask_isdn_write_frame(dev->device, buf, dev->layer3 | IF_DOWN,
		DL_UNITDATA | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
*/
	if ( dev->debug >3)
		fprintf(stdout,"%s : DL_ESTABLISH write ret=%d\n", dev->name, ret);

	ret = ask_isdn_read(dev->fd, buf, 1024, TIMEOUT_10SEC);
	if ( dev->debug >3)
		fprintf(stdout,"%s : dl_etablish read ret=%d\n", dev->name, ret);
	if (ret>0)
	{
		if (frm->prim != (DL_ESTABLISH | CONFIRM))
			return(6);
	}
	else
	{
		fprintf(stdout,"%s : DL_ESTABLISH | REQUEST return(%d)\n", dev->name,  ret);
		return(7);
	}

	return(0);
}


int dev_init(devinfo_t *dev)
{
	unsigned char buf[1024];
	iframe_t *frm = (iframe_t *)buf;
	int i, ret = 0;
	stack_info_t *stack_info;
	status_info_t *si;
	char fileName[64];
	
	sprintf(fileName, "record%s.data", dev->name );
	assert((dev->save = open(fileName , O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU))>0);
	
	if (0>(dev->play = open("greeting2.u"/*FileNameOut*/, O_RDONLY)))
	{
		printf(" %s : cannot open audio file due to %s\n", dev->name,	strerror(errno));
		dev->play = -1;
	}
	else 
		dev->fplay = fdopen(dev->play, "r");

	assert((dev->fd = ask_isdn_open())>0);
	
	ret = ask_isdn_get_stack_count(dev->fd );
	assert (ret >= dev->cardnr && dev->cardnr > 0);
	
	ret = ask_isdn_get_stack_info(dev->fd, dev->cardnr, buf, 1024);
	assert (ret>0);

	stack_info = (stack_info_t *)&frm->data.p;
	dev->d_stid = stack_info->id;

	if( stack_info->pid.protocol[0]&ISDN_PID_L0_TE_S0 )
	{
		dev->ntMode  = ISDN_MODE_TE;
	}
	else
	{
		dev->ntMode = ISDN_MODE_NT;
	}
	
	ask_isdn_print_stack_info(stdout, stack_info );

	dev->layer1 = ask_isdn_get_layerid(dev->fd, dev->d_stid, 1);
	assert (dev->layer1 > 0);
	
	dev->layer2 = ask_isdn_get_layerid(dev->fd, dev->d_stid, 2);
	assert(dev->layer2 > 0);

	dev->layer3 = ask_isdn_get_layerid(dev->fd, dev->d_stid, 3);
	if (dev->layer3 <= 0)
	{
		fprintf(stdout,"%s : cannot get layer3\n", dev->name );
//		return -(ENODEV);
	}

	ret = dev_add_dstack_layer3_activate(dev, ISDN_PID_L3_DSS1USER);
	if((!dev->ntMode)&& (ret!=0) )
	{
		fprintf(stderr, "TE must be activated before call\n");
		exit(1);
	}

	for (i=0;i<2;i++)
	{
		ret = dev_setup_bchannel( dev, i);
	}
	
	dev->flag |= FLG_BCHANNEL_SETUP;

	if(dev->ntMode)
	{
		ret = ask_isdn_get_status_info(dev->fd, dev->layer1, buf, 1024);
		if (ret > AS_ISDN_HEADER_LEN)
		{
			si = (status_info_t *)&frm->data.p;
			ask_isdn_print_status(stdout, si);
		}
		else
			fprintf(stdout,"%s : get layer1 status info ret(%d)\n", dev->name,  ret);
	}
	
	ret = ask_isdn_get_status_info(dev->fd, dev->layer2, buf, 1024);
	if (ret > AS_ISDN_HEADER_LEN)
	{
		si = (status_info_t *)&frm->data.p;
		ask_isdn_print_status(stdout, si);
	}
	else
		fprintf(stdout,"%s : get layer2 status info ret(%d)\n", dev->name, ret);

	printf("Waiting...\n");
	sleep(1);
	printf("Waiting off\n");
	return 0;
}

void dev_release(devinfo_t *dev)
{
	int err, ret;
	unsigned char buf[1024];
	int i;
	int prim;
	
	sleep(1);
	for(i=0; i<2; i++)
	{
		if(dev->isRaw)
			prim = PH_DEACTIVATE | REQUEST;
		else	
			prim = DL_RELEASE | REQUEST;
		ret = ask_isdn_write_frame(dev->fd, buf, dev->b_adress[i]|IF_DOWN,
			prim, 0, 0, NULL, TIMEOUT_1SEC);
	}

	ret = ask_isdn_write_frame(dev->fd, buf, dev->layer3 | IF_DOWN,
		DL_RELEASE | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	
	if ( dev->debug>3)
		fprintf(stdout,"ret=%d\n", ret);

	ret = ask_isdn_read(dev->fd, buf, 1024, TIMEOUT_10SEC);
	if (dev->debug>3)
		fprintf(stdout,"read ret=%d\n", ret);

	sleep(1);
	ret = ask_isdn_write_frame(dev->fd, buf, dev->layer3 | IF_DOWN,
		MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);

	if (dev->debug>3)
		fprintf(stdout,"ret=%d\n", ret);

	for(i=0; i<2; i++)
	{
		ret = ask_isdn_write_frame(dev->fd, buf, dev->b_adress[i]|IF_DOWN,
			MGR_DELLAYER | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	}

	if(dev->cloned)
		ask_isdn_clear_stack(dev->fd, dev->d_stid );
	
	ret = ask_isdn_read(dev->fd, buf, 1024, TIMEOUT_10SEC);
	if (dev->debug >3)
		fprintf(stdout,"read ret=%d\n", ret);

	if ((err= ask_isdn_close( dev->fd) ) )
		fprintf(stdout,"%s : error %s\n", dev->name, strerror(err) );
	
	fprintf(stdout,"%s : release all\n", dev->name );
}


int dev_record_audio(devinfo_t *dev, 	iframe_t	*rfrm)
{
	int  ret;
	
	ret = write(dev->save, (unsigned char *)&rfrm->data.i,rfrm->len);
	if (dev->debug>1)
		fprintf(stdout,"Record %d bytes\n" , ret );

	if (dev->debug > 3)
		fprintf(stdout,"send out DL_DATA_RESPONSE\n" );
	return(ret);
}


int dev_send_touchtone(devinfo_t *dev, int tone)
{
	iframe_t frm;
	int tval, ret;

	if (dev->debug>1)
		fprintf(stdout,"send_touchtone %c\n", DTMF_TONE_MASK & tone);
	
	tval = DTMF_TONE_VAL | tone;
	ret = ask_isdn_write_frame(dev->fd, &frm, dev->b_adress[dev->used_bchannel] | IF_DOWN,
		PH_CONTROL | REQUEST, 0, 4, &tval, TIMEOUT_1SEC);
	
	if (dev->debug > 3)
		fprintf(stdout,"tt send ret=%d\n", ret);

	return(ret);
}

