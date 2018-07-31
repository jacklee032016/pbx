#ifndef __ISDN_IE_HXX__
#define __ISDN_IE_HXX__
/*
* $Id: IsdnIE.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Data.hxx"
#include "vpbx_globals.h"
#include "Sptr.hxx"
#include "VException.hxx"
#include "as_msg.h"

#include "as_isdn.h"

namespace Assist
{

#define	ISDN_IE_CONTENT_LENGTH		128


typedef enum 
{
	ISDN_IE_BEARER,
	ISDN_IE_CALL_ID,
	ISDN_IE_CHANNEL_ID,
	ISDN_IE_CAUSE,
	ISDN_IE_FACILITY,
	ISDN_IE_PROGRESS,
	ISDN_IE_NOTIFY,
	ISDN_IE_SEGMENT,
	ISDN_IE_CALL_STATE,
	ISDN_IE_NET_FAC,
	ISDN_IE_DISPLAY,
	ISDN_IE_DATE,
	ISDN_IE_KEYPAD,
	ISDN_IE_SIGNAL,
	ISDN_IE_INFORATE,
	ISDN_IE_E2E_TDELAY,
	ISDN_IE_TDELAY_SEL,
	ISDN_IE_PACK_BINPARA,
	ISDN_IE_PACK_WINSIZE,
	ISDN_IE_PACK_SIZE,
	ISDN_IE_CUG,
	ISDN_IE_REV_CHARGE,
	ISDN_IE_CONNECT_PN,
	ISDN_IE_CONNECT_SUB,
	ISDN_IE_CALLING_PN,
	ISDN_IE_CALLING_SUB,
	ISDN_IE_CALLED_PN,
	ISDN_IE_CALLED_SUB,
	ISDN_IE_REDIR_NR,
	ISDN_IE_REDIR_DN,
	ISDN_IE_TRANS_SEL,
	ISDN_IE_RESTART_IND,
	ISDN_IE_LLC,
	ISDN_IE_HLC,
	ISDN_IE_USER_USER,
	ISDN_IE_ESCAPE,
	ISDN_IE_CNIP, 		/* siemens centrex extension */
	ISDN_IE_SHIFT,
	ISDN_IE_MORE_DATA,
	ISDN_IE_COMPLETE,
	ISDN_IE_CONGESTION,
	ISDN_IE_REPEAT,
	ISDN_IE_CENTREX_FAC,
	ISDN_IE_CENTREX_ID,
	ISDN_IE_CHARGE,		/* 2006.06.11 */
	ISDN_IE_UNKNOWN
}IsdnIeType;

/// this is the number of headers in the IsdnIeType enum.
const int isdnIeCount = ISDN_IE_UNKNOWN + 1;

enum
{ /* number types */
	INFO_NTYPE_UNKNOWN,
	INFO_NTYPE_SUBSCRIBER,
	INFO_NTYPE_NATIONAL,
	INFO_NTYPE_INTERNATIONAL,
};

enum
{ /* number presentation */
	INFO_PRESENT_NULL,
	INFO_PRESENT_ALLOWED,
	INFO_PRESENT_RESTRICTED,
	INFO_PRESENT_NOTAVAIL,
};

enum
{ /* number presentation */
	INFO_SCREEN_USER, 			/* user provided */
	INFO_SCREEN_NETWORK, 		/* network provided */
};


enum
{
	ERROR_IE_IS_NULL = 1,
	ERROR_IE_TOO_SHORT,
	ERROR_IE_DATA_ERROR, 			/* IE type errror */
	ERROR_IE_CHANID_ERROR,
	ERROR_IE_NOTIFY_ERROR,
	ERROR_IE_CALLSUB_ERROR,
};

IsdnIeType headerTypeDecode(const Data& headerName);

Data headerTypeEncode(const IsdnIeType header);

class IsdnIeParserException : public VException
{
	public:
		IsdnIeParserException( const string& msg, const string& file,  const int line,  const int error = 0 );

//		IsdnIeErrorType getError() const
		int getError() const
		{
			return value;
		}

		string getName() const ;

	private:
//		IsdnIeErrorType value;
		int value;

};


class IsdnIE
{
	public:
		/* encode to create a IE */
		IsdnIE(void *data,int ntMode =0 );

		IsdnIE();

		IsdnIE(const unsigned char *p)  throw(IsdnIeParserException &);

		
		IsdnIE(const IsdnIE& src);
		
		virtual ~IsdnIE();

		virtual IsdnIE* duplicate() const = 0;

		virtual bool compareIsdnIE(IsdnIE* msg) const = 0;

		virtual int getIeType() const = 0;

		bool isEmpty() const ;

#if WITH_DEBUG
		virtual const char *debugInfo() = 0;
#endif
		
		/* encode this IE in msg_t(L3) created externally */
		virtual unsigned char *encode(msg_t *msg, int ntMode) =0;

		/* return 0: decode success; others : failed, throw exception 
		* p is in the postion of IE type in order to error check 
		*/
		virtual int decode(const unsigned char *p) = 0;

	protected:
		int 			deviceId;			/* for D channel device */

		bool 		emptyFlg;
};
   
}

#endif

