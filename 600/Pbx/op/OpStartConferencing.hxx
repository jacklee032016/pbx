#ifndef OpStartConferencing_HXX
#define OpStartConferencing_HXX
/*
 * $Id: OpStartConferencing.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>


namespace Vocal
{

class OpStartConferencing : public UaOperator
{
    public:
        ///
        OpStartConferencing();
        ///
        virtual ~OpStartConferencing();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
