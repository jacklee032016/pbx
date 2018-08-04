#ifndef OpTimePbxEnd_HXX
#define OpTimePbxEnd_HXX
/*
 * $Id: OpTimePbxEnd.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include <SipProxyEvent.hxx>
#include "UaOperator.hxx"

namespace Vocal
{

class OpTimePbxEnd : public UaOperator
{
    public:
        ///
        OpTimePbxEnd();
        ///
        virtual ~OpTimePbxEnd();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
