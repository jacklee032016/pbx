#ifndef OpRevert_HXX
#define OpRevert_HXX
/*
 * $Id: OpRevert.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{


class OpRevert : public UaOperator
{
    public:
        ///
        OpRevert();
        ///
        virtual ~OpRevert();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
