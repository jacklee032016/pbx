#ifndef OpRingCancel_HXX
#define OpRingCancel_HXX
/*
 * $Id: OpRingCancel.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpRingCancel : public UaOperator
{
    public:
        ///
        OpRingCancel();
        ///
        virtual ~OpRingCancel();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
