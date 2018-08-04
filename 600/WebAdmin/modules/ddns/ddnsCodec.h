#ifndef	___DDNS_CODEC_H__
#define	___DDNS_CODEC_H__
/*
* $Id: ddnsCodec.h,v 1.2 2007/05/30 19:23:02 lizhijie Exp $
*/

typedef enum
{
		BASE64		=	1,
		KEYMD5		=	2,
		BLOWFISH	=	3,
}code_type;

#define		MAXMD5SOURCESTRINGLEN		1024
#define		MD5STRINGLEN					16

#define		PADLEN							64

#define				UPPERBASE			0
#define				LOWERBASE			26
#define				DIGITBASE			52
#define				PLUS				62
#define				SLASH				63


//define the error code
#ifndef MY_RET_CODE_
#define MY_RET_CODE_

#define CODE_SUCCESS	0
#define LENGTH_ILLEGAL	1
#define ENCODE_ERROR	2
#define BUFFER_TOO_SMALL 3
#endif //error code

////////////////////////////////////////////////
//Blowfish algorithm
//
// Usage:
// construct a instance of the class.
// then call encode or decode function and ger result.
// if no key supply the class use a default key
// 
// Note:
// source input must round to 64 bit.


typedef struct
{
	unsigned long S[4][256];
	unsigned long P[18];
}blf_ctx;


typedef	struct
{
	unsigned char 	key[MAXKEYBYTES];
	unsigned 			keylen;
	blf_ctx 			c;
}blowfish_t;


#define	 BITSOFBYTE 		8


typedef	struct
{
	char		*m_pBuf;
	UINT	m_nSize;
	long		m_dwIndex;
}bitstream_t;

int bitstreamCreate(bitstream_t *stream, char *byData, UINT nBufLen);
int  bitstreamInitBuffer(bitstream_t *stream, UINT nBufLen);
int bitstreamSeek(bitstream_t *stream, long dwIndex);
int bitstreamRead(bitstream_t *stream, char *pBuf, UINT nBitCount);
int bitstreamWrite(bitstream_t *stream, char *pBuf, UINT nBitCount);
int bitstreamCopyBuffer(bitstream_t *stream, char *pBuf, UINT nBufLen);

long Blowfish_EnCode(blowfish_t *fish, char* src,char* dest,unsigned long len);
long Blowfish_DeCode(blowfish_t *fish, char* src,char* dest,unsigned long len);
void Blowfish_SetKey(blowfish_t *fish, const unsigned char* keybuf,unsigned int len);
void Blowfish_Create(blowfish_t *fish, unsigned char* keybuf, unsigned int len);

int lutil_b64_ntop(
	unsigned char const *src,
	size_t srclength,
	char *target,
	size_t targsize);
int lutil_b64_pton(
	char const *src,
	unsigned char *target, 
	size_t targsize);

int MDString (unsigned char* string, UINT nLen, unsigned char* digest);

#endif

