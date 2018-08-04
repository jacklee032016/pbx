/*
	Function:			implement of module CdrDataServer
	Version:			1.0.0
	Bulid date:		11/05/2005
	Last Modify date:	16/05/2005
	Author:			wuaiwu
	
*/

#include "CdrDataServer.hxx"
#include "helper.hxx"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <sys/time.h>
//#include <dirent.h>
//#include <stddef.h>
//#include <stdarg.h>

//#include <string>
//#include <fstream>

using namespace std;

/*void cdr_debug_info(int pri, const char* file, int line, const char* frmt,...)
{
	va_list ap;
	va_start(ap, frmt);
	char buf[1024] = {0};
	vsncpDebug(LOG_DEBUG, buf, 1024, frmt, ap);
	va_end(ap);
	
	cpDebug(LOG_DEBUG, "%s: %d  %s\n",file, line, buf);
}


#define //filter-cpLog(pri,frmt, args...)\
if (pri <= 10 && pri <= CdrDataServer::get_loglevel())\
cdr_debug_info(pri, __FILE__, __LINE__,frmt,##args);\*/



const int FCFS = O_WRONLY | O_CREAT | O_APPEND;
const int FAFS = O_WRONLY | O_APPEND;
const mode_t FMODE = S_IRUSR | S_IWUSR;
const int LINE = 300;

static const char* CdrClientString[] = 
{
	"m_callId",

	"m_callEvent",

	"m_callDirection",

	"m_from",

	"m_to",

	"m_deviceId",

	"m_gwStartRing",

	"m_gwStartRingMsec",

	"m_gwStartTime",

	"m_gwStartTimeMsec",

	"m_gwEndTime",

	"m_gwEndTimeMsec",

	"m_originatorIp",

	"m_Line",

	"m_terminatorIp",

	"m_terminatorLine",

	"m_callType",

	"m_callParties",

	"m_protocolNum",

	"m_callDisconnect"
	
};

char* CdrDataServer::server_pipe = new char[64];
char* CdrDataServer::records_dir =  new char[128];
char* CdrDataServer::sfile = new char[256];
int CdrDataServer::file_size = 1024;
LOG_LEVEL CdrDataServer::cur_level = LOG_DEBUG;

CdrDataServer::CdrDataServer(const char * inifile)
{
	//cpDebug(LOG_DEBUG, "entry in CdrDataServer ctor");
	memset(server_pipe, 0, 64);
	memset(records_dir, 0, 128);
	memset(sfile, 0, 256);
	if (file_parse(inifile))
	{   
		strcpy(server_pipe, "/etc/sip/fifo.cdr");
		strcpy(records_dir, "/etc/sip/cdr/");
	}
}

CdrDataServer::~CdrDataServer()
{
	if (server_pipe == NULL) return;
	
	delete [] server_pipe;
	delete [] records_dir;
	delete [] sfile;
	
	server_pipe = NULL;
}

void CdrDataServer::startup()
{
	//cpDebug(LOG_DEBUG, "entry in startup \n");
	pthread_create(&th_c, NULL, &receiver, NULL);
	//pthread_create(&th_s, NULL, &dispatcher, NULL);

	pthread_join(th_c, NULL);

}

void CdrDataServer::jion()
{
	pthread_join(th_c, NULL);
}

bool CdrDataServer::file_parse(const char * pfn)
{
	//cpDebug(LOG_DEBUG, "entry in file_parse ");
	
	if (pfn == NULL)
	{
		//filter-cpLog(LOG_ERR,"pfn == NULL");
		return true;
	}

	//ifstream ifs(pfn, ios::binary);
	FILE *fp = fopen(pfn, "rb");

	if (fp == NULL)
	{
		return true;
	}

	int index1 = 0, index2 = 0;
	string cfg[10][3];
	while (!feof(fp))
	{
		char buffer[256] = {0};
		fgets(buffer, sizeof(buffer), fp);
		string linebuf(buffer);
		if (linebuf.size() < 10 || linebuf.find("#") != string::npos)continue;
		int i = 0;
		while ( linebuf[i] == ' ')

		{
			linebuf.erase(i, 1);
			i++;
		}

		string::size_type pos = 0;
		string mark("\r\n");

		while ((pos = linebuf.find_first_of(mark, pos)) != string::npos)linebuf.erase(pos,1);
      
		int l = linebuf.size();
		bool inc = true;
		for (i = 0; i < l; ++i)
		{
			if (linebuf[i] == ' ' && inc == true)
			{
				inc = false;
				index2++;
			}
			else if (linebuf[i] != ' ')
			{
				cfg[index1][index2] += linebuf[i];
				inc = true;
			}
		}


		index1++;
		index2 = 0;
		
	}

	strcpy(server_pipe, cfg[0][2].c_str());
	strcpy(records_dir, cfg[1][2].c_str());
	file_size = 1024 * atoi(cfg[2][2].c_str());
	cur_level = static_cast<LOG_LEVEL>(atoi(cfg[3][2].c_str()));
		
	return false;
	
}


void* CdrDataServer::receiver(void *p)
{

	int readf;
	readf = open(server_pipe, O_RDONLY);
	if (readf == -1)
	{
		perror("open on fifo");
		return p;
	}

	
      char cdrfile[128] = {0};
	get_file_name(cdrfile);
	int fs = open(cdrfile, FCFS, FMODE);
	if (fs == -1)
	{
		perror("open on cdrfile");
		return p;
	}
      
	fd_set readset;
	FD_ZERO(&readset);
	FD_SET(readf, &readset);
	FD_SET(1, &readset);

	while (1)
	{
		//sleep(3); // here will replace with select()
		int whatselt = select(FD_SETSIZE, &readset, NULL, NULL, NULL);
		if (whatselt < 0)continue;
		
		if (FD_ISSET(readf, &readset))
		{			
			char buff[LINE] = {0};
			size_t rdlen = read(readf, buff, sizeof(buff));
			if (rdlen <= 0) continue;
			
			string_parse(buff);
			
			struct stat stbuf;
			if (fstat(fs, &stbuf) == -1)
			{
				//cpDebug(LOG_DEBUG, "fstat error");
				perror("fstat file");
			}
			else
			{
			      //filter-cpDebug(LOG_DEBUG, "current file size: %d\n",(int)stbuf.st_size);
				if (stbuf.st_size > file_size)
				{
					close(fs);
					strcpy(sfile, cdrfile);
					memset(cdrfile, 0, sizeof(cdrfile));				
					get_file_name(cdrfile);
					
					//cpDebug(LOG_DEBUG,"Create a new file: %s", cdrfile);
					if ((fs = open(cdrfile, FCFS, FMODE)) < 0)
					{
						perror("open cdrile in screate");
						return p;
					}
						
				}
			}

			write(fs, buff, rdlen);
			memset(buff, 0 , LINE);
		
		}
		

	}
	
	return p;
	
}


#if 0
void* CdrDataServer::dispatcher(void *p)
{
	while (1)
	{
		//filter-cpDebug(LOG_DEBUG,"dispatcher waiting... ");
		
		//sem_wait(sem);

		//filter-cpDebug(LOG_DEBUG,"dispatcher start working... ");
		//char filepath[256] = {0};
		//strcpy(filepath, sfile);
		//ifstream ifs(filepath);
		
		//if (ifs == NULL)
		//{
			//filter-cpLog(LOG_ERR, "dispatcher don't open file: %s", filepath);
		//	continue;
		//}

	}

	return p;
}
#endif

void CdrDataServer::string_parse(const string& source)
{
	
	string cdrdetial[20] = {""};
	int prev = 0, cn = 0;
	int i = 0;
	int size = source.size();
	
	while (i < size)
	{
		if (source[i] == ',')
		{
			cdrdetial[cn++] = source.substr(prev, i-prev);
			prev = i+1;	
			
			if (cn > 19)
			{
				//filter-cpLog(LOG_ERR,"Record maybe wrong. cn = %d", cn);
				break;
			}			
			
		}

		i++;
		
	}

	cdrdetial[19] = "END_NORMAL";
	cdrdetial[18] = "MIND_VSA_DTMF";
	cdrdetial[17] = "PHONE_TO_PHONE";
	cdrdetial[16] = "VOICE";

	string mark("\t ");
	string::size_type fpos = 0;
	while ((fpos = cdrdetial[0].find_first_of(mark, fpos)) != string::npos)cdrdetial[0].erase(fpos,1);
	//cdrdetial[0] = cdrdetial[0].substr(8, cdrdetial[0].size()-8);
			
	fpos = 0;
	while ((fpos = cdrdetial[3].find_first_of(mark, fpos)) != string::npos)cdrdetial[3].erase(fpos,1);
	//cdrdetial[3] = cdrdetial[3].substr(5, cdrdetial[3].size()-5);

	fpos = 0;
	while ((fpos = cdrdetial[4].find_first_of(mark, fpos)) != string::npos)cdrdetial[4].erase(fpos,1);
	//cdrdetial[4] = cdrdetial[4].substr(3, cdrdetial[4].size() - 3);

	string sip;
	cdrdetial[12] = ip_parse(cdrdetial[3]);
	fpos = 0;
	fpos = cdrdetial[0].find("@");
	if (fpos != string::npos)
	{
		cdrdetial[12] = cdrdetial[0].substr(fpos+1, cdrdetial[0].size()-fpos-1);
	}
	else cdrdetial[0] = "0.0.0.0";
	
	cdrdetial[14] = ip_parse(cdrdetial[4]);	/**/

	
	for (int i = 6; i <= 10; i+=2)
	{
		if (cdrdetial[i] != "0")
		{
			long  t = atoi(cdrdetial[i].c_str());
			struct tm *ptm;
			ptm = localtime(&t);
			char time_string[32] = {0};
			strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", ptm);

			cdrdetial[i] = time_string;
		}
	}
	cpLog(LOG_INFO, "nIn cdrServer: Received one record == >>\n");
  
	for (int i = 0; i < 20; ++i)
	{
		string cnbf = "%s:%";
		char tbf[16] = {0};
		int ti = strlen(CdrClientString[i]);
		ti = 30 - ti;
		sprintf(tbf, "%dc", ti);
		cnbf += tbf;
		cnbf += "%s";
		if (i == 19)cnbf += "\n";
			
		cpLog(LOG_INFO, cnbf.c_str(),CdrClientString[i], ' ', cdrdetial[i].c_str());
		//cpDebug(LOG_DEBUG, "%s   :%s\n", CdrClientString[i], cdrdetial[i].c_str());
	}
	cpLog(LOG_INFO, "In cdrServer: one record over << == \n");

	// end of for test

	/* there are for current and later would be replaced by other 
	  * other maybe a interface refer from VOCAL CdrInterface or 
	  * a interface to Radius that necessary for build a Radius strcut
	  */
	
	
}

void CdrDataServer::get_file_name(char *pfn)
{
	
	if (pfn == NULL) 
	{
		//filter-cpLog(LOG_ERR,"pfn == NULL, in get_file_name");
		return;
	}
	
	struct timeval tv;
	struct tm *ptm;

	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);

	char time_string[32] = {0};
	strftime(time_string, sizeof(time_string), "%Y%m%d%H%M%S", ptm);

	strcat(time_string, ".cdr");

	strcpy(pfn,records_dir);
	strcat(pfn, time_string);
	
}

#if 0
bool CdrDataServer::get_first_filen(char * pfn)
{
      if (pfn == NULL) 
	{
		//filter-cpLog(LOG_ERR,"pfn == NULL, in get_first_filen");
		return false;
	}
	  
	DIR *dp;
  	struct dirent *ep;

	dp = opendir (records_dir);

	if (dp == NULL)
	{
	     //filter-cpLog (LOG_ERR, "Couldn't open the directory %s, will create it.",records_dir);
	     string command("mkdir ");
	     command += records_dir;

	     //filter-cpDebug(LOG_DEBUG, "system start");
	     system(command.c_str());
	     //filter-cpLog (LOG_DEBUG, "system command over.",command.c_str());
	     get_file_name(pfn);
		 
	     //filter-cpLog (LOG_DEBUG, "create %s over.",pfn);
		 
	     return false;
	}

      while ((ep = readdir (dp)))
	{ 
	      char fn[128] = {0};
		int len = strlen(ep->d_name);
	      strcpy(fn, ep->d_name+len-4);
		  
		if (strcmp(fn, ".cdr") == 0)
		{
			strcpy(fn, records_dir);
			strcat(fn, ep->d_name);
			struct stat stbuf;
			stat(fn, &stbuf);

			if (stbuf.st_size < file_size)
			{
				strcpy(pfn, fn);
				return true;
			}
		}	
	      
      	}

	get_file_name(pfn);
  
	(void) closedir (dp); 

	return false;
    	     
}

#endif 

string CdrDataServer::ip_parse(const string& ss)
{
	string ip;
	string::size_type ippos = 0;
	unsigned i = 0;
	ippos = ss.find("@");
	if (ippos == string::npos)
		ippos = ss.find(":");
	
	if (ippos != string::npos)
	{
		////filter-cpDebug(LOG_DEBUG, "find ippos at %d", ippos);
		ippos++;
		while (i <= 3)
		{
						
			if (i == 3 && (ss[ippos] < '0' || ss[ippos] > '9'))break;
			ip += ss[ippos];
			if (ss[ippos] == '.')i++;
			ippos++;

		}
	}
	else 
	{
		ip = "0.0.0.0";
	}

	return ip;
}

int CdrDataServer::get_loglevel()
{
	return static_cast<int>(cur_level);
}



/*void //filter-cpLog(int pri, const char *frmt,...)
{
	    va_list ap;
		
	    if (pri <= 10 && pri == CdrDataServer::get_loglevel())
	    {
	        va_start(ap, frmt);
	        cdr_debug_info(pri, frmt, ap);
	        va_end(ap);
	    }
}*/

/*#define //filter-cpLog(int pri, const char *frmt,...)\
{va_list ap;\
if (pri <= 10 && pri == CdrDataServer::get_loglevel()){\
va_start(ap, frmt);\
cdr_debug_info(pri, __FILE__, __LINE__,frmt, ap);\
va_end(ap);}}*/

