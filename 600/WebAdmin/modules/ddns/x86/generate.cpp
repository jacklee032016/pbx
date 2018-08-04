#include <stdio.h>  
#include "generate.h"
#include "coder.h"
#include "lutil.h"
  

//__stdcall
int GenerateCrypt(LPCTSTR szUser, 
							 LPCTSTR szPassword, 
							 LPCTSTR szChallenge64, 
                                                         long clientinfo,
                                                         long embkey,
							 LPTSTR szResult)
{
	CCoder cr;

	UCHAR szDecoded[256];
	UCHAR szKey[256];

	//BYTE szDecoded[256];
	//(BYTE*)szChallenge64
	//UINT nDecodedLen = cr.Decode(szDecoded, szChallenge64, strlen((char*)szChallenge64), BASE64, NULL);
	UINT nDecodedLen =  lutil_b64_pton(szChallenge64, szDecoded, 256);
	//char const *src,
	//unsigned char *target, 
	//size_t targsize)
	//printf("Decode challenge64 len:%d %s.\n", nDecodedLen, szDecoded);


	//modified by oray/skyvense, for ph35
	long challengetime = 0;
	challengetime = *((long*)(szDecoded + 6));
	printf("Challengetime : 0x%x\n", challengetime);
	challengetime |= ~embkey;
	int nMoveBits = challengetime % 30;

long challengetime_new;
	printf("Challengetime : 0x%x, right shift %d bits\n", challengetime, nMoveBits );
	{
		long t1 = (challengetime << (32 - nMoveBits));
		long t2 = (challengetime >> nMoveBits) ;
		long t3 = ~(0xffffffff << (32 - nMoveBits));
		challengetime_new =  t1|(t2 & t3) ;
	
		printf("After right shift %d bits, challengetime is 0x%x\n \t0x%x\n \t0x%x\n \t0x%x\n", 
			nMoveBits , challengetime_new, t1, t2, t3);
	}
	//(BYTE*)szPassword,
	UINT nKey = cr.KeyEncode(szKey, (unsigned char*)szPassword, strlen((char*)szPassword), (unsigned char*)szDecoded, nDecodedLen, KEYMD5);
	szKey[nKey] = 0;
	
	int nUser = strlen((char *)szUser);
	//printf("Password Encode Key len:%d %s nUser:%d.\n", nKey, szKey, nUser);

	BYTE szAscii[256];

	//modified by oray/skyvense, for ph35
	memcpy(szAscii, szUser, nUser);
	szAscii[nUser] = ' ';
	memcpy(szAscii+nUser+1, &challengetime_new,4);
	memcpy(szAscii+nUser+1+4,&clientinfo,4);
	memcpy(szAscii+nUser+1+4+4, szKey, nKey);
	//UINT nEncoded = cr.Encode((UCHAR *)szResult, (UCHAR *)szAscii, nUser + 1 + nKey, BASE64); 
	//UINT nEncoded = cr.Encode((UCHAR *)szResult, (UCHAR *)szDecoded, nDecodedLen, BASE64); //TEST
	UINT nEncoded =  lutil_b64_ntop((UCHAR *)szAscii, nUser + 1 + 4 + 4 + nKey, szResult, 256);
//	unsigned char const *src,
//	size_t srclength,
//	char *target,
//	size_t targsize)

	//szResult[nEncoded] = 0;
#if  1//def MYDEBUG
	printf("BASE64 Encode:%s outputsize:%d Encodelen:%d.\n", szResult, sizeof(szResult), nEncoded);
#endif
	
	return nEncoded;
}

//__stdcall
void CryptBlow(LPCTSTR szChallenge64, BYTE* pData)
{
	CCoder cr;

	UCHAR szDecoded[256];
	//BYTE szDecoded[256];
	//UINT nDecodedLen = cr.Decode(szDecoded, szChallenge64, strlen((char *)szChallenge64), BASE64);
	UINT nDecodedLen =  lutil_b64_pton(szChallenge64, szDecoded, 256);

	BYTE szRet[16];
	UINT nKey = cr.KeyEncode((UCHAR*)szRet, (UCHAR*)pData, 16, (UCHAR*)szDecoded, nDecodedLen, BLOWFISH);
	memcpy(pData, szRet, 16);

}

//__stdcall
void DecryptBlow(LPCTSTR szChallenge64, BYTE* pData)
{
	CCoder cr;


	UCHAR szDecoded[256];
	//BYTE szDecoded[256];
	//UINT nDecodedLen = cr.Decode(szDecoded, szChallenge64, strlen((char *)szChallenge64), BASE64);
	UINT nDecodedLen =  lutil_b64_pton(szChallenge64, szDecoded, 256);

	BYTE szRet[16];
	UINT nKey = cr.KeyDecode((UCHAR*)szRet, (UCHAR*)pData, 16, (unsigned char*)szDecoded, nDecodedLen, BLOWFISH);
	memcpy(pData, szRet, 16);

}
