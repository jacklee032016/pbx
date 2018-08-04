#ifndef OpAddPbxDigit_HXX
#define OpAddPbxDigit_HXX
/*
 * $Id: OpAddPbxDigit.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpAddPbxDigit : public UaOperator
{
    public:
        ///
        OpAddPbxDigit();
        ///
        virtual ~OpAddPbxDigit();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
