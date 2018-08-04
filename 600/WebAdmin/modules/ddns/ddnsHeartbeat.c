/*
* $Id: ddnsHeartbeat.c,v 1.4 2007/05/31 20:38:05 lizhijie Exp $
*/

#include "_ddns.h"

static struct sockaddr_in addrsin;
static socklen_t sinLen;

void __ddnsUdpClose(ddns_task_t *task)
{
	if( task->udpSocket < 0 )
		return;
	
	if ( close(task->udpSocket ) == -1 )
	{
		AS_MSG_LOG(AS_LOG_WARNING, "UDP Disable Network close failed!" );
		return ;
	}
	task->udpSocket = -1;
}

int __ddnsUdpOpen(ddns_task_t *task)
{
	long save_file_flags;
#if 0	
	network_info	*netInfo;
	netInfo = cgi_get_network_info( 0);
#endif

	memset(&addrsin, 0, sizeof(addrsin));
	addrsin.sin_family = AF_INET;
//	if(netInfo== NULL)
	{
		addrsin.sin_addr.s_addr = htonl( INADDR_ANY );
	}
#if 0
	else
	{
		AS_MSG_LOG(AS_LOG_ERR, "UDP use address %s",netInfo->wanInfo.addr );
		if (inet_aton(netInfo->wanInfo.addr, &addrsin.sin_addr) == 0)
		{
			AS_MSG_LOG(AS_LOG_ERR, "IP Address is invalidate" );
			exit(1);
		}
		free(netInfo);
	}
#endif

	addrsin.sin_port = htons( task->udpMyPort);
	sinLen = sizeof( addrsin );
	
	task->udpSocket = socket( AF_INET, SOCK_DGRAM, 0 );
	if ( task->udpSocket  < 0 )
	{
		AS_MSG_LOG(AS_LOG_ERR, "UDP Enable Network Failed!" );
		return -1;
	}
	
	if ( bind( task->udpSocket, ( struct sockaddr * )&addrsin, sizeof( addrsin ) ) == -1 )
	{
		__ddnsUdpClose(task);
		AS_MSG_LOG(AS_LOG_ERR, "UDP bind failed!%s", strerror(errno) );
		return -2;
	}
	
	save_file_flags = fcntl( task->udpSocket, F_GETFL );
	save_file_flags = save_file_flags | O_NONBLOCK;
	if ( fcntl( task->udpSocket, F_SETFL, save_file_flags ) == -1 )
	{
		__ddnsUdpClose( task);
		AS_MSG_LOG(AS_LOG_ERR, "UDP set input socket to non-blocking fail!" );
		return -3;
	}

	return 0;
}

int __ddnsUdpSend(ddns_task_t *task, char *sendData, int length)
{
	struct sockaddr_in address;
	int rc;	
#if 0	
	struct hostent * hostbyname;
	hostbyname = gethostbyname( task->serverIp);
	if ( hostbyname == NULL )
	{
		printf( "SendPacket get \"%s\" hostbyname failed. \n", task->serverIp);
		return -1;
	}
#endif

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
#if 1	
	memcpy( &address.sin_addr.s_addr, task->serverHost->h_addr, sizeof( address.sin_addr.s_addr ) );
#else
	memcpy( &address.sin_addr.s_addr, hostbyname->h_addr, sizeof( address.sin_addr.s_addr ) );
#endif
	address.sin_port = htons( task->udpDestPort );
		
	if ( (rc = sendto( task->udpSocket, sendData, length, 0, ( struct sockaddr * ) &address, sizeof( address ))) == -1 )
	{
		AS_MSG_LOG(AS_LOG_ERR, "SendUDPPacket to:%s port:%d len:%d fail! %s", task->serverIp, task->udpDestPort, length, strerror(errno) );
		return -2;
	}
	
	AS_MSG_LOG(AS_LOG_NOTICE,"sendto UDP packet length of %d bytes to Server %s", rc , asInetNtoa(address.sin_addr.s_addr));
	return 0;
}

void __initPacket(ddns_task_t *task, UpdatePack *packet)
{
#if __ARM_IXP__
	packet->nReq = as_Big2Little(task->sessionCode);
	packet->nOperateCode = 10;
	packet->sessionCode = task->sequence;
//	packet->nCheckSum = -(packet->nOperateCode+packet->sessionCode);
	packet->nCheckSum = as_Big2Little(-(packet->nOperateCode+packet->sessionCode));
	packet->nOperateCode = as_Big2Little(packet->nOperateCode);
	packet->sessionCode = as_Big2Little(packet->sessionCode);

	packet->nFill = 0;
#else
	packet->nReq = task->sessionCode;
	packet->nOperateCode = 10;
	packet->sessionCode = task->sequence;
	packet->nCheckSum = -(packet->nOperateCode+packet->sessionCode);

	packet->nFill = 0;
#endif
}

#if __ARM_IXP__
void __getPlatformPacket(ddns_task_t *task, UpdatePack *packet)
{
	packet->nReq = as_Big2Little(packet->nReq);
	packet->nOperateCode = as_Big2Little(packet->nOperateCode);
	packet->sessionCode = as_Big2Little(packet->sessionCode);
	packet->nCheckSum = as_Big2Little(packet->nCheckSum);

	packet->nFill = as_Big2Little(packet->nFill);
}
#endif


int	ddnsHeartBeatLoop(ddns_task_t *task)
{
	unsigned char recvbuf[256];
	long lastseq = 0;
	int iSleepCount = 0;
	static int s_nMiss = 0;
	
	__ddnsUdpOpen( task);

	AS_MSG_LOG(AS_LOG_NOTICE, "Enter UDP Heartbeat Session.....");
	
	while(!task->quitFlags )
	{
		//Every 60 seconds timer;
		if (iSleepCount%60 == 0)
		{
			UpdatePack up;
			int nRecv;

			__initPacket( task, &up);
			lastseq = task->sequence;
			task->sequence++;
			AS_MSG_LOG(AS_LOG_NOTICE, "Send Heartbeat Packet, Session Code : %d(0x%x), last Sequence :%d(0x%x)\n", task->sessionCode, task->sessionCode, lastseq, lastseq);
#if 0//DDNS_DEBUG
			ddnsDebugBuffer("Before Blow", (char*)&up, 20);
#endif
			CryptBlow(task,  (char*)&up.nOperateCode);
			__ddnsUdpSend( task, (char*)&up, sizeof(UpdatePack));  
#if 0//DDNS_DEBUG
			ddnsDebugBuffer("After Blow", (char*)&up, 20);
#endif
			
			
			nRecv = recvfrom( task->udpSocket, recvbuf, sizeof(recvbuf), 0, ( struct sockaddr * )&addrsin, &sinLen);

			if(nRecv <= 0)	
			{
				s_nMiss++;
				if(s_nMiss == 3)
				{
					s_nMiss = 0;
					AS_MSG_LOG(AS_LOG_WARNING, "Do not Recv Udp Reponse Packet for 3 times\n");
					break;
					//ReConnect();
				}			
			}
			else if(nRecv == sizeof(UpdatePackEx) )
			{
				UpdatePackEx upex;
				UpdatePack up2;

//				ddnsDebugBuffer("UDP RX", recvbuf, nRecv);
				memcpy(&upex, recvbuf, sizeof(UpdatePackEx) );
				memcpy(&up2, &upex.base, sizeof(UpdatePack));
				
//				ddnsDebugBuffer("RX Before Blow", (char*)&up2, 20);
				DecryptBlow(task, (char*)&up2.nOperateCode);
//				AS_MSG_LOG(AS_LOG_NOTICE, "Op Code is %d", up2.nOperateCode);
//				ddnsDebugBuffer("RX After Blow", (char*)&up2, 20);
#if __ARM_IXP__
				__getPlatformPacket(task, &up2);
#endif
				AS_MSG_DEBUG(AS_LOG_NOTICE, "Op Code is %d", up2.nOperateCode);
//				ddnsDebugBuffer("RX After Blow 2", (char*)&up2, 20);
				if( (up2.nOperateCode != 50) || (up2.sessionCode < (lastseq-2)) || (up2.sessionCode > (lastseq+2)) )
				{  //up.sessionCode
					AS_MSG_LOG(AS_LOG_ERR, "Err operatecode : 0x%x or seq : 0x%x last seq : 0x%x s_nMiss:%d!\n", up2.nOperateCode, up2.sessionCode, lastseq, s_nMiss);
				
					if(s_nMiss == 3)
					{
						break;
					}
					s_nMiss++;
					//ReConnect();
				}
				else
				{
					s_nMiss = 0;
					AS_MSG_LOG(AS_LOG_WARNING, "Recv UDP Response Packet, OpCode:%d, seq:%d, IP:%s.\n", up2.nOperateCode, up2.sessionCode,asInetNtoa(upex.ip));
				}
			} 
			else
			{
				s_nMiss++;
				if(s_nMiss == 3)
				{
					AS_MSG_LOG(AS_LOG_WARNING, "Do not Recv UDP Answer(50) for 3 times.\n");
					break;
				}
			}
			//Can not recv 
		}  //end if every 60s
		
		sleep(1);
		iSleepCount++;
	}
	
	__ddnsUdpClose(task);	
	AS_MSG_LOG(AS_LOG_NOTICE, "DDNS Heartbeat Session exit and reconnect.");
	sleep(3);

	return 0;
}

