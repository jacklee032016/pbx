/*
* $Id: MsgDecoder.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "MsgDecoder.hxx"

using namespace Assist;

MsgDecoder::MsgDecoder()
{
}
		
MsgDecoder::~MsgDecoder()
{
}

ISDN_METHOD MsgDecoder::getType(const msg_t *msg)
{
	return getType( getPrimType(msg) );
}

