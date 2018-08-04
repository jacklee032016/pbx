#ifndef		__CALLMSGFACTORY_HXX__
#define		__CALLMSGFACTORY_HXX__
/*
* $Id: CallMsgBuilder.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "CommonMsg.hxx"
#include "EndPoint.hxx"

namespace Assist
{

//class EndPoint;

#define DATA_MEMBERS \
private:

#define MEMBER_FUNCTIONS \
public:

#define CTOR(name) \
_Call##name::_Call##name()

#define DECTOR(name) \
_Call##name::~_Call##name()

#define CP_CTOR(name) \
_Call##name::_Call##name(const _Call##name& robj)

#define OP_EVALUATE(name) \
_Call##name& _Call##name::operator=(const _Call##name& robj)

#define VIRTUAL_FUNCTION(name, fuun) \
const char* _Call##name::_##fuun()


/**
* Call Message declare template
* If declear addition data member, please define these functions:
*
* operator=
* copy ctor
*
* _Call##name(const _Call##name& robj); \
* _Call##name& operator=(const _Call##name& robj);\
*/

// ~_##Call##name();

#define MSG_DECLARE(name, type) \
class _Call##name : public CommonMsg \
{ \
public: \
_Call##name() : CommonMsg(type) {} \
~_##Call##name(){} \
virtual const char* _Describer(); \
MEMBER_FUNCTIONS \
DATA_MEMBERS \
}

/////////////////////////////////////

/**
* Declare call messages
* Use  DATA_MEMBERS or MEMBER_FUNCTIONS like follow to 
* add  data members or member functions for particular class
*
* #ifdef MEMBER_FUNCTIONS
* #undef MEMBER_FUNCTIONS
* #define MEMBER_FUNCTIONS \
* private: \
*	int foobar(){ return 0; }
* #endif 
*
* #ifdef DATA_MEMBERS
* #undef DATA_MEMBERS
* #define DATA_MEMBERS \
* private: \
*	int aaa;
* #endif
*
*   MSG_DECLARE(foo)
*   
*    As you see, the class foo have a aditional function foobar and a member aaa
*/


MSG_DECLARE(Alerting, CALL_ALERTING);
MSG_DECLARE(Connect, CALL_CONNECT);
MSG_DECLARE(DisConnect, CALL_DISCONNECT);
MSG_DECLARE(Information, CALL_INFORMATION);
MSG_DECLARE(Proceeding, CALL_PROCEEDING);
MSG_DECLARE(Setup, CALL_SETUP); 
MSG_DECLARE(SetupAck, CALL_SETUP_ACK);


#ifdef MEMBER_FUNCTIONS
#undef MEMBER_FUNCTIONS
#define MEMBER_FUNCTIONS \
	public: \
		int getCause(){ return myCause; } \
		void setCause(int _cause) { myCause=_cause; } 
#endif 

#ifdef DATA_MEMBERS
#undef DATA_MEMBERS
#define DATA_MEMBERS \
	private: \
		int myCause;
#endif

MSG_DECLARE(Release, CALL_RELEASE);

MSG_DECLARE(ReleaseComplete, CALL_RELEASE_COMPLETE);


#ifdef MEMBER_FUNCTIONS
#undef MEMBER_FUNCTIONS
#define MEMBER_FUNCTIONS \
	public: \
		Sptr <EndPoint> getMasterEndPoint(){ return myMasterEp; } \
		void setMasterEndPoint(Sptr <EndPoint> _master) { myMasterEp=_master; } \
		Sptr <EndPoint> getSlaveEndPoint(){ return mySlaveEp; } \
		void setSlaveEndPoint(Sptr <EndPoint> _slave) { mySlaveEp=_slave; } 
		
#endif 

#ifdef DATA_MEMBERS
#undef DATA_MEMBERS
#define DATA_MEMBERS \
	private: \
		Sptr <EndPoint> myMasterEp; \
		Sptr <EndPoint> mySlaveEp; 
#endif

MSG_DECLARE(ConnectAck, CALL_CONNECT_ACK);


}

#endif

