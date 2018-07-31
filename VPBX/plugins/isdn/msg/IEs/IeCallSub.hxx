#ifndef __IE_CALL_SUB_HXX__
#define __IE_CALL_SUB_HXX__
/*
* $Id: IeCallSub.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IeContent.hxx"

#include "VException.hxx"

namespace Assist
{

#define IE_CALLSUB_TYPE_NASP							0x0
#define IE_CALLSUB_TYPE_USER							0x2

#define IE_CALLSUB_IS_EVEN								0x0
#define IE_CALLSUB_IS_ODD								0x1

#define IE_CALLSUB_MAX_LENGTH							23

/*
* p.83
* represent both Called SubAddress and Calling SubAddress
* variable length IE: MT, length, type+isOdd+spare, subAddress, max length is 23 
*/
class IeCallSub : public IeContent
{
	public:
		IeCallSub( const unsigned char *p)  throw(IsdnIeParserException &) ;
		IeCallSub( int _isCalling);
		IeCallSub(const IeCallSub&);
		~IeCallSub( );
		
		IeCallSub& operator = (const IeCallSub& src);

		bool operator ==(const IeCallSub& src) const;
		
		const int getType() const;
		void setType(int _type);
		
		const int getIsOdd() const;
		void setIsOdd(int _isOdd);
		
		/// method for copying sip headers of any type without knowing which type
		IsdnIE* duplicate() const;

		/// compare two headers of (possibly) the same class
		virtual bool compareIsdnIE(IsdnIE* msg) const;
	
		unsigned char *encode(msg_t *msg, int ntMode);

		int decode(const unsigned char *p);
		
		virtual int getIeType() const;
		

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
	protected:
		bool validate();

	private:
		int 				type;
		int				isOdd;	/*when SubAddress type is NASP(BCD code)*/

		/* this data memner is not for IE, just for shared by calling or called*/
		int				isCalling;	/*1 : calling Sub; 0: called Sub*/
		
		
};
 
}
#endif

