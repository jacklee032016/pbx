#ifndef __IE_CENTRIX_HXX__
#define __IE_CENTRIX_HXX__
/*
* $Id: IeCentrex.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "IsdnIE.hxx"
#include "Data.hxx"

namespace Assist
{

/* facility for siemens CENTEX (known parts implemented only) */
class IeCentrex : public IsdnIE
{
	public:
		IeCentrex();
		IeCentrex( const IeCentrex& src );

		const IeCentrex& operator=(const IeCentrex& rhs)
		{
			if (this != &rhs)
			{
				time = rhs.time;
				delay = rhs.delay;
			}
			return *this;
		}

		bool operator ==( const IeCentrex& src ) const;

		int getTime();
		void setTime( const int& newTime);

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;
		unsigned char *encode(msg_t *msg, int ntMode) ;

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected :

	private:
		
};
 
}

#endif

