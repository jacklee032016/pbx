#ifndef  __OPAUTHORIZATION_H__
#define __OPAUTHORIZATION_H__

/*
 * $Id: OpAuthorization.hxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include "SipProxyEvent.hxx"
#include "InviteMsg.hxx"


/* end added */


namespace Vocal
{


class OpAuthorization : public UaOperator
{
    public:
        ///
        OpAuthorization();
        ///
        virtual ~OpAuthorization();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );

};
 
}

#endif
