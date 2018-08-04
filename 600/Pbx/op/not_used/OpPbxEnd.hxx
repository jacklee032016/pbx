#ifndef __OP_PBX_END_HXX__
#define __OP_PBX_END_HXX__

/*
$Id: OpPbxEnd.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpPbxEnd : public UaOperator
{
	public:
		OpPbxEnd();

		virtual ~OpPbxEnd();

		const char* const name() const;

		const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
