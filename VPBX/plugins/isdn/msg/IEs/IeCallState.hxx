#ifndef __IE_CALL_STATE_HXX__
#define __IE_CALL_STATE_HXX__
/*
* $Id: IeCallState.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/


#include "global.h"

#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

#define 	IE_CALLSTATE_CODING_TTC				0x00

#define 	IE_CALLSTATE_VALUE_NULL								0x00
#define 	IE_CALLSTATE_VALUE_CALL_INITED						0x01
#define 	IE_CALLSTATE_VALUE_OUT_PROCEEDING					0x03
#define 	IE_CALLSTATE_VALUE_DELIVERED							0x04
#define 	IE_CALLSTATE_VALUE_PRESENT							0x06
#define 	IE_CALLSTATE_VALUE_RECEIVED							0x07
#define 	IE_CALLSTATE_VALUE_CONN_REQUETS					0x08
#define 	IE_CALLSTATE_VALUE_IN_PROCEEDING					0x09
#define 	IE_CALLSTATE_VALUE_ACTIVE							0x0a
#define 	IE_CALLSTATE_VALUE_DISCONN_REQUEST					0x0b
#define 	IE_CALLSTATE_VALUE_DISCONN_IND						0x0c
#define 	IE_CALLSTATE_VALUE_SUSPEND_REQUEST					0x0f	/* state N/U 15 */
#define 	IE_CALLSTATE_VALUE_RESUME_REQUEST					0x11	/* state N/U 17 */
#define 	IE_CALLSTATE_VALUE_RELEASE_REQUEST					0x13	/* state N/U 19 */
#define 	IE_CALLSTATE_VALUE_N_CALL_ABORT						0x06	/* state N 22 */	

/*
* Page.79
* 3 bytes IE: MT, length, Coding+state_value
*/
class IeCallState : public IsdnIE
{
	public:
		IeCallState(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
//		IeCallState( int _notify);

		IeCallState( const IeCallState& src );

		int getCoding()
		{
			return coding;
		};

		void setCoding( int _coding)
		{
			coding = _coding;
		};

		int getState()
		{
			return callStateValue;
		};

		void setState( int _state)
		{
			callStateValue = _state;
		};

		IeCallState& operator=(const IeCallState& src);

		bool operator==(const IeCallState&) const;

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;

		unsigned char *encode(msg_t *msg, int ntMode);

		int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_NOTIFY;
		};

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:
		bool validate();
		
	private:
		int 			coding;
		int			callStateValue;
		
};
 
}

#endif

