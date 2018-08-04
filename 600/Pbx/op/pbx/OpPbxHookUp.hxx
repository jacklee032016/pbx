#ifndef __OP_PBX_HOOK_UP_HXX__
#define __OP_PBX_HOOK_UP_HXX__

/*
$Id: OpPbxHookUp.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpPbxHookUp : public UaOperator
{
	public:
		OpPbxHookUp();

		virtual ~OpPbxHookUp();

		const char* const name() const;

		const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
