#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <errno.h>
#include <time.h>

//!!!20050218 Add get passwd from nvram
#ifdef NO_MM
#include <asm/uCbootstrap.h>
_bsc1 (int, setbenv, char *, a)
#endif 


#include <signal.h>     /* for singal handle */
#include "mylinuxdef.h"
#include "generate.h"




#define SEND_BUFFER_SIZE 512
#define SEND_BUFFER_DATA_SIZE 504
#define SIZE_CMDPACKET 128 // Shake packet size
#define SESSION_FRAME_BUFFER_SIZE 65536

#define CMD_REQUEST		0x9101
#define CMD_AFFIRM		0x9102
#define CMD_EXIT		0x9103
#define CMD_ACCEPT		0x9104
#define CMD_CONNECTINFO		0x9105
#define CMD_STREAMHEAD		0x9106
#define COMMAND_REGI    "regi a"
#define COMMAND_CNFM    "cnfm\r\n"


typedef struct
{
    DWORD cmd_head;		//CMD_REQUEST        0x9101
    BYTE connecttype;	//连接的类型 0,TCP点播 ，1,UDP点播，2,广播 ERR_NV_NOCHANNEL ERR_NV_NOSOURCE
    BYTE Channel;		//通道号
    BYTE ClientID;		//客户端中的元素号码。由客户端的返回信息提供
} REQUEST;

typedef struct
{
	DWORD cmd_head;
	BYTE ClientID;
	BYTE ServerID;
	BYTE Channel;
	BYTE Type;
	char UDPRemoteAddress[16];
	UINT ClientPort;
} AFFIRM, EXIT;

#define MAX_STREAMHEADSIZE 80
typedef struct
{
	DWORD cmd_head;
	BYTE ClientID;
	int Headersize;
	char StreamHeadBuf[MAX_STREAMHEADSIZE];
} STREAMHEAD;

typedef struct
{
	DWORD cmd_head;
	char  MCAAddress[16];
	BYTE ReciveType;
	UINT SendPort;
	BYTE ServerID;
	BYTE ClientID;
} ACCEPT;

typedef struct
{
	DWORD cmd_head;
	BYTE Channel;
	BYTE Type;
	BYTE ServerID;
	BYTE ClientID;
} CONNECTINFO;

typedef struct
{
	BYTE packetType; // = SP_DATA 0x7f
	BYTE mediaType; // = MEDIA_VIDEO 0x01
	WORD contentSize;
	BYTE frameSeq;
	BYTE subpacketSeq;
	BYTE subpacketAmount;
	BYTE frameData; //^(nChannel<<4);ClientID
	char data[SEND_BUFFER_DATA_SIZE];
} SENDPACKET;


// quit flag when SIG_INT/SIGTERM
int process_quit = 0;

//!!!20050218 add for UDP
int udpSocket;
struct sockaddr_in addrsin;
socklen_t sinLen;
int g_sendPort = 6060;  //default
long g_nReq;
long g_intCount = 0;
long g_tm_lastInt = time(0);
ULONG g_uSeq;
unsigned char g_strChallenge64[256];;

struct UpdatePack
{
	long nReq;
	long nOperateCode;
	long nSeq;
	long nCheckSum;
	long nFill;
	UpdatePack()
	{
		nReq = g_nReq;
		nOperateCode = 10;
		nSeq = g_uSeq;
		nCheckSum = -(nOperateCode+nSeq);
		nFill = 0;
	}
};

struct UpdatePackEx
{
	UpdatePack base;
	long ip;
};

void CloseUDP()
{
	if ( udpSocket < 0 )
		return;
	if ( close( udpSocket ) == -1 )
	{
		printf( "UDP DisableNetwork close call failed.\n" );
		return ;
	}
	udpSocket = -1;
}

void CreateUDP( int recvPort , int sendPort )
{
	g_sendPort = sendPort;
	memset(&addrsin, 0, sizeof(addrsin));
	addrsin.sin_family = AF_INET;
	addrsin.sin_addr.s_addr = htonl( INADDR_ANY );
	addrsin.sin_port = htons( recvPort );
	sinLen = sizeof( addrsin );
	
	udpSocket = socket( AF_INET, SOCK_DGRAM, 0 );
	if ( udpSocket  == -1 )
	{
		printf( "UDP EnableNetwork socket call failed..\n" );
		return;
	}
	if ( bind( udpSocket, ( struct sockaddr * )&addrsin, sizeof( addrsin ) ) == -1 )
	{
		CloseUDP();
		printf( "UDP bind call failed..\n" );
		return;
	}
	
	long save_file_flags;
	save_file_flags = fcntl( udpSocket, F_GETFL );
	save_file_flags = save_file_flags | O_NONBLOCK;
	if ( fcntl( udpSocket, F_SETFL, save_file_flags ) == -1 )
	{
		CloseUDP();
		printf( "UDP set input socket to non-blocking fail..\n" );
		return;
	}
}

int SendUDPPacket( const char * addr, char *pSendData, int iSendLen)
{
	struct hostent * hostbyname;
	struct sockaddr_in address;
	hostbyname = gethostbyname( addr );
	if ( hostbyname == NULL )
	{
		printf( "SendPacket get \"%s\" hostbyname failed. \n", addr);
		return -1;
	}
	
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	memcpy( &address.sin_addr.s_addr , hostbyname->h_addr, 
			  sizeof( address.sin_addr.s_addr ) );
	address.sin_port = htons( g_sendPort );
		
	int rc;	
	if ( (rc = sendto( udpSocket, 
					pSendData, iSendLen,
					0, ( struct sockaddr * ) &address,
					sizeof( address ))) == -1 )
	{
		printf( "SendUDPPacket to:%s port:%d len:%d fail. \n", addr, g_sendPort, iSendLen);
		return -2;
	}
	else {
		printf("sendto len:%d packet.\n", rc);
	}
	return 0;
}


// process terminate & interrupt signal handler
static void my_handleSIG (int sig)
{
	if (sig == SIGINT)
	{
		if (time(0) - g_tm_lastInt >= 1)
		{
			g_intCount = 0;
		}
		g_tm_lastInt = time(0);
		printf ("signal = SIGINT %d\n",g_intCount);
		g_intCount++;
		if (g_intCount > 5)
		{
			exit(0);
		}
	}
	if (sig == SIGTERM)
		printf ("signal = SIGTERM\n");
	signal (sig, my_handleSIG);

	process_quit = 1;
}

/*
 * Create a file containing our process ID.
 */
static void
create_pidfile()
{
    FILE *pidfile;
    char pidfilename[128];
    sprintf(pidfilename, "%s", "/var/run/updatednsip.pid");

    if ((pidfile = fopen(pidfilename, "w")) != NULL) {
	fprintf(pidfile, "%d\n", getpid());
	(void) fclose(pidfile);
    } else {
	printf("Failed to create pid file %s: %m", pidfilename);
	pidfilename[0] = 0;
    }
  
}

int ReadOneLine(int s, unsigned char * lpszBuf, int nBufLen)
{
	int nReadCount = 0;
	int nRecv = 0;

	lpszBuf[0] = '\0';

	for (; ; )
	{
		nRecv = recv(s, lpszBuf + nReadCount, 1, 0);
		//printf("recvoneline:%s,count:%d\n",lpszBuf,nRecv);
		//if (nRecv == 0) continue;
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

const char *my_inet_ntoa(long ip)
{
	in_addr addr;
	addr.s_addr = ip;
	return inet_ntoa(addr);
}

int main(int argc, char *argv[])
{
	int shakeSocket = 0;		/* Shake socket descriptor */
	int dataSocket = 0;		/* Data socket descriptor */
	char * servIP = NULL;		/* Server IP address (dotted quad) */
	char DDNSip[] = "hph021.oray.net"; //"219.136.252.79";
	int servPort = 6060;		/* Server port of shake and data */ //Default5050;
					//modified by oray/skyvense
	struct sockaddr_in servAddr;	/* Server address struct for both shake and data */
	unsigned char szShakeBuffer[SIZE_CMDPACKET];
					/* Shake connection buffer */
	unsigned char szDataBuffer[SEND_BUFFER_SIZE];
					/* Data connection buffer */
	unsigned char szFrameBuffer[SESSION_FRAME_BUFFER_SIZE];
					/* Frame buffer */
	//!!!20050218 Add
	unsigned char recvbuf[256];
	unsigned char *strChallenge64;
	unsigned char szUser[32]; 
	unsigned char szPassword[32];
	//char szResult[256];
	char strCryptStr[256];

	unsigned char domains[255][64];
    unsigned char regicommand[128];
	int i,len, totaldomains,ret;


	unsigned char * pFrameBuffer = szFrameBuffer;
	unsigned int iFrameSize = 0;
	unsigned int iSentSize = 0;	/* Length of sent bytes */
	unsigned int iReceivedSize = 0;	/* Length of received bytes */

	REQUEST requestoutput;
	ACCEPT input;
	AFFIRM affirmoutput;
	EXIT exitoutput;
	CONNECTINFO condata;
	STREAMHEAD sysinput;

	void (*ohandler) (int);

	if (argc != 3 && argc != 1 && argc != 5)
	{
		fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>] <User Name> <Password>.\n", argv[0]);
		exit(1);
	}

	ohandler = signal (SIGINT, my_handleSIG);
	if (ohandler != SIG_DFL)
	{
		printf ("previous signal handler for SIGINT is not a default handler\n");
		signal (SIGINT, ohandler);
	}

	/*ohandler = signal (SIGTERM, my_handleSIG);
	if (ohandler != SIG_DFL) {
		printf ("previous signal handler for SIGTERM is not a default handler\n");
		signal (SIGTERM, ohandler);
	}*/
	
	if (argc == 3 || argc == 5)
	{
		servIP = argv[1];
		servPort = atoi(argv[2]);
	}
	else
	{
		servIP = DDNSip;  //default value;
	}

	create_pidfile();

RECONNECT:

	if ((shakeSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket() failed");

	{
	// Enlarge Socket Receiving Buffer
	int nMaxBuf = 64000;
	if(-1 == setsockopt(shakeSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nMaxBuf, sizeof(nMaxBuf)))
		return 0;

	// Enlarge Socket Sending Buffer
	nMaxBuf = 64000;
	if(-1 == setsockopt(shakeSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nMaxBuf, sizeof(nMaxBuf)))
		return 0;
	}

	{
	int bFlag = 1;
	if(-1 == setsockopt(shakeSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&bFlag, sizeof(bFlag)))
		return 0;
	}

	// connect to shake socket
	struct hostent * hostbyname;
	hostbyname = gethostbyname(servIP);
	if ( hostbyname == NULL )
	{
		printf( " Get \"%s\" hostbyname failed. \n", servIP);
		sleep(3);
	    	goto RECONNECT;
	}
	
	memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
	servAddr.sin_family      = AF_INET;             /* Internet address family */
	//servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	memcpy( &servAddr.sin_addr.s_addr , hostbyname->h_addr, sizeof( servAddr.sin_addr.s_addr ) );
	servAddr.sin_port        = htons(servPort); /* Server port */

	if (connect(shakeSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	{
		perror("connect() failed");
		printf("Internet is unreachable or the DDNS Server is down!.\n");
		close(shakeSocket);
		sleep(10);  //!!!200604 change 3 to 10
		goto RECONNECT;
	}
	else
	{
		printf("connect server:%s port #%i succeed.\n", servIP, servPort);
	}

	//1. GET ANSWERCODE;
	// Receive 220 ANSWER CODE
	//iReceivedSize = recv(shakeSocket, recvbuf, 256, 0);
	iReceivedSize = ReadOneLine(shakeSocket,recvbuf,256);
	if (atoi((char*)recvbuf) == 220)
	{
		printf("accept recv: %i bytes ANSWERCODE:%d.\n", iReceivedSize, atoi((char*)recvbuf));
	}
	else
	{
		printf("Can not get ANSWERCODE:220 and reconnect.\n");
		close(shakeSocket);
		sleep(10); //!!!200604 change 3 to 10
		goto RECONNECT;
	}
		

	//2. SEND AUTHTYPE ROUTER
	const char szAuth[] = "AUTH ROUTER";	//modified by oray/skyvense
	if (strlen(szAuth) != send(shakeSocket, szAuth, strlen(szAuth), 0))
	{
		perror("send() sent a different number of bytes than expected");
	}
	else
	{
		printf("send AUTH ROUTER.\n");
	}
	
	send(shakeSocket, "\r\n", 2, 0);

	/*//3. GET strChallenge64*/
	memset(recvbuf, 0, 256);
	iReceivedSize = ReadOneLine(shakeSocket,recvbuf,256);
	//iReceivedSize = recv(shakeSocket, recvbuf, 256, 0);
	recvbuf[iReceivedSize-2] = 0;
	strChallenge64 = recvbuf+4;
	//CheckSuccess(atoi(buf) == 334);
	printf("accept recv: %i bytes AUTHCODE:%d and get AUTHKEY:%s.\n", iReceivedSize, atoi((char*)recvbuf), strChallenge64);
	strcpy((char*)g_strChallenge64, (char*)strChallenge64);

	memset(szUser, 0, 32);
	memset(szPassword, 0, 32);
	if (argc == 5)
	{
		//start at ipcam.c
		strcpy((char *)szUser, argv[3]);
		strcpy((char *)szPassword, argv[4]);
	}
	else
	{//default value
		strcpy((char *)szUser, "test"); //登陆Oray护照名称(Oray.net account)
		strcpy((char *)szPassword, "test"); //密码(Oray.net account Password)
	}

#if 1//def MYDEBUG
	printf("User:%s len:%d; Password:%s len:%d; Challenge64str:%s len:%d.\n", szUser, strlen((char*)szUser), szPassword, \
		strlen((char*)szPassword), strChallenge64, strlen((char*)strChallenge64) );
#endif

	//!!!20050218 check fault memory.
	int cryptlen;
	cryptlen = GenerateCrypt((char*)szUser, (char*)szPassword, (char*)strChallenge64, 
	//modified by oray/skyvense, for ph35 embed
	//example:id=1234 + version=5678 = 04d2+162E = 0x04d2162e
	//example: key=556677889 = 0x212E3B01
								   0x25E69767, //id+version 十六进
								   0x0CDC2F23, //Key十六进
	//oray modify end.
								   (char*)strCryptStr);
#if  1//def MYDEBUG
	printf("GenerateCryptStr:%s len:%d.\n", strCryptStr, cryptlen);
#endif

	/*//4. SEND CryptStr. and get Domain name*/
	char strFirstDomain[64];
	//strlen((char*)strCryptStr)
	if (cryptlen != send(shakeSocket, strCryptStr, cryptlen, 0))
	{
		perror("send() sent a different number of bytes than expected");
	}
	else
	{
		printf("send CryptStr len:%d .\n", cryptlen);
	}
	send(shakeSocket, "\r\n", 2, 0);

	memset(recvbuf, 0, 256);

//modified by oray/skyvense	


	ret = ReadOneLine(shakeSocket,recvbuf,128);
	if (ret<=0)
	{
		printf("buffer:%s ret:%d\n", recvbuf,ret);
		close(shakeSocket);
		sleep(10); //!!!200604 change 3 to 10
		goto RECONNECT;
	}
	
	printf("%s %d\n", recvbuf,ret);
	recvbuf[3] = 0;
	if (strcmp((const char*)recvbuf,"250")!=0)
	{
		 //AUTH FAILED
		close(shakeSocket);
		sleep(10); //!!!200604 change 3 to 10
		goto RECONNECT;
	}

	for (i=0,totaldomains=0;;i++)
	{
		memset(domains[i], 0, 256);
		ReadOneLine(shakeSocket,domains[i],255);
//		printf("%s\n",domains[i]);
		totaldomains++;
		if (domains[i][0] == '.')
			break;
	}

	for (i=0;;i++)
	{
		if (domains[i][0] == '.')
			break;
		
		memset(regicommand, 0, 128);
		strcpy((char*)regicommand, COMMAND_REGI);
		strcat((char*)regicommand, " ");
		strcat((char*)regicommand, (const char*)domains[i]);
		strcat((char*)regicommand, "\r\n");
		printf("%s",regicommand);
		send(shakeSocket,regicommand,strlen((const char*)regicommand),0);
	}

	printf("SEND CNFM DATA...");
	send(shakeSocket,COMMAND_CNFM,strlen(COMMAND_CNFM),0);
	printf("OK\n");

	for (i=0;i<totaldomains-1;i++)
	{
		memset(recvbuf, 0, 256);
		ReadOneLine(shakeSocket,recvbuf,sizeof(recvbuf));
	}

//oray modify end.
	memset(recvbuf, 0, 256);
	iReceivedSize = recv(shakeSocket, recvbuf, 256, 0);
	printf("%s\n",(char *) recvbuf);
	if (atoi((char*)recvbuf) == 250)
	{
		char *pReq = strchr((char*)recvbuf, ' ');
		int nReq = atoi(pReq);
		char *pInit = strchr(pReq+1, ' ');
		int uInit = atoi(pInit);
		g_nReq = nReq;
		g_uSeq = uInit;
		printf("accept recv: %i bytes SessionCode:0x%x Sequence:0x%x\n", iReceivedSize, nReq, uInit);
	}
	else
	{
		printf("Get REGICODE:%d unsuccessfully and reconnect.\n", atoi((char*)recvbuf));
		close(shakeSocket);
		sleep(10); //!!!200604 change 3 to 10
		goto RECONNECT;
		//exit(0);
	}

#ifdef NO_MM
	char tmp[256]; // Alert: large nvram names will fail 
    	sprintf(tmp, "%s=%s", "nat_ipaddr", strFirstDomain);
    	setbenv(tmp);
    	printf("nvram_set: %s=%s\n", "nat_ipaddr", strFirstDomain);
#endif

	printf("Register: %s successfully!\n", strFirstDomain);

	const char szQuit[] = "QUIT";
	if (strlen(szQuit) != send(shakeSocket, szQuit, strlen(szQuit), 0))
		perror("send() sent a different number of bytes than expected");
	else
		printf("Send QUIT.\n");
	send(shakeSocket, "\r\n", 2, 0);

	//iReceivedSize = recv(shakeSocket, recvbuf, 256, 0);
	iReceivedSize = ReadOneLine(shakeSocket,recvbuf,256);
	if (atoi((char*)recvbuf) == 221) 
		printf("Get 221 TCP Quit.\n");
	close(shakeSocket);
	
	//Start UDP session
	CreateUDP( 6060, 6060);
	bool bQuit = true;
	int iSleepCount = 0;
	static int s_nMiss;/*连续错过包个数*/

	printf("Start UDP update timer.\n");
	s_nMiss = 0;
	long lastseq = 0;
	
	while (bQuit)
	{
		//Every 60 seconds timer;
		if (iSleepCount%60 == 0)
		{
			lastseq = g_uSeq;
			UpdatePack up;
			g_uSeq++;
			
			{
				char *p = (char*)&up;
				
				for(int  k = 0; k<sizeof(up);k++)
				{
					printf(" 0x%2x", p[k]);
				}
				printf("\n");
			}
			
			CryptBlow((char*)g_strChallenge64, (BYTE*)&up.nOperateCode);
			SendUDPPacket(servIP, (char*)&up, sizeof(up));  
			
			printf("Send seq:%d last seq:%d.\n", up.nReq, lastseq);

			{
				char *p = (char*)&up;
				
				for(int  k = 0; k<sizeof(up);k++)
				{
					printf(" 0x%2x", p[k]);
				}
				printf("\n");
			}
			
			int nRecv = recvfrom( udpSocket, recvbuf, 256, 0,
						  ( struct sockaddr * )&addrsin, &sinLen);

			if(nRecv <= 0)	
			{
				s_nMiss++;
				if(s_nMiss == 3)	// 连续错过三次
				{
					s_nMiss = 0;
					printf("Do not Recv Udp 3 times.\n");
					break;
					//ReConnect();
				}			
			}
			else if(nRecv == 24)
			{
				UpdatePackEx upex;
				UpdatePack up2;
				memcpy(&upex, recvbuf, 24);
				up2 = upex.base;
				DecryptBlow((char*)g_strChallenge64, (BYTE*)&up2.nOperateCode);
				if( (up2.nOperateCode != 50) || (up2.nSeq < (lastseq-2)) || (up2.nSeq > (lastseq+2)) ) {  //up.nSeq
					printf("Err operatecode:%d or seq:%d last seq:%d s_nMiss:%d!\n", up2.nOperateCode, up2.nSeq, lastseq, s_nMiss);
				
					if(s_nMiss == 3) {	// 连续错过三次
						break;
					}
					s_nMiss++;
					//ReConnect();
				}
				else {
					s_nMiss = 0;
					printf("Recv UDP operatecode:%d seq:%d ip:%s.\n", up2.nOperateCode, up2.nSeq,my_inet_ntoa(upex.ip));
				}
			} 
			else {
				s_nMiss++;
				if(s_nMiss == 3) {	// 连续错过三次
					printf("Do not Recv Answer:50 3 times.\n");
					break;
				}
			}
			//Can not recv 
		}  //end if every 60s
		sleep(1);
		iSleepCount++;
	}  //end while
	CloseUDP( );	
	printf("Updatednsip exit and reconnect.\n");
	sleep(3);
	goto RECONNECT;
	
	exit(0);
}
