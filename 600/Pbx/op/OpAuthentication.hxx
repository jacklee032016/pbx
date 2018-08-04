#ifndef  __OPINVITEAUTHENTICATION_H__
#define  __OPINVITEAUTHENTICATION_H__

#include "UaOperator.hxx"
#include "SipProxyEvent.hxx"
#include "SipCommand.hxx"
#include "SipAuthorization.hxx"
#include "Data.hxx"
#include <map>

/* end added */


namespace Vocal
{


typedef map<int, FifoEventId> TimerEventMap;

class OpAuthentication : public UaOperator
{
	public:
        ///
        OpAuthentication();
        ///
        virtual ~OpAuthentication();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );

	///* added by wuaiwu for return call, 2005/4/11 */

	//private:
	//	void SavePrevCalls(const Sptr < SipProxyEvent > event, const Sptr<InviteMsg> sipMsg );
		
	/* end of added */

	private:
		void MakeAuthenString(char* md5String, const SipCommand& source, const Data& stype);
		//char* DiscardQuote(const char*);
		bool LocalVerifyUser(const SipAuthorization& author);
		void RemoteVeriyUser(const SipAuthorization& author);

		static TimerEventMap tvMap;
		
};
 
}

#endif 
