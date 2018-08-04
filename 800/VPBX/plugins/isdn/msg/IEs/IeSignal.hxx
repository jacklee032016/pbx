#ifndef __IE_SIGNAL_HXX__
#define __IE_SIGNAL_HXX__
/*
* $Id: IeSignal.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

#define IE_SIGNAL_VALUE_DIAL_ON					0x00
#define IE_SIGNAL_VALUE_CALL_WAIT_ON				0x07
#define IE_SIGNAL_VALUE_TONE_OFF					0x3F

/* Specs, p130  net-->user signal 
* 3 bytes for this IE: MT, length,value
*/
class IeSignal : public IsdnIE
{
	public:
		IeSignal(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
		IeSignal(int _signal);
		IeSignal( const IeSignal& src );

		bool operator ==( const IeSignal& src ) const;
		const IeSignal& operator=( const IeSignal& src );

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;

		unsigned char *encode(msg_t *msg, int ntMode);

		int decode(const unsigned char *p);

		virtual int getIeType() const
		{
			return ISDN_IE_SIGNAL;
		};
		

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:	

	private:
		int 		signal;

};
 
}

#endif

