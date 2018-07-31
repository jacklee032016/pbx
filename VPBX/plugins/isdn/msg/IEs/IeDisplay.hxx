#ifndef __IE_DIAPLAY_HXX__
#define __IE_DIAPLAY_HXX__
/*
* $Id: IeDisplay.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "IeContent.hxx"
#include "VException.hxx"

namespace Assist
{

/*
* p.99 
* variable length IE: MT, length, content(IA5 chars)
*/
class IeDisplay : public IeContent
{
	public:
		IeDisplay(const unsigned char *p)  throw(IsdnIeParserException &) ;
		IeDisplay();
		IeDisplay( const IeDisplay& src);
		~IeDisplay();

		bool operator ==( const IeDisplay& src) const;
		const IeDisplay& operator=( const IeDisplay& src);

		IsdnIE* duplicate() const;

		virtual bool compareIsdnIE(IsdnIE* msg) const;

		unsigned char *encode(msg_t *msg, int ntMode);
		
		virtual int getIeType() const
		{
			return ISDN_IE_DISPLAY;
		};

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:

	private:

};
 
}

#endif

