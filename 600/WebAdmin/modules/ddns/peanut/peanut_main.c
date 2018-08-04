/*
* $Id: peanut_main.c,v 1.10 2007/09/06 09:19:58 lizhijie Exp $
*/

#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include "peanut.h"

#define SIZE 					1024
#define SLEEPTIME 			10//60
#define LOGFILE 				1

static log_stru_t myLogObj = 
{
	lstyle		:	USE_SYSLOG,
	llevel		: 	LOG_DEBUG,
	lfacility		:	AS_LOG_4,
	isDaemonized	:	1
};

PEANUT_INFO		peanutInfo;

//char *testKey = {"3tJHcgxmJL9zRRlzNksRCQ=="};

int peanut_init(PEANUT_INFO *info)
{
	list_create(&info->configs);
	if(readconfig(PEANUT_CONFIG_FILE, ITEM_EQU,NULL, &info->configs))
	{
		printf("Without PEANUT-HULL configuration file : %s\n", PEANUT_CONFIG_FILE);
		return -1;
	}

	info->username = GET_VALUE(&info->configs, PEANUT_KEYWORD_USERNAME );
	info->password = GET_VALUE(&info->configs, PEANUT_KEYWORD_PASSWD);
	info->fqdn = GET_VALUE(&info->configs, PEANUT_KEYWORD_FQDN );
	info->port = atoi(GET_VALUE( &info->configs, PEANUT_KEYWORD_PORT)  );

	if(!strlen(info->username) || !strlen(info->password) || !strlen(info->fqdn) )
	{
		printf("Error in PEANUT-HULL Configuration File(Field of UserName, Password and FQDN)\n");
		return -2;
	}

	if(!strcasecmp(GET_VALUE(&info->configs, PEANUT_KEYWORD_LOG_TYPE), PEANUT_LOG_TYPE_FILE ) )
	{
		myLogObj.lstyle = USE_FILE;
	}
	else if(!strcasecmp(GET_VALUE(&info->configs, PEANUT_KEYWORD_LOG_TYPE), PEANUT_LOG_TYPE_CONSOLE ) )
	{
		myLogObj.lstyle = USE_CONSOLE;
	}
		
	myLogObj.logFileName = GET_VALUE(&info->configs, PEANUT_KEYWORD_LOG_FILE );
	myLogObj.llevel = atoi(GET_VALUE( &info->configs, PEANUT_KEYWORD_LOG_LEVEL)  );
	as_log_init( &myLogObj);

	as_debug(LOG_DEBUG, "User Name : %s\tPassword : %s\tFQDN : %s", info->username, info->password, info->fqdn );
	as_debug(LOG_DEBUG, "Port : %d\tPassword : %s\tFQDN : %s", info->port, info->password, info->fqdn );
	as_debug(LOG_DEBUG, "Log Type : %s\tLog File : %s", (myLogObj.lstyle==USE_SYSLOG)?"syslog":GET_VALUE(&info->configs, PEANUT_KEYWORD_LOG_TYPE), myLogObj.logFileName);

	return 0;
}

int main(int argc, char **argv)
{
	char des[16]={0};
	char keys[16]={0};
	char *cram_md5_in;
	char *cram_md5_out;
	char *cram_md5;
	char key[25]={0};
	char *decodekey;
	PEANUT_INFO *info = &peanutInfo;

	char *ip="61.152.96.116";
	struct hostent *hp;
	struct sockaddr_in addr;
	char buf[SIZE];
	int isocket,recvsize;
	char code[4]={0};
	int count250=0;
	int is_fisrt=1;
	
	if(peanut_init(info) )
		exit(1);
	
	while(1)
	{
		if(!is_fisrt)
			sleep(SLEEPTIME);
		
		is_fisrt=0;
		
		hp=gethostbyname(ip);
		if (hp==NULL)
		{
			as_cplog(LOG_ERR, "ERROR:hostent error for IP Address '%s'\n", ip);
			sleep(SLEEPTIME);
			continue;
		}

		if((isocket=socket(AF_INET,SOCK_STREAM,0))<0)
		{
			as_cplog(LOG_ERR, "create socket error\n");
			sleep(SLEEPTIME);
			continue;
		}
		
		memset(&addr,0,sizeof(struct sockaddr_in));
		addr.sin_family=AF_INET;
		addr.sin_port=htons( info->port);
		memcpy(&addr.sin_addr,hp->h_addr_list[0],hp->h_length);
		
		printf("Connecting to %s:%d...\n",ip, info->port );
		as_cplog(LOG_EMERG,"Connecting to %s:%d...\n",ip, info->port );
		if(connect(isocket,( struct sockaddr * )&addr,sizeof(addr))<0)
		{
			as_cplog(LOG_ERR,"ERROR:socket connect error : %s\n" , strerror(errno) );
			sleep(SLEEPTIME);
			continue;
		}

		memset(buf, 0 , sizeof(buf) );
		while ( (recvsize=read(isocket,buf,sizeof(buf) ) ) >0 )
		{
			as_debug(LOG_DEBUG, "[SERVER] : %s\n", buf );
			
			if(recvsize>3)
			{
				memcpy(code,buf,3);
				if(strcmp(code,"220")==0)
				{
#if 0				
					if( info->port== 6060)
					{
						write(isocket,"auth phsrv3\r\n",15);
						
						as_debug(LOG_DEBUG, "[CLIENT] : auth phsrv3\n");
					}
					else
#endif						
					{
						write(isocket,"auth cram-md5\r\n",17);
						as_debug(LOG_DEBUG, "[CLIENT]auth cram-md5\n" );
					}
				}
				else if(strcmp(code,"334")==0)
				{

					//复制服务器送过来的Key
					memcpy(key,buf+4,24);
					as_debug(LOG_DEBUG,"[DEBUG] :  getkey={%s}\n",key );
//					memcpy(key, testKey,24);
//					as_debug(LOG_DEBUG,"[DEBUG] :  test getkey={%s(%s)}\n",key, testKey);
				
					//对Key进行DecodeBase64解码，解码后的key是16字节的
					decodekey=DecodeBase64(key);
//					as_debug(LOG_DEBUG,"[DEBUG] :  decodekey={%s}\n",decodekey );
					memcpy(keys,decodekey,16);
					//使用key对密码进行MD5加密，加密后的报文存储在des中
					hmac_md5(info->password, strlen(info->password),keys, 16, des);
#if 0					
					//发送给服务器的内容是username+空格+Des(MD5)
					cram_md5_in=(char *) malloc(strlen(info->username)+18);
					memset(cram_md5_in, 0, strlen(info->username)+18);
					//复制username
					memcpy(cram_md5_in,info->username,strlen(info->username));
					//复制一个空格
					memcpy(cram_md5_in+strlen(info->username)," ",1);
					//复制16字节MD5码
					memcpy(cram_md5_in+strlen(info->username)+1,des,16);
#else
					//发送给服务器的内容是username+空格+Des(MD5)
					cram_md5_in=(char *) malloc(strlen(info->username)+18);
					memset(cram_md5_in, 0, strlen(info->username)+18);
					//复制username
					memcpy(cram_md5_in,info->username,strlen(info->username));
					//复制一个空格
					memcpy(cram_md5_in+strlen(info->username)," ",1);
					//复制16字节MD5码
					memcpy(cram_md5_in+strlen(info->username)+1,des,16);

#endif
					//进行EncodeBase64加密
					cram_md5_out=EncodeBase64(cram_md5_in);
					//最后复制一个回车
					cram_md5=(char *) malloc(strlen(cram_md5_out)+2);
					memset(cram_md5, 0, strlen(cram_md5_out)+2);
					memcpy(cram_md5,cram_md5_out,strlen(cram_md5_out));
					memcpy(cram_md5+strlen(cram_md5_out),"\r\n",2);
					//发送给服务器
					write(isocket,cram_md5,strlen(cram_md5_out)+2);
					
					as_debug(LOG_DEBUG,"[CLIENT] : %s\n",cram_md5_out );
					
					memset(cram_md5, 0, strlen(cram_md5_out)+2);
					memset(cram_md5_in, 0, strlen(info->username)+18);
					memset(decodekey,0,512);
					memset(cram_md5_out,0,512);
					free(decodekey);
					free(cram_md5_out);
					free(cram_md5_in);
					free(cram_md5);

				}
				
				else if(strcmp(code,"250")==0)
				{
					count250++;
					if(count250==1)
					{
						sprintf(buf, "regi a %s\r\n", info->fqdn );
						write(isocket, buf, strlen(buf) );//你自己的网址

						as_debug(LOG_DEBUG,"[CLIENT]%s\n", buf);
					}
					
					if(count250==2)
					{
						write(isocket,"cnfm\r\n",6);

						as_debug(LOG_DEBUG,"[CLIENT]cnfm\n" );
					}
					
					if(count250==3)
					{
						write(isocket,"quit\r\n",6);
						
						as_debug(LOG_DEBUG,"[CLIENT]quit\n" );
					}
				}
				else if(strcmp(code,"221")==0)
				{
					count250=0;
					close(isocket);

					as_debug(LOG_DEBUG,"Disconnected!\n" );
				}
				
				memset(buf, 0, SIZE);

			}
		}

	}

	return 0;
}

