/*
* $Id: as_isdn.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef  __AS_ISDN_H__
#define __AS_ISDN_H__

#ifdef  __cplusplus
	extern "C"
	{
#endif

#include "ask_isdn.h"
#include "asnt_isdn.h"

#define ANY_CHANNEL 0xff /* IE attribut for 'any channel' */

#define PDEBUG(mask, fmt, arg...) 			_printdebug(__FUNCTION__, __LINE__, mask, fmt, ## arg)
#define PERROR(fmt, arg...) 				_printerror(__FUNCTION__, __LINE__, fmt, ## arg)
#define PDEBUG_RUNTIME(mask, fmt, arg...) _printdebug(NULL, 0, mask, fmt, ## arg)
#define PERROR_RUNTIME(fmt, arg...) _printerror(NULL, 0, fmt, ## arg)
void _printdebug(const char *function, int line, unsigned long mask, const char *fmt, ...);
void printlog(const char *fmt, ...);
void _printerror(const char *function, int line, const char *fmt, ...);

void sprint(char *dst, unsigned int siz, char *fmt, ...);
#define SPRINT(dst, fmt, arg...) 	sprint(dst, sizeof(dst), fmt, ## arg)

#define DEBUG_CONFIG		0x0001
#define DEBUG_MSG 			0x0002
#define DEBUG_STACK 		0x0004
#define DEBUG_BCHANNEL 	0x0008
#define DEBUG_PORT 			0x0100
#define DEBUG_ISDN 			0x0110
#define DEBUG_SIP			0x0120 
#define DEBUG_H323			0x0130 
//#define DEBUG_KNOCK		0x0140
#define DEBUG_VBOX			0x0180
#define DEBUG_EPOINT 		0x0200
#define DEBUG_CALL 			0x0400
//#define DEBUG_STALL 		0x0800
#define DEBUG_CRYPT			0x1000
#define DEBUG_ROUTE		0x2000
#define DEBUG_IDLETIME		0x4000
#define DEBUG_LOG			0x7fff

// check any faulty malloc
#define MALLOC_CHECK_	1

/*
 * one of the bits must be enabled in order to write log files
 */
#define DEBUG_LOG	0x7fff

/* disable DSP_MODULE to override the kernel space mixer for mixing
 * between isdn cards. this is usefull to test the user space mixer which
 * is needed in order to mix audio data of other devices like h323 connections.
 * if this is disabled, kernel space mixing is used whenever data is
 * transferred between isdn cards, otherwhise user space mixer is used.
 * NOTE: disabling causes a bigger delay between isdn cards caused by the
 * user space. also it needs higher cpu load. it is only for debugging
 */

//#define DSP_MODULE

/* audio buffer for mixer and recording.
 * all partys within a call (most time two endpoints) write audio data to the buffer. this is used because
 * the buffer experience jitter. if the buffer is too small, jitter will cause drops and gaps.
 * if the buffer is too large, the delay is large. 768 is a good value to start with.
 */
#define PORT_BUFFER	768

/* keep this 0 for minimum delay */
#define ISDN_PRELOAD	0

/* the jitterlimit specifies the number of samples received too fast, before
 * it recognizes a stalling process.
 * but should NOT be less 256.
 */
#define ISDN_JITTERLIMIT 512	/* maximum samples received before dropping */

/* give sendmail program. if not inside $PATH, give absolute path here (e.g. "/usr/sbin/sendmail")
 */
#define SENDMAIL	"sendmail"

/* maximum number of redial/powerdial numbers to remember
 */
#define MAX_REMEMBER	50

/* leave it above 1024, because lower values can be unsafe, higher valuse cause
 * data larger than 512 bytes of hex strings.
 */
#define RSA_BITS	1536

/* 'goto' or 'menu' actions may cause infinite loops. they will be prevented by this limit.
 * Also other recrusions, like redialing the 'redial' action must be prevented.
 * increase it ONLY IF you have a deeper tree of rule sets, than the value given here.
 */
#define RULE_NESTING	10

/* special debugging for buffer overflow bugs
 * note: whenever a buffer gets strange values, the budetect function must
 * be modified to detect the change of these values. whenever it is detected,
 * an error message is given at budetect function.
 */
//#define BUDETECT_DEF

#ifdef BUDETECT_DEF
 #define BUDETECT	budetect(__FILE__, __LINE__, __FUNCTION__);
 void budetect(const char *file, int line, char *function);
#else
 #define BUDETECT	;
#endif


#define FLAG_PORT_USE		1
#define FLAG_PORT_PTP		(1<<1)



/* unsave */
#define UCPY strcpy
#define UNCPY strncpy
#define UCAT strcat
#define UNCAT strncat
#define UPRINT sprintf
#define UNPRINT snprintf
#define VUNPRINT vsnprintf


/* save strcpy/strncpy */

#define SCPY(dst, src) scpy(dst, src, sizeof(dst))
extern __inline__ void scpy(char *dst, char *src, unsigned int siz)
{
	strncpy(dst, src, siz);
	dst[siz-1] = '\0';
}

/* save strcat/strncat */

#define SCAT(dst, src) scat(dst, src, sizeof(dst))
extern __inline__ void scat(char *dst, char *src, unsigned int siz)
{
	strncat(dst, src, siz);
	dst[siz-1] = '\0';
}

/* save concat of a byte */

#define SCCAT(dst, src) sccat(dst, src, sizeof(dst))
extern __inline__ void sccat(char *dst, char chr, unsigned int siz)
{
	if (strlen(dst) < siz-1)
	{
		dst[strlen(dst)+1] = '\0';
		dst[strlen(dst)] = chr;
	}
}

/* support stuff */
__inline__ void strnncpy(unsigned char *dest, unsigned char *src, int len, int dst_len)
{
	if (len > dst_len-1)
		len = dst_len-1;
	UNCPY((char *)dest, (char *)src, len);
	dest[len] = '\0';
}

//void strnncpy(unsigned char *dest, unsigned char *src, int len, int dst_len);

void portdevs_info(void);


#define INACTIVE_EP		0
#define ACTIVE_EP		1

#define WITH_CRYPT		0


#define ISDN_DEV_TYPE_USER					0			/* device in user side */
#define ISDN_DEV_TYPE_NET					1			/* device in network side */


#ifdef __cplusplus
	};
#endif

#endif

