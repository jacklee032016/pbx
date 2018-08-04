#ifndef		__COMMONMSG_HXX__
#define		__COMMONMSG_HXX__
/*
* $Id: CommonMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Sptr.hxx"
#include "CallId.hxx"
#include <string>
using namespace std;

namespace Assist
{

class EndPoint;
class CallId;

typedef  enum
{
	CALL_SETUP = 0,
	CALL_SETUP_ACK,
	CALL_INFORMATION,
	CALL_PROCEEDING,
	CALL_ALERTING,
	CALL_CONNECT,
	CALL_CONNECT_ACK,
	CALL_DISCONNECT,
	CALL_RELEASE,
	CALL_RELEASE_COMPLETE,
	CALL_UNKNOWN = 99
	
}call_method_t;

/**
*	Base class for message known to CallScheduler
*  This is a value object, just like IsdnMsg, so no EndPoint, AudioChannel and CallId is store in this object
*  EndPoint is keep in PbxEvent, CallId and AudioChannel is keep in EndPoint, 
*  For some event with out EndPoint, such as Call Timeout event, CallId is added into PbxEvent also
*/
class CommonMsg
{
public:
	CommonMsg();
	explicit CommonMsg(call_method_t type);
	virtual ~CommonMsg();
	CommonMsg(const CommonMsg& rmsg);
	const CommonMsg& operator=(const CommonMsg& rmsg);
	
public:

	void setFrom(const char* newFrom);
	const char* getFrom() const;

	void setTo(const char* newcallee);
	const char* getTo() const;
	
		dtmf_signal_type_t getPeerSignalType(); 
		void setSignalType(dtmf_signal_type_t _type);
#if 0
	/**
	*/
	void setFromEpId(int Id);

	/**
	*/
	void setFromAuId(int Id);
	
	/**
	*/

	/**
	*/
	int getFromEpId() const;

	/**
	*/
	int getFromAuId() const;
#endif 

#if 0
	void setEndPoint(Sptr <EndPoint> ep);
	Sptr<EndPoint> getEndPoint() const;

	int getToEpId() const;
	void setToEpId(int Id);
	int getToAuId() const;
	void setToAuId(int Id);
	const CallId& getCallId() const;
	void setCallId(Sptr <CallId> Id);
#endif

	/**
	*/
	const string& getMethodName() const ;
	
	/**
	*/
	virtual call_method_t getMethod() const ;

	/**
	*  When add a virtual function MUST follow the naming rule start with a "_"
	*  eg:
	*  virtual void _foobar();
	*  due to the GNU preprocessor
	*/
	virtual const char* _Describer() = 0;

		

protected:
	///
	string   strFrom;
	///
	string   strTo;
	///
	string   strMethod;
	///
	call_method_t method;

		dtmf_signal_type_t		peerSignalType;
		
#if 0
	Sptr <EndPoint> epFrom;
	///
	int aidTo;
	///
	//int eidFrom;
	///
	int eidTo;
	///
	Sptr <CallId> callId;
#endif

private:
	void copyValue(const CommonMsg& rmsg);

	
};

}

#endif


