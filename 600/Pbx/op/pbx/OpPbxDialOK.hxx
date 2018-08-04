#ifndef __OP_PBX_DIAL_OK_HXX__
#define __OP_PBX_DIAL_OK_HXX__

/*
$Id: OpPbxDialOK.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpPbxDialOK : public UaOperator
{
	public:
		OpPbxDialOK();

		virtual ~OpPbxDialOK();

		const char* const name() const;

		const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
