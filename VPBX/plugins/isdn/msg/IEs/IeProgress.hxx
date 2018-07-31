#ifndef __IE_PROGRESS_HXX__
#define __IE_PROGRESS_HXX__
/*
* $Id: IeProgress.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

#define IE_PROGRESS_CODING_TTC	0x0

#define IE_PROGRESS_LOCATION_USER							0x0
#define IE_PROGRESS_LOCATION_PRIVATE_LOCAL_USER			0x1
#define IE_PROGRESS_LOCATION_PUBLIC_LOCAL_USER			0x2
#define IE_PROGRESS_LOCATION_TRANSIT_NETWORK			0x3
#define IE_PROGRESS_LOCATION_RPIVATE_REMOTE_USER		0x4
#define IE_PROGRESS_LOCATION_PUBLIC_REMOTE_USER			0x5
#define IE_PROGRESS_LOCATION_BEYOND_INTERNETWORK		0xa

#define IE_PROGRESS_CONTENT_NOT_ISDN_E2E				0x00
#define IE_PROGRESS_CONTENT_NOT_ISDN_DEST				0x02
#define IE_PROGRESS_CONTENT_NOT_ISDN_SRC				0x03
#define IE_PROGRESS_CONTENT_RETURN_2_ISDN				0x04
#define IE_PROGRESS_CONTENT_INBAND_SIGNAL				0x08

/*
* P.124, 4 bytes MT,Length,coding+location, progress content
*/
class IeProgress : public IsdnIE
{
	public:
		IeProgress(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
		IeProgress(int coding = IE_PROGRESS_CODING_TTC	, 
			int location = IE_PROGRESS_LOCATION_PUBLIC_LOCAL_USER, 
			int progress = IE_PROGRESS_CONTENT_INBAND_SIGNAL);
		
		IeProgress( const IeProgress& src);
		~IeProgress( );

		bool operator ==( const IeProgress& src) const;
		bool operator<( const IeProgress& src) const;
		const IeProgress& operator=(const IeProgress& src);

		IsdnIE* duplicate() const;

		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		unsigned char *encode( msg_t *msg, int ntMode);

		int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_PROGRESS;
		};


#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
	protected :
	
	private:
		int coding;
		int location;
		int progress;

};
 
} 

#endif

