#ifndef __IE_PN_HXX__
#define __IE_PN_HXX__
/*
* $Id: IePN.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "IeCalledPN.hxx"
#include "VException.hxx"


namespace Assist
{
#define PN_MAX_LENGTH								36		/* 35 in Called PN */



enum
{ /* redirection reason */
	INFO_REDIR_UNKNOWN,
	INFO_REDIR_BUSY,
	INFO_REDIR_NORESPONSE,
	INFO_REDIR_UNCONDITIONAL,
	INFO_REDIR_OUTOFORDER,
	INFO_REDIR_CALLDEFLECT,
};



/* base class for ConnectPN, CalledPN */
class IePN : public IeCalledPN
{
	public:
		IePN(const unsigned char *p)   throw(IsdnIeParserException &);
		
		IePN(int _type, int _plan,  const unsigned char *_number);

		IePN( const IePN& src );
		
		IePN( );

		bool operator ==( const IePN& src ) const;
		bool operator <( const IePN& src ) const;
		const IePN& operator =(const IePN& src);

		void setPresent( int _present)
		{
			present = _present;
		};
		
		int getPresent() 
		{
			return present;
		};

		void setScreen( int _screen)
		{
			screen = _screen;
		};

		int getScreen() 
		{
			return screen;
		};

		/* can be inhirented in child class */
		virtual const unsigned char *getPartyNumber() const;
			
		virtual void *encode( ) const;

		virtual int decode(const unsigned char *p);
		
//		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;

#if WITH_DEBUG
		virtual const char *debugInfo();
		const char *getPresentName();
		const char *getScreenName();
#endif
	protected:

		/* only used in Calling Party Number */
		int 		present;
		int 		screen;

		int validateScreen(int _screen);
		int validatePresent( int _present);
		

	private:
		
};
 
}

#endif

