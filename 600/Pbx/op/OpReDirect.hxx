#ifndef OpReDirect_HXX
#define OpReDirect_HXX

/*
 * $Id: OpReDirect.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpReDirect : public UaOperator
{
    public:
        ///
        OpReDirect();
        ///
        virtual ~OpReDirect();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
