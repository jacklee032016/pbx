/*
* $Log: as_fsk.h,v $
* Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
* AS800 PstnAPI
*
* Revision 1.12  2006/05/30 03:04:27  lizhijie
* optimzed DTMF decoder and add DSP configuration interface
*
* Revision 1.11  2006/05/28 10:14:58  lizhijie
* dd
*
* Revision 1.10  2006/05/25 03:19:41  lizhijie
* add pstn_standard
*
* Revision 1.9  2006/05/23 02:54:17  lizhijie
* dd
*
* Revision 1.8  2006/05/19 08:46:59  lizhijie
* add fsk demodulation in buffer
*
* Revision 1.7  2006/04/19 12:28:39  lizhijie
* add CAR support and others
*
* Revision 1.6  2006/04/19 12:24:26  lizhijie
* no message
*
* Revision 1.5  2006/03/20 06:46:37  lizhijie
* add and debug some. refer to ChangeLog
*
* Revision 1.4  2006/03/20 06:36:48  lizhijie
* no message
*
* Revision 1.3  2006/03/17 09:26:25  wangwei
* no message
*
* Revision 1.2  2006/03/17 07:58:10  lizhijie
* add busy tone detect and debug problem of DTMF detect
*
* $Author: lizhijie $
*/

#ifndef  __AS_FSK_H__
#define  __AS_FSK_H__

#include "assist_lib.h"

#define	FSK_DEBUG						1

#define 	FS								8000 

#define 	NBW							2
#define 	BWLIST							{75,800}
#define	NF								4
#define	FLIST 							{1400,1800,1200,2200}

#define 	NSBUF							1024
#define 	NCOLA							0x4000

#ifndef	ELAST
#define	ELAST 							500
#endif

#ifndef 	AS_ELAST
#define 	AS_ELAST 						500
#endif


typedef 	struct	fsk_data					fsk_data_t;
typedef	struct	as_fsk_decode_engine		fsk_decoder_t;

/* used internally, but needs to be here so that ZAP structure will be valid */
struct fsk_data
{
	float 			spb;	/* Samples / Bit */
	int 				nbit;	/* Número de bits de datos (5,7,8) */
	float 			nstop;	/* Número de bits de stop 1,1.5,2  */
	int 				paridad;	/* Bit de paridad 0=nada 1=Par 2=Impar */
	int 				hdlc;	/* Modo Packet */

	int 				bw;		/* Ancho de Banda */

	int 				f_mark_idx;	/* Indice de frecuencia de marca (f_M-500)/5 */
	int 				f_space_idx;/* Indice de frecuencia de espacio (f_S-500)/5 */
	
	int 				pcola;				/* Puntero de las colas de datos */
	float 			cola_in[NCOLA];		/* Cola de muestras de entrada */
	float 			cola_filtro[NCOLA];	/* Cola de muestras tras filtros */
	float 			cola_demod[NCOLA];	/* Cola de muestras demoduladas */
} ;

typedef enum
{
	FSK_STAGE_UNINIT		= 0,
	FSK_STAGE_INITED,				/* 1 , */
	FSK_STAGE_FIRST_DEL,
	FSK_STAGE_PARSING	,			/* 2 , parse is not end but some part has been detect out, other buffer should be feed continue*/
	FSK_STAGE_PARSING_PARAM,		/* 3 , now parsing parameters in msg, so STX and DLE can tackled specially */
	FSK_STAGE_END					/* 4 , end, reset the engine */
}fsk_stage_t;

typedef enum
{
	NTT_FSK_PARSE_DISABLE  	= 0,
	NTT_FSK_PARSE_HEADER  ,
	NTT_FSK_PARSE_NUMBER,			/* parsing caller ID */
	NTT_FSK_PARSE_CALLED,				/* parse Called Phone number, for DID */
	NTT_FSK_PARSE_DATE,				/* date in format of  mm:dd:hh:mm */
	NTT_FSK_PARSE_END				/* all parameters of this msg has been parsed, this must be last element in this enum */
}ntt_fsk_parse_type;


struct  as_fsk_decode_engine
{
	fsk_data_t  		fskd;							/* fsk data */
	int				bp;								/* fsk read buffer pointer */
	int				ns;								/* fsk read silly thingy */
	float				x0,cont;							/* stuff for dpll */
	int				mode;							/* fsk transmit mode */
	int				modo;							/* fsk receive mode */
	unsigned char 	audio_buf[AS_DTMF_BLOCKSIZE];  	/* fsk read audio buffer */
//	int	p;		/* fsk transmit buffer pointer */
	double 			fmxv[8],fmyv[8];					/* filter stuff for Mark filter */
	int				fmp;							/* pointer for Mark filter */
	double 			fsxv[8],fsyv[8];					/* filter stuff for Space filter */
	int				fsp;								/* pointer for Space filter */
	double 			flxv[8],flyv[8];					/* filter stuff for L filter */
	int				flp;								/* pointer for L filter */
//	float lxv[13],lyv[13];	/* filter stuff for low group DTMF filter */
//	float hxv[9],hyv[9];	/* filter stuff for high group DTMF filter */

//	int	nxfer;		/* number of bytes transfered last */
//	unsigned int nsilence;	/* number of contiguous silence samples */
//	int	mflevel;	/* accumulator for MF det. level */
	float				scont;		/* stuff for CLID sending */
//	unsigned short recbuf[ZAP_BLOCKSIZE * 2];
//	int recsamps;

//	int law;

	fsk_stage_t			stage;		/* stage for detect from buffer */
	ntt_fsk_parse_type		parseType;
	pstn_param_t			param;

	int					msgLength;
	int					dataLength;		/* data length of current parameter */
	int					position;
};

typedef enum 
{
	/* some parameters such as ringoff timer, FSK mark/spare frequency are different 
	* for Japan and China, Lizhijie
	*/
	AS_FSK_LOCALE_JAPAN = 0,			
	AS_FSK_LOCAL_CHINA   = 1			/* 1200 HZ */
}fsk_locale_type;


#define FSK_LOCAL_JAPAN		1

#if  0//FSK_LOCAL_JAPAN
	#define 	RING_OFF_LENGTH			2000	/* ms */
	/* delay for FSK data after ringer off, for Japan about 500ms , need to test in future, lizhjie, 2006.03.16 */
	//#define	RINGOFF_BEFORE_FSK		2000	/* ms */
	#define	RINGOFF_BEFORE_FSK		2000	/* ms */
	
	#define 	FSK_FREQUENCY_MARK		1300.0
	#define	FSK_FREQUENCY_SPARE		2100.0

	#define	FREQ_SPARE_REAL			(-7.845910e-02)	
	#define	FREQ_SPARE_IMAGE			(9.969173e-01)
	#define	FREQ_MARK_REAL			(5.224985e-01)
	#define	FREQ_MARK_IMAGE			(8.526402e-01)
#else
	#define 	RING_OFF_LENGTH			4000	/* ms */
	/* delay for FSK data after ringer off, for China 2000ms is good for mant phone, lizhjie, 2006.03.16 */
	#define	RINGOFF_BEFORE_FSK		2000	/* ms */
	
	#define 	FSK_FREQUENCY_MARK		1200.0
	#define	FSK_FREQUENCY_SPARE		2200.0
	
	#define	FREQ_SPARE_REAL			-1.564345e-01	
	#define	FREQ_SPARE_IMAGE			9.876884e-01
	#define	FREQ_MARK_REAL			5.877852e-01
	#define	FREQ_MARK_IMAGE			8.090170e-01

#endif

typedef enum 
{
	AS_FSK_SPACE = 0,			/* 2200 HZ */
	AS_FSK_MARK = 1			/* 1200 HZ */
}fsk_carrier_type;


typedef struct 
{
//	unsigned short 			*buf; /*[AS_DTMF_BLOCKSIZE];  write audio buffer ,length is get from ioctl */
	unsigned char 			*buf; /*[AS_DTMF_BLOCKSIZE];  write audio buffer ,length is get from ioctl */
	int 						bufp;
}fsk_tone_data_t;


typedef struct 
{
	float	ci;		/* ci for local (non-fsk) tone generation */
	float	cr;		/* cr for local (non-fsk) tone generation */
#if 0	
	float	ci1;		/* ci1 for local (non-fsk) tone generation */
	float	cr1;		/* cr1 for local (non-fsk) tone generation */
#endif	
}fsk_calculator_t;


typedef struct 
{/* keep float for space and mark */
	float 					dr[2];
	float 					di[2];
	float 					scont ; /* initialize fractional time accumulator */
}fsk_encoder_t;

typedef struct
{
}dtmf_gen;

typedef enum
{
	FSK_BUFFER_PSTN			= 0,		/* fd is a device file of PSTN */
	FSK_BUFFER_NORMAL_FILE			/* fd is a normal file */
}fsk_buf_type_t;

typedef struct 
{
	fsk_buf_type_t			buf_type;
	
	int 						buf_size;
	
	fsk_tone_data_t			tone_data;
	fsk_tone_data_t			voice_data;
}fsk_config_t;

/* length about  [(128*67)%160+1]*160 + preamble mark bit  */
#define	FSK_MODEM_BUFFER_LENGTH			(10000)

typedef struct
{
	law_t					law;		
	fsk_calculator_t			cal;
	fsk_encoder_t				encoder;
	fsk_decoder_t				decoder;
	
	fsk_config_t				*config;

	unsigned char 			modem_buf[FSK_MODEM_BUFFER_LENGTH];  	/* used for FSK modulation(FXS) and demodulation(FXO) */
	int						modemLength;							/* how many bytes are used in this buffer */

	unsigned char				*inputBuf;
	
	as_dsp_t					*root;
}fsk_t;



int as_fsk_clid( fsk_t *dsp, int fd, char *number, char *name );

#define		FSK_IO_ERROR					-1
#define		FSK_IO_AGAIN					-2

#define		FSK_PUT_QUIT					-1
#define 		FSK_PUT_CONTINUE				-2


#define		FSK_FORMAT_MDMF				0x80
#define		FSK_FORMAT_SDMF				0x04

#define		FSK_MSG_TYPE_DATE			0x01
#define		FSK_MSG_TYPE_NUMBER			0x02
#define		FSK_MSG_TYPE_NUMBER_NO		0x04
#define		FSK_MSG_TYPE_NAME			0x07
#define		FSK_MSG_TYPE_NAME_NO		0x08

extern 	int 		as_fsk_serie( fsk_t *dsp, int fd);
extern 	float 	filtroM( fsk_decoder_t *engine,float in);
extern 	float 	filtroS(fsk_decoder_t *engine, float in);
extern 	float 	filtroL(fsk_decoder_t *engine,float in);

extern	unsigned char __baudot(fsk_decoder_t *engine,unsigned char data);
extern	float 	__as_fsk_get_carrier(fsk_t *dsp, fsk_carrier_type type);

extern	int 		as_dsp_lib_ring_with_fsk_callerid(fsk_t *dsp, int fd , unsigned char *number , unsigned char *name );
extern	int 		as_dsp_lib_ring_with_ntt_callerid(fsk_t *dsp, int fd , unsigned char *number  );
extern	int 		as_fsk_decode_clid_from_fd( fsk_t *dsp, int fd, char *number, char *name );
extern	int 		as_fsk_decode_ntt_clid_from_fd( fsk_t *dsp, int fd, char *number);

#if 0
extern	pstn_param_t *as_fsk_buf_decode_ntt_clid(fsk_t *dsp, unsigned char *rawbuf, int length );
#endif
extern	pstn_param_t *as_fsk_decode_chinese_clid( fsk_t *dsp, unsigned char *rawbuf, int length );
extern	pstn_param_t *as_fsk_decode_ntt_clid(fsk_t *dsp, unsigned char *rawbuf, int length );
/* return : buffer with FSK modulated data */
extern	unsigned char *as_dsp_lib_encode_ntt_callerid(fsk_t *dsp, unsigned char *number , int *returnLength );
extern	unsigned char *as_dsp_lib_encode_chinese_callerid(fsk_t *dsp,  unsigned char *number , unsigned char *name, int *returnLength );

/* interface exposed to dsp.c */
extern	fsk_t 	*as_dsp_new_fsk( as_dsp_t *root );
extern	void 	as_dsp_destroy_fsk(fsk_t *dsp);
extern	int 		as_fsk_decode(fsk_t *dsp, int fd, char *number, char *name );
extern	pstn_param_t *as_fsk_ntt_decode_buf(fsk_t *dsp, unsigned char *rawbuf, int length);

extern	int 		as_fsk_ring_with_callerid(fsk_t *dsp, int fd , char *number , char *name );

extern	void 	as_fsk_crc_init(void);
extern	int 		as_fsk_ntt_decode(fsk_t *dsp, int fd, char *number);
extern	int 		as_fsk_ntt_ring_with_callerid(fsk_t *dsp, int fd , char *number );

/* NTT specs p.77 */
#define 	NTT_FSK_DLE					0x10
#define 	NTT_FSK_SOH					0x01	/* start of header */
#define	NTT_FSK_HEADER				0x07
#define	NTT_FSK_STX					0x02	/* start of text */
#define	NTT_FSK_SERVICE_TYPE			0x40
#define	NTT_FSK_EXT					0x03	/* end of text */	
#define	NTT_FSK_SI						0x0f	
#define	NTT_FSK_SO						0x0e

#define	NTT_FSK_PARITY_BIT			0x80

/* until Service Type in Frame header */
#define	NTT_FSK_HEADER_LENGTH		6

/* NTT specs p.79 */
#define	NTT_FSK_PARAM_NUMBER				0x02
#define	NTT_FSK_PARAM_REASON				0x04
#define	NTT_FSK_PARAM_EXPAND				0x21
/* NTT specs p.60 */
#define	NTT_FSK_PARAM_CALLED_NUMBER		0x09
#define	NTT_FSK_PARAM_CALLED_EXTENSION		0x22



#define	NTT_FSK_BLOCK_LENGTH			128
#define	NTT_FSK_MARK_DURATION		60


#endif

