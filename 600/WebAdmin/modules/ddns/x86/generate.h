#ifndef __GENERATE_H_ZHY_0505241025__
#define __GENERATE_H_ZHY_0505241025__

#include "mylinuxdef.h"
#define LPCTSTR char *
#define LPTSTR char *

//__stdcall
extern "C"
int GenerateCrypt(LPCTSTR szUser, 
							LPCTSTR szPassword, 
							LPCTSTR szChallenge64, 
							long clientinfo,
							long embkey, 
							LPTSTR szResult
							 );
//__stdcall
extern "C" void CryptBlow(LPCTSTR szChallenge64, BYTE* pData);
//__stdcall
extern "C" void DecryptBlow(LPCTSTR szChallenge64, BYTE* pData);

#endif
