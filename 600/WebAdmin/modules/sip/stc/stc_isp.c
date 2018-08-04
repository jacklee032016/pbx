/*
* $Id: stc_isp.c,v 1.10 2007/06/14 11:23:50 lizhijie Exp $
*/

#include <stdio.h>
#include "aslib.h"
#include "as_common_version.h"
#include "as_version.h"
#include "cgi_lib.h"
#include "cgi_alert.h"
#if __WITH_NEW_STC__
#include "uart_cmd.h"
#endif

#define	STC_ISP_SECTOR_LENGTH			512

#define	DATA_LENGTH_LOW					128
#define	DATA_FRAM_LENGTH_LOW			DATA_LENGTH_LOW+14

#define	ERASE_DATA_COMMAND	0xb3
#define	WRITE_DATA_COMMAND	0xb4

#define	STC_ISP_RESERVE				0x00

#define	STC_ISP_CMD_REQUEST				0xB0
#define	STC_ISP_CMD_RESET_IAP			0xB1
#define	STC_ISP_CMD_ERASE					0xB2
#define	STC_ISP_CMD_WRITE					0xB3
#define	STC_ISP_CMD_ERASE_WRITE			0xB4
#define	STC_ISP_CMD_FIRM_VERSION			0xB5

#define	STC_ISP_CHECKSUM_REQUEST		0xb7
#define	STC_ISP_CHECKSUM_ERASE			0xb9
#define	STC_ISP_CHECKSUM_RESET_IAP		0xb8
#define	STC_ISP_CHECKSUM_FIRM_VERSION	0xF9


struct	_isp_frame_header
{
	unsigned char		first;
	unsigned	char		second;
	unsigned	char		lengthHigh;
	unsigned	char		lengthLow;
	unsigned	char		cmd;
}  __attribute__ ((packed));
typedef	struct _isp_frame_header		STC_ISP_FRAME_HEADER;

struct _isp_cmd_frame
{
	STC_ISP_FRAME_HEADER	header;
	
	unsigned char				checksumHigh;
	unsigned	char				checksumLow;
	unsigned char				end;
} __attribute__ ((packed));
typedef struct _isp_cmd_frame			STC_ISP_CMD_FRAME;

unsigned char	stc_isp_request[] =
{
	STC_ISP_FIRST_BYTE,	STC_ISP_SECOND_BYTE,
	0x00,	0x08,
	STC_ISP_CMD_REQUEST, 	0x01, 
	STC_ISP_CHECKSUM_REQUEST, 	STC_ISP_END_BYTE
};

struct _isp_version_frame
{
	STC_ISP_FRAME_HEADER	header;
	
	unsigned	char				version;
	unsigned char				mcuHigh;
	unsigned	char				mcuLow;
	unsigned char				reserved;
	
	unsigned char				checksumHigh;
	unsigned	char				checksumLow;
	unsigned char				end;
}__attribute__ ((packed));

typedef	struct _isp_version_frame STC_ISP_VERSION_FRAME;

struct _isp_data_frame
{
	STC_ISP_FRAME_HEADER	header;
	
	unsigned char				reserveHigh;
	unsigned char				reserveLow;
	unsigned char				addressHigh;
	unsigned char				addressLow;
	unsigned char				dataLengthHigh;
	unsigned char				dataLengthLow;	
	unsigned char				flashData[DATA_LENGTH_LOW + 3];/* 3 : for checksum(2) and end byte 0x16 */
} __attribute__ ((packed));
typedef	struct _isp_data_frame		STC_ISP_DATA_FRAME;

unsigned char	_data_frame_header[]=
{
	STC_ISP_FIRST_BYTE, STC_ISP_SECOND_BYTE,
	0x00, sizeof(STC_ISP_DATA_FRAME),
	STC_ISP_CMD_WRITE, /*header */
	
	STC_ISP_RESERVE, STC_ISP_RESERVE,
	0x00,0x00,
	0x00,DATA_LENGTH_LOW
};

STC_ISP_DATA_FRAME	_data_frame;

typedef	struct
{
	uart_t		*uart;
	int			binFd;

	int			imageSize;
	int			total;
}stc_isp;

extern	uart_t  	serial_pbx;

#define	STC_ISP_CHECK_CMD(frame, cmdType) \
	( ((frame)->first == STC_ISP_FIRST_BYTE)&& \
		((frame)->second== STC_ISP_SECOND_BYTE) \
		&& ((frame)->cmd== (cmdType )) ) 

#define	GET_CHECKSUM(checksum, buf, length)	\
	do{ int	i;	checksum = 0; \
	for(i=0; i< (length); i++){ \
		checksum += *((buf)+i) ; }	\
}while(0)

#define	STC_ISP_NAME	"ISP"

log_stru_t ispLog = 
{
	name		:	STC_ISP_NAME,
	lstyle		:	USE_CONSOLE,//USE_FILE,
	llevel		: 	AS_LOG_DEBUG,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_STC_ISP, 
	isDaemonized	:	0
};

#define	ISP_PRINT_RAW(isRead, data, length)	\
do{ int i; if (length<= 0) break;	\
	printf("%s : \n\t", (isRead!=0)?"READ":"WRITE");	\
	for(i=0; i<(length); i++){	\
		printf("0X%x %s", *((data)+i), ((i%16==0)&& (i!=0) )?"\n\t":" "); }\
		printf("\n");	\
		}while(0)


#ifndef   __AS_RELEASE//__#if 1 
#define	DEBUG_RAW(isRead, data, length)	//ISP_PRINT_RAW(isRead, data, length)
#else
#define	DEBUG_RAW(isRead, data, length)
#endif
			
int	__isp_cmd_frame(stc_isp *isp, unsigned char	frameType)
{
	int	res;
	STC_ISP_CMD_FRAME	*frame = (STC_ISP_CMD_FRAME *)stc_isp_request;
	char		*cmdName;

	frame->header.cmd = frameType;
	if(frameType==STC_ISP_CMD_REQUEST)
	{
		frame->checksumLow = STC_ISP_CHECKSUM_REQUEST;
		cmdName = "ISP REQUEST";
	}
	else if(frameType == STC_ISP_CMD_ERASE )
	{
		frame->checksumLow = STC_ISP_CHECKSUM_ERASE;
		cmdName = "ISP ERASE";
	}
	else if(frameType == STC_ISP_CMD_RESET_IAP)
	{
		frame->checksumLow = STC_ISP_CHECKSUM_RESET_IAP;
		cmdName = "ISP RESET";
	}
	else if(frameType == STC_ISP_CMD_FIRM_VERSION )
	{
		frame->checksumLow = STC_ISP_CHECKSUM_FIRM_VERSION;
		cmdName = "ISP FIRMWARE VERSION";
	}
	else
	{	
		AS_MSG_LOG(AS_LOG_WARNING, "Not support STC ISP command frame with type %x", frameType);
		return 1;
	}

	AS_MSG_LOG(AS_LOG_INFO, "Command %s is send out", cmdName );
	res = (isp->uart->write)(isp->uart, stc_isp_request, sizeof(stc_isp_request));
	if( res == sizeof(stc_isp_request) )
	{
		DEBUG_RAW(0, stc_isp_request, res);
		return 0;
	}
	
	AS_MSG_LOG(AS_LOG_WARNING, "write only type %d, ought to be %d", res, sizeof(stc_isp_request) );
	return 1;
}

int	__is_validate_data_feedback(unsigned short datachecksum, unsigned char *response)
{
//	unsigned short checksum;
	if( (*(response+5)==(datachecksum>>8) ) && ( *(response+6)== (datachecksum&0xff)) )
		return 1;

	AS_MSG_LOG(AS_LOG_WARNING, "data check 0x%x:0x%x, received 0x%x:0x%x", 
		datachecksum>>8, datachecksum&0xff,*(response+5), *(response+6) );
	
	return 0;
}

int	__isp_program_a_sector(stc_isp *isp, int sectorNumber, unsigned char	*bufForSector, int length )
{
	int	count, res, len = DATA_LENGTH_LOW;
	int	position = 0, i;
	unsigned short checksum, datachecksum;
	unsigned short address = sectorNumber*STC_ISP_SECTOR_LENGTH;
	unsigned char	buf[1024];
	STC_ISP_FRAME_HEADER *ackFrame = (STC_ISP_FRAME_HEADER *)buf;
	
	STC_ISP_FRAME_HEADER	*header = (STC_ISP_FRAME_HEADER *)&_data_frame;
	unsigned char			*frame = (unsigned char *)&_data_frame;
	unsigned char			*data = (unsigned char *)_data_frame.flashData;

	count = length/DATA_LENGTH_LOW;
	if( (length%DATA_LENGTH_LOW)!= 0 )
	{/*this is last block of binary file */
		count ++;
	}
	
	for(i=0; i< count; i++)
	{
		if( (position + DATA_LENGTH_LOW) > length )
			len = length - position;
		checksum = 0;
		isp->total += len;
		AS_MSG_DEBUG(AS_LOG_INFO, "\tWriting %d Data Block with length of %d bytes(%d).....", i, len, isp->total);
		
		memcpy(_data_frame.flashData, bufForSector + position, len);
		
		GET_CHECKSUM(datachecksum, data, len );
		
		header->lengthLow = sizeof(STC_ISP_DATA_FRAME)-DATA_LENGTH_LOW + len;

		_data_frame.dataLengthLow = len;
		_data_frame.addressHigh = address>>8;
		_data_frame.addressLow = address;
		
		GET_CHECKSUM(checksum, frame, sizeof(STC_ISP_DATA_FRAME)-DATA_LENGTH_LOW-3 + len );
		*(data+len) = checksum>>8;
		*(data+len +1 ) = checksum;
		*(data+len +2 ) = STC_ISP_END_BYTE;

#if 1
		res = (isp->uart->write)(isp->uart, frame, header->lengthLow );
		if(res != header->lengthLow)
		{
			AS_MSG_LOG(AS_LOG_WARNING, "\tWrite Data Frame to UART Error");
			if(res > 0 )
				ISP_PRINT_RAW(0, frame, res);
			return 1;
		}
		DEBUG_RAW(0, frame, header->lengthLow);
		usleep(1200);/*1.2 ms is value tested, lizhijie, 2007.05.06 */
		res = (isp->uart->read)(isp->uart, buf, sizeof(buf) );

		DEBUG_RAW(1, buf, res);
		if(res< 0 || !STC_ISP_CHECK_CMD(ackFrame, STC_ISP_CMD_WRITE) ||
			!__is_validate_data_feedback( datachecksum, buf) )
		{
			AS_MSG_LOG(AS_LOG_WARNING, "\t%d block of Sector Programming Error(Frame %d, data %d bytes)", i+1, header->lengthLow, len);
			ISP_PRINT_RAW(0, frame, header->lengthLow);
			if(res > 0 )
				ISP_PRINT_RAW(1, buf, res);
			return 1;
		}
		address += len;
		position += len;
#else
		DEBUG_RAW(0, frame, header->lengthLow);
		AS_MSG_DEBUG(AS_LOG_INFO, "\tWriting %d Data Block Sucessfully!", i );
#endif
		
	}
	
	return 0;
}

int	_isp_program(stc_isp *isp)
{
	unsigned char	buf[STC_ISP_SECTOR_LENGTH];
	int res ;
	int	count = 0;

	memcpy(&_data_frame, _data_frame_header, sizeof(_data_frame_header) );
	
	res = read( isp->binFd, buf, sizeof(buf) );
	while(res > 0)
	{
		count ++;
		AS_MSG_DEBUG(AS_LOG_INFO, "Programming %d Sectors.....", count);
		isp->imageSize += res;
		
		if(__isp_program_a_sector(isp,count-1, buf, res) )
		{
			AS_MSG_LOG(AS_LOG_ERR, "Programming %d Sectors Error", count);
			return 1;
		}
		
		AS_MSG_LOG(AS_LOG_INFO, "Programming %d Sectors Sucessfully!", count);
		
		res = read( isp->binFd, buf, sizeof(buf) );
	};

#if 1	
	AS_MSG_LOG(AS_LOG_INFO, "Programming %d Sectors.....", 117);
	memset(buf, 0x55, STC_ISP_SECTOR_LENGTH);
	if(__isp_program_a_sector(isp, 117, buf, STC_ISP_SECTOR_LENGTH) )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Programming %d Sectors Error", 117);
		return 0;
	}
		
	AS_MSG_LOG(AS_LOG_INFO, "Programming %d Sectors Sucessfully!", 117);
#endif
	return 0;
}

int	_isp_read_firm_version(stc_isp *isp)
{
	int	res;
	char		buf[1024];
	STC_ISP_VERSION_FRAME *version = (STC_ISP_VERSION_FRAME *)buf;

	if(__isp_cmd_frame(isp, STC_ISP_CMD_FIRM_VERSION) )
		return 1;

	res = (isp->uart->read)(isp->uart, buf, sizeof(buf) );
	if(res!= sizeof(STC_ISP_VERSION_FRAME) )
	{
		AS_MSG_LOG(AS_LOG_WARNING, "ISP Version frame length is %d, ought to be %d", res, sizeof(STC_ISP_VERSION_FRAME));
		ISP_PRINT_RAW(1, buf, res);
		return 1;
	}

	AS_MSG_LOG(AS_LOG_INFO, "ISP Version is %d, MCU is %d.%d", version->version, version->mcuHigh, version->mcuLow );
	return 0;
}

int	_isp_check_cmd_frame(stc_isp *isp, unsigned char	frameType)
{
	int	res = 0;
	char		buf[1024];
	STC_ISP_FRAME_HEADER *header = (STC_ISP_FRAME_HEADER *)buf;

//	while(res< 90)
	{
		if(__isp_cmd_frame(isp, frameType) )
			return 1;
		res++;
//	}

//	res = 0;
//	while(res<10)
//		usleep(10*1000);
//	{
		res = (isp->uart->read)(isp->uart, buf, sizeof(buf) );

		
		if( STC_ISP_CHECK_CMD(header, STC_ISP_CMD_REQUEST) )
			return 0;
		
		ISP_PRINT_RAW(1, buf, res);
		AS_MSG_LOG(AS_LOG_WARNING, "read only type %d, ought to be %d", res, 8 );
		res++;
	}
	
	return 1;
}

int main(int argc, char *argv[])
{
	stc_isp	_isp;
	int		res;
	int		count = 0;

	unsigned char	buff[1024];
//	STC_ISP_CMD_FRAME *frame = (STC_ISP_CMD_FRAME *)buff;
//	STC_ISP_FRAME_HEADER *header = (STC_ISP_FRAME_HEADER *)buff;

	as_ver_debug(AS_VERSION_INFO(AS_STC_ISP_MODULE_NAME));	
	as_ver_log(AS_STC_ISP_MODULE_NAME,AS_VERSION_INFO(AS_STC_ISP_MODULE_NAME));
	if (as_log_init(&ispLog) < 0)
	{
		printf("%s Log Init Failed.\n", ispLog.name);
		return 0;
	}

	if(argc < 2)
	{
		AS_MSG_LOG(AS_LOG_EMERG, "No STC Binary is defined");
		return -1;
	}

	if((_isp.binFd = open(argv[1], O_RDONLY ,0444))<0)
	{
		AS_MSG_LOG(AS_LOG_EMERG,"File of %s is not found", argv[1]);
		return -1;
	}
	
	res = as_cmd_uart_init(0, 0);
	if(res < 0)
	{
		AS_MSG_LOG(AS_LOG_EMERG, "Can not open UART for STC ISP update");
		return -1;
	}
	_isp.uart = &serial_pbx;
	_isp.total = 0;
	_isp.imageSize = 0;
	
/*inform PBX update program */
#if __WITH_NEW_STC__
	buff[0] = CMD_CFG_IP_PBX_RESET_ISP;
#else
	buff[0]=0x60;
#endif
	buff[1]=0x00;
	buff[2]=0xff;
	AS_MSG_LOG(AS_LOG_INFO, "Reset STC to ISP");
	for(count = 0; count<4; count++)
		res = (_isp.uart->write)(_isp.uart, buff, 3);

	usleep(10*1000);

#if 0
	
	count = 0;
	while(1)
	{
		count++;
		AS_MSG_LOG(AS_LOG_INFO, "send %d request", count);
		__isp_cmd_frame(&_isp, STC_ISP_CMD_REQUEST);
	}
#endif

#if 0	
	AS_MSG_LOG(AS_LOG_INFO, "Waiting ISP response.....");
	while(1)
	{
		res = (_isp.uart->read)(_isp.uart, buff, sizeof(buff) );

		if(res > 0 )
			DEBUG_RAW(1, buff, res);
		if( !STC_ISP_CHECK_CMD( header, STC_ISP_CMD_REQUEST) )
		{
			AS_MSG_LOG(AS_LOG_WARNING, "read only length %d", res );
			if(count > 3)
				return 0;
		}
		else
			break;
		
		count++;
	};


	/*handshake */
	AS_MSG_LOG(AS_LOG_INFO, "Handshake.....");
#endif	
#if 0
	count = 0;
	while(1)
	{
//		res = (_isp.uart->write)(_isp.uart, buff, 3);
		
		if(_isp_check_cmd_frame(&_isp, STC_ISP_CMD_REQUEST) )
		{
			count++;
			if(count>100)
				return 0;
		}
		AS_MSG_LOG(AS_LOG_INFO, "Handshake OK");
	}
#endif
	for(count = 0; count< 1; count++)
	{
		if(__isp_cmd_frame(&_isp, STC_ISP_CMD_REQUEST) )
			return 1;
	}
	
		if(_isp_check_cmd_frame(&_isp, STC_ISP_CMD_REQUEST) )
		{
			return 0;
		}
		
	AS_MSG_LOG(AS_LOG_INFO, "Handshake OK");
	
	if(_isp_check_cmd_frame(&_isp, STC_ISP_CMD_ERASE) )
		return 0;

	if(_isp_program(&_isp) )
		return 0;

#if 0
	if(_isp_read_firm_version(&_isp) )
		return 0;
#endif
	
	if(_isp_check_cmd_frame(&_isp, STC_ISP_CMD_RESET_IAP) )
		return 0;
	
	close(_isp.binFd);
	as_cmd_uart_destroy();
	
	AS_MSG_LOG(AS_LOG_INFO, "RESULT : Hex Length is %d, wrote %d bytes!", _isp.imageSize, _isp.total );
	
	return 0;
}

