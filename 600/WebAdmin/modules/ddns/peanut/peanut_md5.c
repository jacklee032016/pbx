/*
* $Id: peanut_md5.c,v 1.3 2006/12/04 08:56:17 lizhijie Exp $
*/

#include <arpa/inet.h>
#include <fcntl.h>

#include "peanut.h"

/* typedef a 32 bit type */

/* Data structure for MD5 (Message Digest) computation */
typedef struct
{
	UINT4 i[2];				/* number of _bits_ handled mod 2^64 */
	UINT4 buf[4];				/* scratch buffer */
	unsigned char in[64];		/* input buffer */
	unsigned char digest[16];	/* actual digest after MD5Final call */
} MD5_CTX;

void MD5Init (MD5_CTX *mdContext);
void MD5Update (MD5_CTX *mdContext,unsigned char *inBuf,unsigned int inLen);
void MD5Final (unsigned char *digest,MD5_CTX *mdContext);

const char BaseTable[65]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static unsigned char PADDING[64] =
{
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G and H are basic MD5 functions: selection, majority, parity */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */
#define FF(a, b, c, d, x, s, ac) {(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define GG(a, b, c, d, x, s, ac) {(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define HH(a, b, c, d, x, s, ac) {(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define II(a, b, c, d, x, s, ac) {(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }


void MD5Init (MD5_CTX * mdContext)
{
	mdContext->i[0] = mdContext->i[1] = (UINT4)0;

	/* Load magic initialization constants.
	*/
	mdContext->buf[0] = (UINT4)0x67452301;
	mdContext->buf[1] = (UINT4)0xefcdab89;
	mdContext->buf[2] = (UINT4)0x98badcfe;
	mdContext->buf[3] = (UINT4)0x10325476;
}

void MD5Update (MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen)
{
	UINT4 in[16];
	int mdi;
	unsigned int i, ii;

	/* compute number of bytes mod 64 */
	mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

	/* update number of bits */
	if ((mdContext->i[0] + ((UINT4)inLen << 3)) < mdContext->i[0])
		mdContext->i[1]++;
	
	mdContext->i[0] += ((UINT4)inLen << 3);
	mdContext->i[1] += ((UINT4)inLen >> 29);

	while (inLen--)
	{
		/* add new character to buffer, increment mdi */
		mdContext->in[mdi++] = *inBuf++;

		/* transform if necessary */
		if (mdi == 0x40)
		{
			for (i = 0, ii = 0; i < 16; i++, ii += 4)
				in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
				(((UINT4)mdContext->in[ii+2]) << 16) |
				(((UINT4)mdContext->in[ii+1]) << 8) |
				((UINT4)mdContext->in[ii]);

			Transform (mdContext->buf, in);
			mdi = 0;
		}
	}
}

void MD5Final(unsigned char *digest,MD5_CTX *mdContext)
{
	UINT4 in[16];
	int mdi;
	unsigned int i, ii;
	unsigned int padLen;

	/* save number of bits */
	in[14] = mdContext->i[0];
	in[15] = mdContext->i[1];

	/* compute number of bytes mod 64 */
	mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

	/* pad out to 56 mod 64 */
	padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
	MD5Update (mdContext, PADDING, padLen);

	/* append length in bits and transform */
	for (i = 0, ii = 0; i < 14; i++, ii += 4)
		in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
		(((UINT4)mdContext->in[ii+2]) << 16) |
		(((UINT4)mdContext->in[ii+1]) << 8) |
		((UINT4)mdContext->in[ii]);
	
	Transform (mdContext->buf, in);

	/* store buffer in digest */
	for (i = 0, ii = 0; i < 4; i++, ii += 4)
	{
		mdContext->digest[ii] = (unsigned char)(mdContext->buf[i] & 0xFF);
		mdContext->digest[ii+1] =	(unsigned char)((mdContext->buf[i] >> 8) & 0xFF);
		mdContext->digest[ii+2] =	(unsigned char)((mdContext->buf[i] >> 16) & 0xFF);
		mdContext->digest[ii+3] =	(unsigned char)((mdContext->buf[i] >> 24) & 0xFF);
	}

	memcpy(digest,mdContext->buf,16);
}

/* Basic MD5 step. Transform buf based on in.
*/
void Transform (UINT4 *buf, UINT4 *in)
{
	UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

	/* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
	FF ( a, b, c, d, in[ 0], S11, 3614090360UL); /* 1 */
	FF ( d, a, b, c, in[ 1], S12, 3905402710UL); /* 2 */
	FF ( c, d, a, b, in[ 2], S13, 606105819UL); /* 3 */
	FF ( b, c, d, a, in[ 3], S14, 3250441966UL); /* 4 */
	FF ( a, b, c, d, in[ 4], S11, 4118548399UL); /* 5 */
	FF ( d, a, b, c, in[ 5], S12, 1200080426UL); /* 6 */
	FF ( c, d, a, b, in[ 6], S13, 2821735955UL); /* 7 */
	FF ( b, c, d, a, in[ 7], S14, 4249261313UL); /* 8 */
	FF ( a, b, c, d, in[ 8], S11, 1770035416UL); /* 9 */
	FF ( d, a, b, c, in[ 9], S12, 2336552879UL); /* 10 */
	FF ( c, d, a, b, in[10], S13, 4294925233UL); /* 11 */
	FF ( b, c, d, a, in[11], S14, 2304563134UL); /* 12 */
	FF ( a, b, c, d, in[12], S11, 1804603682UL); /* 13 */
	FF ( d, a, b, c, in[13], S12, 4254626195UL); /* 14 */
	FF ( c, d, a, b, in[14], S13, 2792965006UL); /* 15 */
	FF ( b, c, d, a, in[15], S14, 1236535329UL); /* 16 */

/* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
	GG ( a, b, c, d, in[ 1], S21, 4129170786UL); /* 17 */
	GG ( d, a, b, c, in[ 6], S22, 3225465664UL); /* 18 */
	GG ( c, d, a, b, in[11], S23, 643717713UL); /* 19 */
	GG ( b, c, d, a, in[ 0], S24, 3921069994UL); /* 20 */
	GG ( a, b, c, d, in[ 5], S21, 3593408605UL); /* 21 */
	GG ( d, a, b, c, in[10], S22, 38016083UL); /* 22 */
	GG ( c, d, a, b, in[15], S23, 3634488961UL); /* 23 */
	GG ( b, c, d, a, in[ 4], S24, 3889429448UL); /* 24 */
	GG ( a, b, c, d, in[ 9], S21, 568446438UL); /* 25 */
	GG ( d, a, b, c, in[14], S22, 3275163606UL); /* 26 */
	GG ( c, d, a, b, in[ 3], S23, 4107603335UL); /* 27 */
	GG ( b, c, d, a, in[ 8], S24, 1163531501UL); /* 28 */
	GG ( a, b, c, d, in[13], S21, 2850285829UL); /* 29 */
	GG ( d, a, b, c, in[ 2], S22, 4243563512UL); /* 30 */
	GG ( c, d, a, b, in[ 7], S23, 1735328473UL); /* 31 */
	GG ( b, c, d, a, in[12], S24, 2368359562UL); /* 32 */

/* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
	HH ( a, b, c, d, in[ 5], S31, 4294588738UL); /* 33 */
	HH ( d, a, b, c, in[ 8], S32, 2272392833UL); /* 34 */
	HH ( c, d, a, b, in[11], S33, 1839030562UL); /* 35 */
	HH ( b, c, d, a, in[14], S34, 4259657740UL); /* 36 */
	HH ( a, b, c, d, in[ 1], S31, 2763975236UL); /* 37 */
	HH ( d, a, b, c, in[ 4], S32, 1272893353UL); /* 38 */
	HH ( c, d, a, b, in[ 7], S33, 4139469664UL); /* 39 */
	HH ( b, c, d, a, in[10], S34, 3200236656UL); /* 40 */
	HH ( a, b, c, d, in[13], S31, 681279174UL); /* 41 */
	HH ( d, a, b, c, in[ 0], S32, 3936430074UL); /* 42 */
	HH ( c, d, a, b, in[ 3], S33, 3572445317UL); /* 43 */
	HH ( b, c, d, a, in[ 6], S34, 76029189UL); /* 44 */
	HH ( a, b, c, d, in[ 9], S31, 3654602809UL); /* 45 */
	HH ( d, a, b, c, in[12], S32, 3873151461UL); /* 46 */
	HH ( c, d, a, b, in[15], S33, 530742520UL); /* 47 */
	HH ( b, c, d, a, in[ 2], S34, 3299628645UL); /* 48 */

/* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
	II ( a, b, c, d, in[ 0], S41, 4096336452UL); /* 49 */
	II ( d, a, b, c, in[ 7], S42, 1126891415UL); /* 50 */
	II ( c, d, a, b, in[14], S43, 2878612391UL); /* 51 */
	II ( b, c, d, a, in[ 5], S44, 4237533241UL); /* 52 */
	II ( a, b, c, d, in[12], S41, 1700485571UL); /* 53 */
	II ( d, a, b, c, in[ 3], S42, 2399980690UL); /* 54 */
	II ( c, d, a, b, in[10], S43, 4293915773UL); /* 55 */
	II ( b, c, d, a, in[ 1], S44, 2240044497UL); /* 56 */
	II ( a, b, c, d, in[ 8], S41, 1873313359UL); /* 57 */
	II ( d, a, b, c, in[15], S42, 4264355552UL); /* 58 */
	II ( c, d, a, b, in[ 6], S43, 2734768916UL); /* 59 */
	II ( b, c, d, a, in[13], S44, 1309151649UL); /* 60 */
	II ( a, b, c, d, in[ 4], S41, 4149444226UL); /* 61 */
	II ( d, a, b, c, in[11], S42, 3174756917UL); /* 62 */
	II ( c, d, a, b, in[ 2], S43, 718787259UL); /* 63 */
	II ( b, c, d, a, in[ 9], S44, 3951481745UL); /* 64 */

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}


void hmac_md5(unsigned char * text, int text_len, unsigned char * key,int key_len, unsigned char * digest )
{
	MD5_CTX context;
	unsigned char k_ipad[65]; /* inner padding key XORd with ipad */
	unsigned char k_opad[65]; /* outer padding key XORd with opad */
	unsigned char tk[16];
	int i;

	/* if key is longer than 64 bytes reset it to key=MD5(key) */
	if (key_len > 64)
	{
		MD5_CTX tctx;
		MD5Init(&tctx);
		MD5Update(&tctx, key, key_len);
		MD5Final(tk, &tctx);

		key = tk;
		key_len = 16;
	}

	/*
	* the HMAC_MD5 transform looks like:
	*
	* MD5(K XOR opad, MD5(K XOR ipad, text))
	*
	* where K is an n byte key
	* ipad is the byte 0x36 repeated 64 times
	* opad is the byte 0x5c repeated 64 times
	* and text is the data being protected
	*/

	/* start out by storing key in pads */
	memset(k_ipad,0,sizeof(k_ipad));
	memcpy(k_ipad,key,key_len);
	memset(k_opad,0,sizeof(k_opad));
	memcpy(k_opad,key,key_len);

	/* XOR key with ipad and opad values */
	for (i=0; i<64; i++)
	{
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	/*
	* perform inner MD5
	*/
	MD5Init(&context); /* init context for 1st pass */
	MD5Update(&context, k_ipad, 64); /* start with inner pad */
	MD5Update(&context, text, text_len); /* then text of datagram */
	MD5Final(digest, &context); /* finish up 1st pass */

	/*
	* perform outer MD5
	*/
	MD5Init(&context); /* init context for 2nd pass */
	MD5Update(&context, k_opad, 64); /* start with outer pad */
	MD5Update(&context, digest, 16); /* then results of 1st hash */
	MD5Final(digest, &context); /* finish up 2nd pass */
}

unsigned char FindInTable(unsigned char c)
{
	int i=0;
	for(i=0;i<65;i++)
	{
		if(BaseTable[i]==c)
		{
			return i;
		}
	}

	return 0;
}

char * DecodeBase64(char * Source)
{
	unsigned char x1,x2,x3,x4,xt;
	int SrcLen,Times,i,iResult=0;
	
	char *Result=(char *)malloc(512);
	memset(Result,0,512);

	SrcLen=strlen(Source);
	Times=SrcLen / 4;
	
	for(i=0;i<Times;i++)
	{
		x1=FindInTable(Source[0+i*4]);
		x2=FindInTable(Source[1+i*4]);
		x3=FindInTable(Source[2+i*4]);
		x4=FindInTable(Source[3+i*4]);

		x1<<=2;
		xt=x2>>4;
		x1=x1 | xt;
		Result[iResult++]=x1;

		x2<<=4;
		if (x3==64)
			break;
		
		xt=x3>>2;
		x2=x2 | xt;
		x3<<=6;
		Result[iResult++]=x2;

		if (x4==64)
			break;
		
		x3=x3 | x4;
		Result[iResult++]=x3;
	}
	
	return Result;
}

char * EncodeBase64(char * Source)
	{

	int Times,LenSrc,i,iResult=0;
	unsigned char x1,x2,x3,x4,xt,xt1;
	
	char *Result=(char *)malloc(512);
	memset(Result,0,512);
	LenSrc=strlen(Source);
	
	if (LenSrc % 3==0) 
		Times=LenSrc/3;
	else 
		Times=LenSrc/3+1;
	
	for(i=0;i<Times;i++)
	{
		if (LenSrc>=(3*i+3))
		{
			xt=Source[0+i*3];
			xt>>=2;
			x1=BaseTable[xt];
			xt=Source[0+i*3];
			xt<<=4;
			xt&=48;
			xt1=Source[1+i*3];
			xt1>>=4;
			xt|=xt1;
			x2=BaseTable[xt];
			xt=Source[1+i*3];
			xt<<=2;
			xt&=60;
			xt1=Source[2+i*3];
			xt1>>=6;
			xt|=xt1;
			x3=BaseTable[xt];
			xt=Source[2+i*3] ;
			xt&=63;
			x4=BaseTable[xt];
		}
		else if (LenSrc>=3*i+2)
		{
			xt=Source[0+i*3];
			xt>>=2;
			x1=BaseTable[xt];
			xt=Source[0+i*3];
			xt<<=4;
			xt&=48;
			xt1=Source[1+i*3];
			xt1>>=4;
			xt|=xt1;
			x2=BaseTable[xt];
			xt=Source[1+i*3];
			xt<<=2;
			xt&= 60;
			x3=BaseTable[xt];
			x4='=';
		}
		else
		{
			xt=Source[0+i*3];
			xt>>=2;
			x1=BaseTable[xt];
			xt=Source[0+i*3] ;
			xt<<=4;
			xt&=48;
			x2=BaseTable[xt];
			x3='=';
			x4='=';
		}

		Result[iResult++]=(char)(x1);
		Result[iResult++]=(char)(x2);
		Result[iResult++]=(char)(x3);
		Result[iResult++]=(char)(x4);
		
	}
	
	return Result;
}

