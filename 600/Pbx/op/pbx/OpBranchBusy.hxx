#ifndef OpBranchBusy_HXX
#define OpBranchBusy_HXX
/*
 * $Id: OpBranchBusy.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpBranchBusy : public UaOperator
{
    public:
        ///
        OpBranchBusy();
        ///
        virtual ~OpBranchBusy();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
