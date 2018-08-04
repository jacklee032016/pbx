/*
* $Id: ddnsTestBlow.c,v 1.1 2007/05/30 19:20:19 lizhijie Exp $
*/
#include "_ddns.h"

/*
*
Before Blow dump
2e 4f 37 4e 0a 00 00 00 3b 25 2f 3a bb da d0 c5 00 00 00 00
2e 4f 37 4e 0a 00 00 00 3b 25 2f 3a bb da d0 c5 00 00 00 00
 
After Blow dump : Length is 20 
2e 4f 37 4e 2a af a2 46 eb 2b aa c2 ac ed 5b 0b df 66 84 6b
2e 4f 37 4e 2a af a2 46 eb 2b aa c2 ac ed 5b 0b df 66 84 6b 

*/
ddns_task_t	testDdns;

int main(int argc, char *argv[])
{
	unsigned char *strChallenge64 = "3tQK4oABNIFdRhlAJxBtRw==";
	UpdatePack up;

	ddns_task_t *task = &testDdns;
	task->challengeLength =  lutil_b64_pton(strChallenge64, task->plainChallenge, DDNS_DEFAULT_BUFFER_LENGTH );

	task->sessionCode = 0x4e374f2e;
	task->sequence = 0x3a2f253b;

	__initPacket( task, &up);

#if DDNS_DEBUG
	ddnsDebugBuffer("Before Blow", (char*)&up, 20);
#endif
	CryptBlow(task,  (char*)&up.nOperateCode);
#if DDNS_DEBUG
	ddnsDebugBuffer("After Blow", (char*)&up, 20);
#endif

	{
		char buf[] = 
			{
				0x6b, 0xa0, 0x81, 0xb1,
				0x44, 0x6f, 0xbe, 0xef,
				0x86, 0x39, 0x2b, 0x48,
				0x8f, 0xae, 0x9b, 0xb9
			};
		ddnsDebugBuffer("RX Before Blow", buf, 16);
		DecryptBlow(task, buf);
		ddnsDebugBuffer("RX After Blow", buf, 16);
	}

	return 0;
}

