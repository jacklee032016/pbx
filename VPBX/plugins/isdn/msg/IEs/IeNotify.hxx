#ifndef __IE_NOTIFY_HXX__
#define __IE_NOTIFY_HXX__
/*
* $Id: IeNotify.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/


#include "global.h"

#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

#define 	IE_NOTIFY_USER_SUSPEND		0x00
#define 	IE_NOTIFY_USER_RESUME		0x01

/*
* Page.123
* 3 bytes IE: MT, length,Content
*/
class IeNotify : public IsdnIE
{
	public:
		IeNotify(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
		IeNotify( int _notify);

		IeNotify( const IeNotify& src );

		int getNotify()
		{
			return notify;
		};

		void setNotify( int _notify)
		{
			notify = _notify;
		};

		IeNotify& operator=(const IeNotify& src);

		bool operator==(const IeNotify&) const;

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
		int notify;
		
};
 
}

#endif

