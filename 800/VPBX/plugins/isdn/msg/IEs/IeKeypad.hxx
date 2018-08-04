#ifndef __IE_KEYPAD_HXX__
#define __IE_KEYPAD_HXX__
/*
* $Id: IeKeypad.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
//#include <map>

#include "global.h"
#include "IeContent.hxx"
#include "VException.hxx"

namespace Assist
{
/*
* p.104 Keypad Facility IE 
* variable length IE: MT, length, content(IA5 chars)
*/

class IeKeypad : public IeContent
{
	public:
		IeKeypad(const unsigned char *p)  throw(IsdnIeParserException &) ;
		IeKeypad( );
		IeKeypad(const IeKeypad&);
		~IeKeypad();

		IeKeypad& operator = (const IeKeypad&);
		bool operator ==(const IeKeypad& srcDiversion) const;
		bool operator< (const IeKeypad& src) const;

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;

		unsigned char *encode( msg_t *msg, int ntMode);
		
		virtual int getIeType() const
		{
			return ISDN_IE_KEYPAD;
		};

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected :	
		
	private:
		
};
 
}

#endif

