/*
* $Id: ddnsCodec.c,v 1.4 2007/05/31 20:38:05 lizhijie Exp $
*/

#include <ctype.h>
#include "_ddns.h"
#include "ddnsCodec.h"

static unsigned char chBase64Table[64] = 
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 
	'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', '+', '/'
};


static UINT __OnBase64Encode(unsigned char* szEncoded, unsigned char* szData, UINT nSize)
{
	char byCode;
	int nEncodedIndex = 0;
	int nBits = 0;
	int i;

	bitstream_t bs;
	if(!bitstreamCreate(&bs, (char *)szData, nSize))
		return 0;

	while((nBits = bitstreamRead(&bs, &byCode, 6)), nBits == 6)
	{
		byCode >>= 2;
		szEncoded[nEncodedIndex++] = chBase64Table[(int)byCode];
	}
	byCode = (byCode >> (BITSOFBYTE - nBits)) << (BITSOFBYTE - nBits);	//Remove the unused bits, it may be 1
	byCode >>= 2;
	szEncoded[nEncodedIndex++] = chBase64Table[(int)byCode];

	if(!(nSize % 3))
		return nEncodedIndex;

	for(i = nSize % 3; i < 3; i++)
		szEncoded[nEncodedIndex++] = '=';
	
	return nEncodedIndex;
}

//unsigned char to char
static UINT __OnBase64Decode(char* szDecoded, const char* szData, UINT nSize)
{
	char byCode;
//	int nBits = 0;
	UINT i;

	//printf("Base64Decode %s len:%d.\n", szData, nSize);
	bitstream_t bs;
	if(!bitstreamInitBuffer(&bs, nSize * 3 / 4 + 1))
		return 0;

	for(i = 0; i < nSize && szData[i] != '='; i++)
	{
		printf("%s ", &szData[i]);
		if(isalpha(szData[i]))
		{
			if(isupper(szData[i]))
				byCode = szData[i] - 'A' + UPPERBASE;
			else
				byCode = szData[i] - 'a' + LOWERBASE;
		}
		else if(isdigit(szData[i]))
			byCode = szData[i] - '0' + DIGITBASE;
		else if(szData[i] == '+')
			byCode = PLUS;
		else if(szData[i] == '/')
			byCode = SLASH;
		byCode <<= 2;
		
		bitstreamWrite(&bs, &byCode, 6);
	}
	bitstreamCopyBuffer(&bs, (char*)szDecoded, nSize * 3 / 4 + 1);

	printf("BASE64 Decode return len:%d %s.\n", (i - 1) * 3 / 4 + 1, szDecoded);

	return (i - 1) * 3 / 4 + 1;
}

static UINT __keyMD5Encode(unsigned char* szEncoded, const unsigned char* szData, UINT nSize, unsigned char* szKey, UINT nKeyLen)
{
	int i;
	//See rfc2104
	unsigned char ipad[PADLEN] = {0}, opad[PADLEN] = {0};
	memset(ipad, 0x36, PADLEN);
	memset(opad, 0x5c, PADLEN);
	
	unsigned char K[PADLEN] = {0};
	memcpy(K, szKey, nKeyLen);

	for(i = 0; i < PADLEN; i++)
	{
		ipad[i] ^= K[i];
		opad[i] ^= K[i];
	}
	unsigned char firstPad[PADLEN + MAXMD5SOURCESTRINGLEN + 1] = {0};
	unsigned char midResult[PADLEN + 16] = {0};
	
	memcpy(firstPad, ipad, PADLEN);
	memcpy(firstPad + PADLEN, szData, nSize);
	firstPad[PADLEN + nSize] = '\x0';
	MDString(firstPad, PADLEN + nSize, midResult + PADLEN);

	memcpy(midResult, opad, PADLEN);
	MDString(midResult, PADLEN + 16, szEncoded);

	return 16;
}

UINT __KeyEncode(ddns_task_t *task, unsigned char *result, unsigned char *src, UINT nSize, UINT nFormat)
{
	switch(nFormat)
	{
		case BLOWFISH:
				{
#if 1			
//	ddnsDebugBuffer("KEY", task->plainChallenge, task->challengeLength);
			Blowfish_SetKey(&task->blowfish, task->plainChallenge, task->challengeLength );
			Blowfish_EnCode(&task->blowfish, src, result, nSize);
#else
			BLOWFISH_context c;
			bf_setkey(&c, task->plainChallenge, task->challengeLength );
			encrypt_block(&c, src, result);
#endif
			return nSize;
			break;
			}
		case KEYMD5:
			return __keyMD5Encode(result, src, nSize, task->plainChallenge, task->challengeLength );
			break;
		default:
			return 0;
	}
}

UINT __KeyDecode(ddns_task_t *task, unsigned char *result, unsigned char *src, UINT nSize, UINT nFormat)
{
	////!!!20050218 CBlowfish bf; set to global bf
	switch(nFormat)
	{
		case BLOWFISH:
#if 1			
			Blowfish_SetKey(&task->blowfish, task->plainChallenge, task->challengeLength );
			Blowfish_DeCode(&task->blowfish, src, result, nSize);
#else
#endif
			return nSize;
			break;
		default:
			return 0;
	}
}

UINT __Encode(ddns_task_t *task, unsigned char* szEncoded, unsigned char* szData, UINT nSize, UINT nFormat)
{
	switch(nFormat)
	{
		case BASE64:
			return __OnBase64Encode(szEncoded, szData, nSize);
			break;
		default:
			return 0;
	}
}

//unsigned char to char
UINT __Decode(ddns_task_t *task, char* szDecoded, const char* szData, UINT nSize, UINT nFormat)
{
	switch(nFormat)
	{
		case BASE64:
			return __OnBase64Decode(szDecoded, szData, nSize);
			break;
		default:
			return 0;
	}
}



/* used when hand shake */
int GenerateCrypt(ddns_task_t *task, long clientinfo, long embkey, char *szResult)
{
	unsigned char szKey[256];

	long challengetime = 0;
	char szAscii[256];
	UINT nKey ;
	int nUser, nMoveBits;
	long challengetime_new;
	UINT nEncoded;

#if __ARM_IXP__
	{
		unsigned char *p = (unsigned char *)&challengetime;
		*p = task->plainChallenge[9];
		*(p+1) = task->plainChallenge[8];
		*(p+2) = task->plainChallenge[7];
		*(p+3) = task->plainChallenge[6];
		AS_MSG_DEBUG(AS_LOG_NOTICE, "byte 1 : 0x%x(%d)", task->plainChallenge[6] , task->plainChallenge[6] );
		AS_MSG_DEBUG(AS_LOG_NOTICE, "byte 2 : 0x%x(%d)", task->plainChallenge[7] , task->plainChallenge[7] );
		AS_MSG_DEBUG(AS_LOG_NOTICE, "byte 3 : 0x%x(%d)", task->plainChallenge[8] , task->plainChallenge[8] );
		AS_MSG_DEBUG(AS_LOG_NOTICE, "byte 4 : 0x%x(%d)", task->plainChallenge[9] , task->plainChallenge[9] );
	}
#else
	challengetime = *((long*)(task->plainChallenge + 6));
	AS_MSG_DEBUG(AS_LOG_NOTICE, "challengetime 1 : 0x%x", challengetime );
#if 0//__ARM_IXP__
	/* following can not be used bacause challengetime can not get correct value in big-endian platform , lizhijie, 2007.05.29 */
	challengetime = as_Big2Little(challengetime);
#endif
#endif
	AS_MSG_DEBUG(AS_LOG_NOTICE, "challengetime 1 : 0x%x", challengetime );
	challengetime |= ~embkey;
	nMoveBits = challengetime % 30;
	
	AS_MSG_DEBUG(AS_LOG_NOTICE, "challengetime 1 : 0x%x, Right shift %d bits", challengetime, nMoveBits);

#if __ARM_IXP__
	{
		int bits = (abs(32 - nMoveBits))%32;
		long t1 = (challengetime << bits );
		if(nMoveBits<0)
			nMoveBits = 32+nMoveBits;
		long t2 = (challengetime >> nMoveBits) ;
		long t3 = ~(0xffffffff << (bits));
		challengetime_new =  t1|(t2 & t3) ;
	
		AS_MSG_DEBUG(AS_LOG_WARNING, "After right shift %d bits, challengetime is 0x%x\n \t0x%x\n \t0x%x\n \t0x%x", 
			nMoveBits , challengetime_new, t1, t2, t3);
	}
#if 0
	{
		AS_MSG_LOG(AS_LOG_NOTICE, "challengetime 2 : 0x%x", challengetime);
		unsigned long tmp = challengetime_new;
		unsigned char *p1 = (unsigned char *)&challengetime_new;
		unsigned char *p2 = (unsigned char *)&tmp;
		*p1 = p2[3];
		*(p1+1) = p2[2];
		*(p1+2) = p2[1];
		*(p1+3) = p2[0];
		AS_MSG_LOG(AS_LOG_NOTICE, "challengetime 3 : 0x%x", challengetime_new);
		p1 = (unsigned char *)&sendClientInfo;
		p2 = (unsigned char *)&clientinfo;
		*p1 = p2[3];
		*(p1+1) = p2[2];
		*(p1+2) = p2[1];
		*(p1+3) = p2[0];
	}
#else
	challengetime_new = as_Big2Little(challengetime_new);
	clientinfo = as_Big2Little(clientinfo);
#endif
#else
	challengetime_new = (challengetime << (32 - nMoveBits)) | ((challengetime >> nMoveBits) & ~(0xffffffff << (32 - nMoveBits)));
#endif

	AS_MSG_LOG(AS_LOG_DEBUG,"0x%x Right Shift %d bits is 0x%x",challengetime, nMoveBits, challengetime_new);

	nKey = __KeyEncode(task, szKey, (unsigned char*)task->passwd, strlen(task->passwd), KEYMD5);
	szKey[nKey] = 0;
	AS_MSG_LOG(AS_LOG_DEBUG,"password encrypted length is %d",nKey);
	
	nUser = strlen(task->userName );
	//printf("Password Encode Key len:%d %s nUser:%d.\n", nKey, szKey, nUser);

	//modified by oray/skyvense, for ph35
	memcpy(szAscii, task->userName, nUser);
	szAscii[nUser] = ' ';
	memcpy(szAscii+nUser+1, &challengetime_new, 4);
	memcpy(szAscii+nUser+1+4, &clientinfo, 4);
//	memcpy(szAscii+nUser+1+4, &sendClientInfo, 4);
	memcpy(szAscii+nUser+1+4+4, szKey, nKey);
	//UINT nEncoded = cr.Encode((unsigned char *)szResult, (unsigned char *)szAscii, nUser + 1 + nKey, BASE64); 
	//UINT nEncoded = cr.Encode((unsigned char *)szResult, (unsigned char *)szDecoded, nDecodedLen, BASE64); //TEST
	nEncoded =  lutil_b64_ntop((unsigned char *)szAscii, nUser + 1 + 4 + 4 + nKey, szResult, 256);

	AS_MSG_DEBUG(AS_LOG_DEBUG,"encoding length : %d, user len : %d; User :%s", nUser + 1 + 4 + 4 + nKey, nUser, task->userName);


	AS_MSG_DEBUG(AS_LOG_DEBUG, "BASE64 Encode:%s outputsize:%d Encodelen:%d.\n", szResult, sizeof(szResult), nEncoded);

	return nEncoded;
}

/* used when send out heart beat */
void CryptBlow(ddns_task_t *task, char *crypted)
{
	char szRet[16];

	__KeyEncode(task, (unsigned char*)szRet, (unsigned char*)crypted, 16, BLOWFISH);
	memcpy(crypted, szRet, 16);
}

/* used when receive heart beat */
void DecryptBlow(ddns_task_t *task, char *decodings)
{
	char szRet[16];

	__KeyDecode(task, szRet, (unsigned char*)decodings, 16, BLOWFISH);
	memcpy(decodings, szRet, 16);
}


#if		DDNS_DEBUG
void ddnsDebugBuffer(char *title, unsigned char *buf, int len)
{
	int	i;
#if 1	
	AS_MSG_LOG(AS_LOG_ERR, "%s dump : Length is %d" , title, len);
#else
	printf( "%s dump : Length is %d\n" , title, len);
#endif
	if (len > 0)
	{
		for (i = 0; i < len; i++)
		{
//			if ((i & 1) == 0)
//				AS_MSG_LOG(AS_LOG_ERR, " ");
#if 1
			AS_MSG_LOG(AS_LOG_ERR, "0x%02x", buf[i]);
#else
			printf("0x%02x ", buf[i]);
#endif
		}
#if 1
		AS_MSG_LOG(AS_LOG_ERR, "");
#else
		printf("\n\n");
#endif
	}
}
#endif

long  as_Big2Little(long raw)
{
	long tmp;
	
	unsigned char *p1 = (unsigned char *)&tmp;
	unsigned char *p2 = (unsigned char *)&raw;
	*p1 = p2[3];
	*(p1+1) = p2[2];
	*(p1+2) = p2[1];
	*(p1+3) = p2[0];

	return tmp;
}

