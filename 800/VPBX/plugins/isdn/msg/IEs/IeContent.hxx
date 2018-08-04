#ifndef __IE_CONTENT_HXX__
#define __IE_CONTENT_HXX__
/*
* $Id: IeContent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

//#include "global.h"
#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

class IeContent : public IsdnIE
{
	public:
		IeContent( const unsigned char  *p)  throw(IsdnIeParserException &);
		
		IeContent();

		IeContent(const IeContent& src);
		
		~IeContent();
		
		const IeContent& operator=(const IeContent& src);

		bool operator == (const IeContent& other) const;
		bool operator != (const IeContent& other) const
		{
			return !(*this == other);
		}

		bool operator < (const IeContent& other) const;
		bool operator > (const IeContent& other) const;
		
		virtual int decode(const unsigned char *p);
		
//		IsdnIE* duplicate() const;
//		virtual bool compareIsdnIE(IsdnIE* msg) const;
/*
*/		
//		virtual char *encode( ) const;

		void setContent( const unsigned char *_content);

		const unsigned char *getContent();

		int getLength();
		
#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
	protected:	
		unsigned char 		content[ISDN_IE_CONTENT_LENGTH];
		int 					length;
		
	private:
		
};
 
}

#endif

