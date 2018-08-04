/* 
* Module Name:		SimulateDB 	
* Function:			An interface  for Cdr data.                          
* $Id: SimulateDB.hxx,v 1.2 2007/03/21 18:12:39 lizhijie Exp $
*/

#ifndef		_SIMULATEDB_HXX
#define		_SIMULATEDB_HXX

//#include "Cdr.hxx"
#include "Fifo.h"
#include "Sptr.hxx"
#include "SipMsg.hxx"
#include "SipCallId.hxx"
#include <string>
using namespace std;
using namespace Vocal;

namespace SDB
{

///
enum CdrCallDirection
{ ///
	DIRECTION_IN = 'I',  ///
	DIRECTION_OUT = 'O'  ///
};

///
enum CdrCallType
{ ///
	TYPE_VOICE = 'V',  ///
	TYPE_FAX = 'F',  ///
	TYPE_DATA = 'D' ///
};

///
enum CdrCallParties
{ ///
	PHONE_TO_PHONE = 1,   ///
	DESK_TO_PHONE = 2,   ///
	PHONE_TO_DESK = 3,   ///
	PHONE_TO_PBX = 10,  ///
	DESK_TO_PBX = 11,  ///
	WEB_TO_PBX = 20  ///
};

///
enum CdrCallEnd
{ ///
	END_NORMAL = 'N',  ///
	END_DISCONNECTED = 'D',  ///
	END_ABANDONED = 'A',  ///
	END_BUSY = 'B',  ///
	END_ERROR = 'E'  ///
};

///
enum CdrErrorType
{ ///
	ERR_NORMAL = 0,   ///
	ERR_BUSY = 1,   ///
	ERR_ABANDONED = 2,   ///
	ERR_INVALID_PHONE = 3,   ///
	ERR_INVALID_USER = 4,   ///
	ERR_CARD_EXPIRED = 5,   ///
	ERR_CREDIT_LIMIT = 6,   ///
	ERR_BLOCKED_USER = 7,   ///
	ERR_RESTRICTED_PHONE = 8,   ///
	ERR_NO_FAX_PAPER = 9,   ///
	ERR_NO_ANSWER = 10,  ///
	ERR_NO_DIAL_TONE = 11,  ///
	ERR_NETWORK_ERROR = 12,  ///
	ERR_NO_ROUTE = 13,  ///
	ERR_NO_PORTS = 14,  ///
	ERR_GENERAL = 15,  ///
	ERR_USER_DEFINED = 1000 ///
};

/** Enums for MIND VSA Attributes **/

///
enum MindVsaCallerIdType
{ ///
	MIND_VSA_USER_ID = 0,  ///
	MIND_VSA_PIN_CODE = 1,  ///
	MIND_VSA_ANI_CODE = 2,  ///
	MIND_VSA_DOMAIN = 4  ///
};

///
enum MindVsaProtocol
{ ///
	MIND_VSA_DTMF = 0,  ///
	MIND_VSA_E164 = 1  ///
};

///
enum MindVsaStatus
{ ///
    MIND_VSA_OK = 0,   ///
    MIND_VSA_FAIL = 1,   ///
    MIND_VSA_INVALID_ARG = 2,   ///
    MIND_VSA_UNKNOWN_USER = 3,   ///
    MIND_VSA_ACCOUNT_USE = 4,   ///
    MIND_VSA_CARD_EXPIRED = 5,   ///
    MIND_VSA_CREDIT_LIMIT = 6,   ///
    MIND_VSA_USER_BLOCKED = 7,   ///
    MIND_VSA_BAD_LINE_NUM = 8,   ///
    MIND_VSA_INVALID_NUM = 11,  ///
    MIND_VSA_NO_RATE = 12,  ///
    MIND_VSA_NO_AUTH = 13,  ///
    MIND_VSA_NO_MONEY = 14,  ///
    MIND_VSA_ACCT_INACTIVE = 15  ///
};

/** Client specific enum **/

///
enum CdrCallEvent
{ ///
    CALL_UNKNOWN = 0,  ///   Message received out of order
    CALL_START = 1,    ///   Received an Invite SIP message
    CALL_UPDATE = 2,   ///   Received Re-invite to refresh session timer
    CALL_RING = 3,     ///   Received 18x status message
    CALL_END = 7,      ///   Received Bye SIP message
    CALL_COMPLETE = 8, ///   Obsolete
    CALL_BILL = 9      ///   Reconciled record for entire call
};


/**
   CdrClient is the minimum data required from the client
 **/

struct CdrSubset
{
	char				m_callId[128];          /// CDR Server unique id
	char				m_userId[128];          /// caller
	char				m_recvId[128];          /// callee NOTE:Set m_protocolNum field
	unsigned int		m_deviceId;   ///  
	CdrCallEnd		m_callDisconnect;       ///
	CdrCallEvent 		m_callEvent;            ///
	CdrCallDirection	m_callDirection;      //

	CdrSubset()
	{
		memset(this, 0, sizeof(CdrSubset));
	}

};

typedef struct CdrSubset CDRSUBSET;

struct CdrClient
{ ///

	CDRSUBSET			ss;
	unsigned long			m_gwStartRing;          /// start time for ringing
	unsigned int			m_gwStartRingMsec;      ///
	unsigned long			m_gwStartTime;          /// seconds since Jan 1,1970
	unsigned int			m_gwStartTimeMsec;      ///
	unsigned long			m_gwEndTime;            /// seconds since Jan 1,1970
	unsigned int			m_gwEndTimeMsec;        ///
	char					m_originatorIp[24];     /// IP of the originator gw
	unsigned int			m_Line;       /// for IN calls only
	char					m_terminatorIp[24];     /// IP of the terminating gw
	unsigned int			m_terminatorLine;       /// for OUT calls only
	CdrCallType			m_callType;             ///
	CdrCallParties			m_callParties;          ///
	MindVsaProtocol		m_protocolNum;        /// Applies to m_recvId    

	///
	CdrClient()
	{
		memset(this, 0, sizeof(CdrClient));
	}

};

class strFormat
{
	public:
		explicit strFormat(unsigned int size = 1024) : 
			maxsize(size),
			length(0)
		{
			strbuf = new char[size];
			memset(strbuf, 0, size);
		}

		strFormat(char* buf, unsigned int size=1024) :
			strbuf(buf),
			maxsize(size),
			length(0)
		{
			assert(strbuf);
		}

		~strFormat()
		{
			delete [] strbuf;
		}

	/*operator char*() const
	{
	return strbuf;
	}*/

		char *c_str()
		{
			return strbuf;
		}

		unsigned int s_size()
		{
			return length;
		}

		strFormat& operator<< (char c);
		strFormat& operator<< (int i);
		strFormat& operator<< (unsigned int ui);
		strFormat& operator<< (unsigned long int li);
		strFormat& operator<< (const char* pc);

		bool isempty() const
		{
			return (length == 0)? true : false;
		}

	private:

		char				*strbuf;
		unsigned int		maxsize;
		unsigned int		length;

};


class SimulateDB
{
public:
	static void create(); //const char* filestate, const char* filerecords);
	static void destroy();
	static SimulateDB* instance();

	void addCdrClient(Sptr <SipMsg> msg, int deviceId, CdrCallEvent callevent );
	void addCdrClient(string& callid, string& from, string&to, int deviceId, CdrCallEvent callevent);

	int writePipe();


private:
	
	SimulateDB();
	~SimulateDB();

	SimulateDB(const SimulateDB&);
	const SimulateDB& operator= (const SimulateDB&);

	void SimulateDB::getTime( unsigned long &secs, unsigned int &millisecs );
	void getexe(const string&, string&);
	const char* enumToString( const MindVsaStatus& e );
	const char* enumToString( const CdrCallEvent& e );
	const char* enumToString( const CdrCallDirection& e);


private:

	static SimulateDB *me_;
	static int obj_counts;
	int writef;
	Fifo < Sptr <CDRSUBSET> > CdrSubsetContainer;
	CDRSUBSET cdr;
	
};
//#endif

}

#endif

