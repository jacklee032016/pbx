#ifndef OpBranchIdle_HXX
#define OpBranchIdle_HXX
/*
 * $Id: OpBranchIdle.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpBranchIdle : public UaOperator
{
    public:
        ///
        OpBranchIdle();
        ///
        virtual ~OpBranchIdle();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
