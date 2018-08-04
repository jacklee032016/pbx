#ifndef OpRing_HXX
#define OpRing_HXX
/*
 * $Id: OpRing.hxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "UaOperator.hxx"
#include "SipProxyEvent.hxx"
#include "InviteMsg.hxx"

/* added by wuaiwu for return call, 2005/4/11 */
#include <deque>
#include <string>
using namespace std;

/* end added */


namespace Vocal
{


class OpRing : public UaOperator
{
    public:
        ///
        OpRing();
        ///
        virtual ~OpRing();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );

	/* added by wuaiwu for return call, 2005/4/11 */

	private:
		void SavePrevCalls(const Sptr < SipProxyEvent > event, const Sptr<InviteMsg> sipMsg );
		
	/* end of added */
};
 
}

#endif
