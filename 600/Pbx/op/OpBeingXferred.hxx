#ifndef OpBeingXferred_HXX
#define OpBeingXferred_HXX
/*
 * $Id: OpBeingXferred.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{


class OpBeingXferred : public UaOperator
{
    public:
        ///
        OpBeingXferred();
        ///
        virtual ~OpBeingXferred();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
