/*
* $Id: ddnsShakehand.c,v 1.3 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "_ddns.h"

#define	DDNS_TCP_RESPONSE				220
#define	DDNS_TCP_QUIT						221

#define	DDNS_AUTH_FAIL					"250"

#define	DDNS_SHAKE_REQUEST				"AUTH ROUTER"
#define	DDNS_SHAKE_QUIT					"QUIT"

static int __readOneLine(int s, unsigned char * lpszBuf, int nBufLen)
{
	int nReadCount = 0;
	int nRecv = 0;

	lpszBuf[0] = '\0';

	for (; ; )
	{
		nRecv = recv(s, lpszBuf + nReadCount, 1, 0);
#if 0		
		AS_MSG_LOG(AS_LOG_NOTICE,"recvoneline:%s,count:%d\n",lpszBuf,nRecv);
		if (nRecv == 0)
			continue;
		else 
#endif			
			if (nRecv > 0)
		{
			nReadCount += nRecv;

			if (lpszBuf[nReadCount - 1] == '\n')
			{
				if (nReadCount > 1 && lpszBuf[nReadCount - 2] == '\r')
				{
					lpszBuf[nReadCount - 2] = '\0';
				}
				else
				{
					lpszBuf[nReadCount - 1] = '\0';
				}
				break;
			}

			if (nReadCount == nBufLen)
			{
				break;
			}
		}
		else
		{
			nReadCount = -1;
			break;
		}
	}
	
	return nReadCount;
}


int	__open_ddns_handshake(ddns_task_t *task )
{
	int shakeSocket = 0;		/* Shake socket descriptor */
	int bFlag = 1;
	struct sockaddr_in servAddr;	/* Server address struct for both shake and data */
	int nMaxBuf = 64000;
	
	if ((shakeSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "TCP socket() failed : %s", strerror(errno) );
		return -1;
	}

	/* Enlarge Socket Receiving Buffer*/
	if(-1 == setsockopt(shakeSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nMaxBuf, sizeof(nMaxBuf)))
	{
		AS_MSG_LOG(AS_LOG_ERR, "TCP enlarge RX buffer failed : %s", strerror(errno) );
		return -1;
	}

	/* Enlarge Socket Sending Buffer*/
	nMaxBuf = 64000;
	if(-1 == setsockopt(shakeSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nMaxBuf, sizeof(nMaxBuf)))
	{
		AS_MSG_LOG(AS_LOG_ERR, "TCP enlarge RX buffer failed : %s", strerror(errno) );
		return -1;
	}

	if(-1 == setsockopt(shakeSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&bFlag, sizeof(bFlag)))
	{
		AS_MSG_LOG(AS_LOG_ERR, "TCP enlarge ReuseAddress failed : %s", strerror(errno) );
		return -1;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	//servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	memcpy( &servAddr.sin_addr.s_addr ,task->serverHost->h_addr, sizeof( servAddr.sin_addr.s_addr ) );
	servAddr.sin_port = htons(task->tcpShakePort); /* Server port */

	if (connect(shakeSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Internet is unreachable or the DDNS Server is down! %s", strerror(errno) );
		close(shakeSocket);
		return -1;
	}

	AS_MSG_LOG(AS_LOG_NOTICE, "connect Server : '%s(%s)'\tPort : '%i' succeed.", 
		task->serverIp, asInetNtoa(servAddr.sin_addr.s_addr), task->tcpShakePort);

	task->shakeHandSocket = shakeSocket;
	return 0;
}

int	__ddns_shakehand_authen(ddns_task_t *task)
{
	unsigned char recvbuf[256];
	unsigned char *strChallenge64 = NULL;//"3tQFgYAfTFpcRrl+tgBwMA==";
	char strCryptStr[256];
	int cryptlen;
	int res;
	
	/* 1. GET ANSWERCODE;
	Receive 220 ANSWER CODE */
	res  = __readOneLine(task->shakeHandSocket, recvbuf, sizeof(recvbuf));
	if (atoi((char*)recvbuf) != DDNS_TCP_RESPONSE )
	{
		AS_MSG_LOG(AS_LOG_ERR,"Can not get ANSWERCODE:220 and reconnect. recved : %s\n", recvbuf);
		close(task->shakeHandSocket);
		sleep(10); //!!!200604 change 3 to 10
		return -2;
	}

	AS_MSG_LOG(AS_LOG_INFO, "accept recv: %i bytes ANSWERCODE: '%d', content : '%s'\n", res, atoi((char*)recvbuf), recvbuf);
//	ddnsDebugBuffer("After Connect, RX", recvbuf, res);
		

	/* 2. SEND AUTHTYPE ROUTER*/
	if (strlen(DDNS_SHAKE_REQUEST) != send(task->shakeHandSocket, DDNS_SHAKE_REQUEST, strlen(DDNS_SHAKE_REQUEST), 0))
	{
		AS_MSG_LOG(AS_LOG_ERR,"send() sent a different number of bytes than expected");
	}
	AS_MSG_LOG(AS_LOG_INFO, "TX 'AUTH ROUTER'\n");
	send(task->shakeHandSocket, "\r\n", 2, 0);


	/* 3. GET strChallenge64*/
	memset(recvbuf, 0, sizeof(recvbuf) );
	res = __readOneLine(task->shakeHandSocket,recvbuf, sizeof(recvbuf));
	AS_MSG_LOG(AS_LOG_INFO,"RX Content : '%s'", recvbuf );
	recvbuf[res-2] = 0;
	
	strChallenge64 = recvbuf+4;
	
	//CheckSuccess(atoi(buf) == 334);
	AS_MSG_LOG(AS_LOG_INFO,"accept recv: %i bytes AUTHCODE : '%d' and get AUTHKEY:%s\n", res, atoi((char*)recvbuf), strChallenge64);
//	ddnsDebugBuffer("RX response to 'AUTH ROUTER'", recvbuf, res);

	task->challengeLength =  lutil_b64_pton(strChallenge64, task->plainChallenge, DDNS_DEFAULT_BUFFER_LENGTH );
	AS_MSG_LOG(AS_LOG_DEBUG,"Decode Key : %i bytes Content : '%s' ", task->challengeLength, task->plainChallenge );

	/*/!!!20050218 check fault memory.*/
	/*modified by oray/skyvense, for ph35 embed
	example:id=1234 + version=5678 = 04d2+162E = 0x04d2162e
	example: key=556677889 = 0x212E3B01
	ID =9702 = 0x25E6
	VERSION = 38759 = 0x9767
	KEY = 215756579 = CDC2F23
	*/
	cryptlen = GenerateCrypt(task, DDNS_CLIENT_INFO, /*/id+version */
		DDNS_KEY_INFO, /*  Key */(char*)strCryptStr);
	AS_MSG_LOG(AS_LOG_INFO,"Generate CryptStr : '%s'\tlength :%d\n", strCryptStr, cryptlen);

	/* 4. SEND CryptStr. and get Domain name*/
	if (cryptlen != send(task->shakeHandSocket, strCryptStr, cryptlen, 0))
	{
		AS_MSG_LOG(AS_LOG_WARNING, "send() sent a different number of bytes than expected : %s", strerror(errno) );
	}
	else
	{
		AS_MSG_LOG(AS_LOG_INFO, "send CryptStr len:%d \n", cryptlen);
	}
	send(task->shakeHandSocket, "\r\n", 2, 0);

	memset(recvbuf, 0, sizeof(recvbuf) );
	res = __readOneLine(task->shakeHandSocket,recvbuf, 128);
	if (res <= 0)
	{
		AS_MSG_LOG(AS_LOG_NOTICE, "buffer:%s ret:%d\n", recvbuf, res);
		close(task->shakeHandSocket);
		sleep(10); //!!!200604 change 3 to 10

		return -3;
	}
	
	AS_MSG_LOG(AS_LOG_NOTICE, "Response of CryptStr : '%s', length : %d\n", recvbuf, res);
	recvbuf[3] = 0;
	if (strcmp((const char*)recvbuf, DDNS_AUTH_FAIL )!=0)
	{
		 //AUTH FAILED
		close(task->shakeHandSocket);
		sleep(10); //!!!200604 change 3 to 10
		return -4;
	}

	return 0;
}

int	__ddns_retrieve_domain(ddns_task_t *task)
{
	unsigned char domains[255][64];
	unsigned char regicommand[128];
	int i, totaldomains;
	char szQuit[] = DDNS_SHAKE_QUIT;
	unsigned char recvbuf[256];
	int res;
	int retries = 0;

	for (i=0,totaldomains=0;;i++)
	{
		memset(domains[i], 0, 256);
		__readOneLine(task->shakeHandSocket,domains[i],255);
		AS_MSG_LOG(AS_LOG_NOTICE, "%d domain of user %s : %s",i+1,task->userName, domains[i]);
		totaldomains++;
		if (domains[i][0] == '.')
			break;
	}

	for (i=0;;i++)
	{
		if (domains[i][0] == '.')
			break;
		if(strcasecmp(domains[i], task->info.fqdn ) ==0)
		{
			task->registerCount++;
			memset(regicommand, 0, sizeof(regicommand) );
			strcpy((char*)regicommand, COMMAND_REGI);
			strcat((char*)regicommand, " ");
			strcat((char*)regicommand, (const char*)domains[i]);
			strcat((char*)regicommand, "\r\n");
			AS_MSG_LOG(AS_LOG_NOTICE, "send out register command '%s'",regicommand);
			
			send(task->shakeHandSocket,regicommand,strlen((const char*)regicommand),0);
		}
	}

	AS_MSG_LOG(AS_LOG_NOTICE, "SEND CNFM DATA...");
	send(task->shakeHandSocket,COMMAND_CNFM,strlen(COMMAND_CNFM),0);
	AS_MSG_LOG(AS_LOG_NOTICE, "OK\n");

//	for (i=0;i<totaldomains-1;i++)
	for (i=0;i< task->registerCount; i++)
	{
		memset(recvbuf, 0, 256);
		__readOneLine(task->shakeHandSocket,recvbuf,sizeof(recvbuf));
		AS_MSG_LOG(AS_LOG_NOTICE, "%d response : %s\n",i+1, recvbuf);
	}

//oray modify end.
retry:
	memset(recvbuf, 0, sizeof(recvbuf) );
	res = recv(task->shakeHandSocket, recvbuf, sizeof(recvbuf), 0);
	if (atoi((char*)recvbuf) != 250)
	{
		AS_MSG_LOG(AS_LOG_WARNING, "Get REGICODE:%d unsuccessfully and reconnect.", atoi((char*)recvbuf));
		sleep(10); //!!!200604 change 3 to 10
		if(retries<5)
			goto retry;
		retries ++;
		
		return -4;
	}

	AS_MSG_LOG(AS_LOG_NOTICE, "recv: %i bytes, Content : %s\n", res, recvbuf );
	{
		char *pReq = strchr((char*)recvbuf, ' ');
		int nReq = atoi(pReq);
		char *pInit = strchr(pReq+1, ' ');
		int uInit = atoi(pInit);
#if 1		
		task->sessionCode = nReq;
		task->sequence = uInit;
#else
		/* -  */
		task->sessionCode = -1970149549;
		task->sequence = 1149455807;

#endif
		AS_MSG_LOG(AS_LOG_NOTICE,"accept recv: %i bytes SessionCode:%d(0x%x) Sequence:%d(0x%x)\n", res, nReq, nReq, uInit , uInit);
	}
		
	AS_MSG_LOG(AS_LOG_NOTICE, "Register: %s successfully!\n", task->info.fqdn );

	if (strlen(szQuit) != send(task->shakeHandSocket, szQuit, strlen(szQuit), 0))
	{
		AS_MSG_LOG(AS_LOG_NOTICE, "send() sent a different number of bytes than expected : %s", strerror(errno) );
	}

	AS_MSG_LOG(AS_LOG_NOTICE,"Send QUIT TCP Shakehand Session\n");
	send(task->shakeHandSocket, "\r\n", 2, 0);

	//iReceivedSize = recv(shakeSocket, recvbuf, 256, 0);
	res = __readOneLine(task->shakeHandSocket, recvbuf, sizeof(recvbuf) );
	if (atoi((char*)recvbuf) == DDNS_TCP_QUIT ) 
		AS_MSG_LOG(AS_LOG_INFO, "Get 221 TCP Quit.\n");

	return 0;
}

int	ddnsShakeHand(ddns_task_t *task)
{
	int res;
	
	if (__open_ddns_handshake( task) < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Internet is unreachable or the DDNS Server is down!");
		return -1;
	}


	if(__ddns_shakehand_authen(task) < 0)
		return -2;

	res = __ddns_retrieve_domain(task);
	
	close(task->shakeHandSocket);

	return res;
}

