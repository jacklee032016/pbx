#ifndef __IE_CONNECT_PN_HXX__
#define __IE_CONNECT_PN_HXX__
/*
* $Id: IeConnectPN.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "IePN.hxx"
#include "VException.hxx"


namespace Assist
{

class IeConnectPN : public IePN
{
	public:
		IeConnectPN(const char *p)  throw(IsdnIeParserException &);
		
		IeConnectPN(int _type, int _plan, char *_number);

		IeConnectPN( const IeConnectPN& src );

		~IeConnectPN( );

		bool operator ==( const IeConnectPN& src ) const;
		bool operator <( const IeConnectPN& src ) const;
		const IeConnectPN& operator =(const IeConnectPN& src);

		void setScreen( int _screen)
		{
			screen = _screen;
		};

		int getScreen() 
		{
			return screen;
		};

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;

		char *encode( msg_t *msg);

//		int decode(const char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_CONNECT_PN;
		};

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:
		int vlidateType(int _type);
		
	private:

};
 
}

#endif

