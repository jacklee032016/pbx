/*
* $Id: SimulateDB.cxx,v 1.2 2007/03/21 18:12:39 lizhijie Exp $
*/

#if	__CDR_OPEN__

#include "SimulateDB.hxx"
#include <unistd.h>
#include <fcntl.h>
#include "cpLog.h"
#include "UaConfiguration.hxx"
#include "DigitCollector.hxx"
#include "AssistPbxDevice.hxx"
#include "NetworkAddress.h"
#include "Data.hxx"
#include <sys/types.h>
#include <signal.h>
#include <error.h>
using namespace SDB;      

#include <iostream>
using namespace std;

int SimulateDB::obj_counts = 0;
SimulateDB* SimulateDB::me_ = NULL;

strFormat& strFormat::operator<< (char c)
{
	if (length+1 > maxsize)return *this;
	strbuf[length] = c;
	length = strlen(strbuf);
	return *this;
}
strFormat& strFormat::operator<< (int i)
{
	char buf[32] = {0};
	sprintf(buf,"%d",i);
	if (length+strlen(buf) > maxsize) return *this;
	strcpy(strbuf+length,buf);
	length = strlen(strbuf);
	return *this;
}
strFormat& strFormat::operator<< (unsigned int ui)
{
	if (length > maxsize)return *this;
	char buf[32] = {0};
	sprintf(buf,"%d",ui);
	if (length+strlen(buf) > maxsize) return *this;
	strcpy(strbuf+length,buf);
	length = strlen(strbuf);
	return *this;
}
strFormat& strFormat::operator<< (unsigned long int li)
{
	if (length > maxsize)return *this;
	char buf[32] = {0};
	sprintf(buf,"%d",li);
	if (length+strlen(buf) > maxsize) return *this;
	strcpy(strbuf+length,buf);
	length = strlen(strbuf);
	return *this;
}

strFormat& strFormat::operator<< (const char* pc)
{
	if (length+strlen(pc) > maxsize)return *this;
	strcpy(strbuf+length, pc);
	length = strlen(strbuf);
	return *this;
}

SimulateDB::SimulateDB() : writef(-1)
{
	obj_counts += 1;
	int sf = UaConfiguration::instance()->getCdrOn();
	if (sf == 0)return;

	string sfifo = UaConfiguration::instance()->getFifo();
	if (mkfifo(sfifo.c_str(), 0666) == -1)
	{
		cpLog(LOG_EMERG, "mkfifo failure.");
	}
	
	cpDebug(LOG_DEBUG, "get fifo is: %s, waiting for open to read...", sfifo.c_str());
	if (sf == 2)
	{
		pid_t pid = fork();
		assert(pid != -1);
		
		if (pid ==0)
		{
			//cpLog(LOG_EMERG, "CHILD PROCESS RUN, ACTUILL IS cdr.");
			string path = UaConfiguration::instance()->getCdr();
			string exe = "";
			getexe(path, exe);
				
			if (exe == "") 
			{
				cpLog(LOG_EMERG, "don't paser %s. child process exit. And left behind a defunct progress.",path.c_str());
				exit(0);
			}
			else
			{
				cpDebug(LOG_DEBUG, "execl(%s, %s, (char*), 0)", path.c_str(), exe.c_str());
				execl(path.c_str(), exe.c_str(), (char*)0);
				cpLog(LOG_EMERG,  "Some error occured when execl program %s, exit. And left behind a defunct progress.", path.c_str());
				exit(0);
			}
		}
	}

	writef = open(sfifo.c_str(), O_WRONLY);
	assert(writef > 0);
	
}

SimulateDB:: ~SimulateDB()
{
	close(writef);
}

void SimulateDB::create() //const char * filestate, const char * filerecords
{
	if (obj_counts > 0 )
	{
	      cpLog(LOG_EMERG, "Simulate existent.");
		return;
	}

	me_ = new SimulateDB();
	assert(me_);
}

SimulateDB* SimulateDB::instance()
{
	return me_;
}

void SimulateDB::destroy()
{
	me_->~SimulateDB();
}


void SimulateDB::addCdrClient(Sptr <SipMsg> msg, int deviceId, CdrCallEvent callevent)
{
	//Sptr <CdrSubset> subset;
	cpDebug(LOG_DEBUG, "CdrSubsetContainer Add Start!");
	string from = msg->getFrom().encode().logData();
   	string to = msg->getTo().encode().logData();
	SipCallId CallId = msg->getCallId();
	string callid = CallId.encode().logData();

	addCdrClient(callid,from,to, deviceId, callevent);

	//cpDebug(LOG_DEBUG, "CdrSubsetContainer Add Over!");
}


void SimulateDB::addCdrClient(string& callid, string& from, string&to, int deviceId, CdrCallEvent callevent)
{
      cpDebug(LOG_DEBUG, "addCdrClient -- 2 Add Start!");
	string::size_type pos = 0;
	string erase("\r\n");
	while ((pos = from.find_first_of(erase, pos)) != string::npos) from.erase(pos, 1);

	pos = 0;
	while ((pos = to.find_first_of(erase, pos)) != string::npos) to.erase(pos, 1);

	pos = 0;
	while ((pos = callid.find_first_of(erase, pos)) != string::npos) callid.erase(pos, 1);

	NetworkAddress local;
	Data dip = local.getIpName();
	string localIp = dip.logData();

	/*cpDebug(LOG_DEBUG, " --------- CDR INFOMORATION ---------");

	cpDebug(LOG_DEBUG, "CALL EVENT: %s", enumToString(callevent));
	cpDebug(LOG_DEBUG, "CALL ID: %s", callid.c_str());
	cpDebug(LOG_DEBUG, "CALL FROM: %s",from.c_str());
	cpDebug(LOG_DEBUG, "CALL TO: %s",to.c_str());
	cpDebug(LOG_DEBUG, "DEVICE ID: %d", deviceId);	
	cpDebug(LOG_DEBUG, "LOCAL IP DATA: %s", localIp.c_str());

	cpDebug(LOG_DEBUG, " ------------------------------------");*/

	
	Sptr <CDRSUBSET> cdrs = new CDRSUBSET ;
	
	strncpy(cdrs->m_callId, callid.c_str(), sizeof(cdrs->m_callId));
	
	strncpy(cdrs->m_userId, from.c_str(), sizeof(cdrs->m_userId));
	
	strncpy(cdrs->m_recvId, to.c_str(), sizeof(cdrs->m_recvId));
	
	cdrs->m_deviceId = static_cast<unsigned int> (deviceId);
	
	cdrs->m_callEvent = callevent;
	if (from.find(localIp) != string::npos) cdrs->m_callDirection = DIRECTION_OUT;
	else if (to.find(localIp) != string::npos) cdrs->m_callDirection = DIRECTION_IN;
	
	CdrSubsetContainer.add(cdrs);
	
}


int SimulateDB::writePipe()
{
	//cpLog(LOG_EMERG, "waiting for writePipe...");
	Sptr <CDRSUBSET> ns = CdrSubsetContainer.getNext();
	if (ns ==0 )return -1;

	//cpLog(LOG_EMERG, "writePipe ...");

	CdrClient nr;
		
	switch (ns->m_callEvent)
	{
       case CALL_RING:
	   	getTime(nr.m_gwStartRing,nr.m_gwStartRingMsec);
		break;
		
	case CALL_START:
		getTime(nr.m_gwStartTime, nr.m_gwStartTimeMsec);
		break;
		
	case CALL_END:
		getTime(nr.m_gwEndTime, nr.m_gwEndTimeMsec);
		break;

	default:
		break;
	}

	
	
	strFormat strm(1024);

	strm << ns->m_callId << ','
	<< enumToString(ns->m_callEvent) << ','
	<< enumToString(ns->m_callDirection) << ','
    	<< ns->m_userId << ','
    	<< ns->m_recvId << ','
	<< ns->m_deviceId << ','
    	<< nr.m_gwStartRing << ','
    	<< nr.m_gwStartRingMsec << ','
    	<< nr.m_gwStartTime << ','
    	<< nr.m_gwStartTimeMsec << ','
    	<< nr.m_gwEndTime << ','
    	<< nr.m_gwEndTimeMsec << ','
    	<< nr.m_originatorIp << ','
    	<< nr.m_Line << ','
    	<< nr.m_terminatorIp << ','
    	<< nr.m_terminatorLine << ','
    	<< nr.m_callType << ','
    	<< nr.m_callParties << ','
    	<< nr.m_protocolNum << ','   	
    	<< ns->m_callDisconnect<< '\n';

	//cpDebug(LOG_DEBUG, "format over!");

	if (strm.isempty()) 
	{
		cpDebug(LOG_DEBUG, "strm is empty.");
		return -1;
	}
	
      
      //char buffer[128] = {0};
	//strcpy(buffer, "this is a test.");
	//string sfifo = UaConfiguration::instance()->getFifo();
	//cpLog(LOG_EMERG, "get fifo is: %s", sfifo.c_str());
	//int wrf = open(sfifo.c_str(), O_WRONLY | O_NONBLOCK);
	/*if (writef== -1) 
	{
		cpLog(LOG_EMERG, "open fifo failure, return in writePipe");
		perror("open on fifo");
		return 0;
	}*/

	int wlen = write(writef, strm.c_str(), strm.s_size()); //  
	if (wlen <= 0)
	{
		cpLog(LOG_ERR, "Write failue with error no: %d",wlen);
	}

	cpDebug(LOG_DEBUG, "write one record: %s", strm.c_str());

	//close(wrf);
		
	return 1;
		
}


void SimulateDB::getTime( unsigned long &secs, unsigned int &millisecs )
{
    struct timeval tval;
	
    gettimeofday(&tval, 0);
    secs = (unsigned long) tval.tv_sec;
    millisecs = (unsigned int) tval.tv_usec / 1000;
}

void SimulateDB::getexe(const string& source, string& dest)
{
	int is = source.size();
	if (is < 2) return;
	for (; is > 0; --is)
	{
		if (source[is-1] == '/')
		{
			dest = source.substr(is, source.size()-is);
			cpDebug(LOG_DEBUG, "%s", dest.c_str());
			return;
		}
	}

	dest = "";
}

const char* SimulateDB::enumToString( const MindVsaStatus& e )
{
    switch (e)
    {
        case MIND_VSA_OK:
        return "MIND_VSA_OK";
        case MIND_VSA_FAIL:
        return "MIND_VSA_FAIL";
        case MIND_VSA_INVALID_ARG:
        return "MIND_VSA_INVALID_ARG";
        case MIND_VSA_UNKNOWN_USER:
        return "MIND_VSA_UNKNOWN_USER";
        case MIND_VSA_ACCOUNT_USE:
        return "MIND_VSA_ACCOUNT_USE";
        case MIND_VSA_CARD_EXPIRED:
        return "MIND_VSA_CARD_EXPIRED";
        case MIND_VSA_CREDIT_LIMIT:
        return "MIND_VSA_CREDIT_LIMIT";
        case MIND_VSA_USER_BLOCKED:
        return "MIND_VSA_USER_BLOCKED";
        case MIND_VSA_BAD_LINE_NUM:
        return "MIND_VSA_BAD_LINE_NUM";
        case MIND_VSA_INVALID_NUM:
        return "MIND_VSA_INVALID_NUM";
        case MIND_VSA_NO_RATE:
        return "MIND_VSA_NO_RATE";
        case MIND_VSA_NO_AUTH:
        return "MIND_VSA_NO_AUTH";
        case MIND_VSA_NO_MONEY:
        return "MIND_VSA_NO_MONEY";
        case MIND_VSA_ACCT_INACTIVE:
        return "MIND_VSA_ACCT_INACTIVE";
    }
    return "UNKNOWN";
}

  
const char* SimulateDB::enumToString( const CdrCallEvent& e )
{
    switch (e)
    {
        case CALL_START:
        return "CALL_START";
        case CALL_UPDATE:
        return "CALL_UPDATE";
        case CALL_RING:
        return "CALL_RING";
        case CALL_END:
        return "CALL_END";
        case CALL_COMPLETE:
        return "CALL_COMPLETE";
        case CALL_BILL:
        return "CALL_BILL";
        default:
        return "CALL_UNKNOWN";
    }
}

const char* SimulateDB::enumToString(const CdrCallDirection& e)
{
	switch (e)
	{
	case DIRECTION_IN:
		return "DIRECTION_IN";
	case DIRECTION_OUT:
		return "DIRECTION_OUT";
	default:
		return "DIRECTION_UNKNOWN";
	}
}


#endif

