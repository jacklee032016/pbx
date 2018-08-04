/*
* $Id: MsgCodec.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __MSG_CODEC_HXX__
#define __MSG_CODEC_HXX__

#include "global.h"
#include "vpbx_globals.h"

#include "Data.hxx"
#include "Sptr.hxx"
#include "VException.hxx"

#include "IsdnIE.hxx"
#include "IsdnMsg.hxx"

#include "as_msg.h"
#include "ask_isdn.h"


namespace Assist
{

class MsgCodec
{
	public:
		MsgCodec( );
		
		virtual ~MsgCodec();

		ISDN_METHOD getType(int prim);

		/* return data for L3 msg, contains msg header, such as Q931_info_t and SETUP_T */
		unsigned char *getL3Data(const msg_t *msg);
		
	protected:
		int 			ntMode;

		/* for NT User Space Stack, header is 8 bytes(prim+dinfo), for TE kernel Stack, it is 16 bytes(addr+prim+dinfo+len) */
		int 			headerLength;

	private:

};

}
#endif

