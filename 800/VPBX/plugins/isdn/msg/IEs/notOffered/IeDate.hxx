#ifndef __IE_DATE_HXX__
#define __IE_DATE_HXX__
/*
* $Id: IeDate.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Data.hxx"
#include "IsdnIE.hxx"
#include "VException.hxx"

#include <stdio.h>
#include <sys/time.h>


namespace Assist
{
/*
* Date/Time is defined but not offered in INS, p.98 
*/
class IeDate : public IsdnIE
{
	public:
		IeDate(const char *p)  throw(IsdnIeParserException &) ;
		
		IeDate();
		IeDate( const IeDate& src );

		bool operator ==(const IeDate& src) const;
		bool operator <(const IeDate& src) const;
		const IeDate& operator=( const IeDate& src );

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		char *encode(msg_t *msg);

		int decode(const char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_DATE;
		};
		
#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:

	private:
		time_t 		ti;
		int 			seconds;
};
 
}

#endif

