#ifndef OpConvertCw_HXX
#define OpConvertCw_HXX
/*
 * $Id: OpConvertCw.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpConvertCw : public UaOperator
{
    public:
        ///
        OpConvertCw();
        ///
        virtual ~OpConvertCw();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
