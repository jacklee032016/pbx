#ifndef OpRecvdXfer_HXX
#define OpRecvdXfer_HXX
/*
 * $Id: OpRecvdXfer.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpRecvdXfer : public UaOperator
{
    public:
        ///
        OpRecvdXfer();
        ///
        virtual ~OpRecvdXfer();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
