#ifndef OpQueryPbxStatus_HXX
#define OpQueryPbxStatus_HXX
/*
 * $Id: OpQueryPbxStatus.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <State.hxx>
#include <SipProxyEvent.hxx>


namespace Vocal
{

///
class OpQueryPbxStatus : public UaOperator
{
    public:
        ///
        OpQueryPbxStatus();
        ///
        virtual ~OpQueryPbxStatus();
        ///
        virtual const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
