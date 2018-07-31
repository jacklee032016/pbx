#ifndef __CONTACT_HXX__
#define __CONTACT_HXX__
/*
* $Id: Contact.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <InviteMsg.hxx>
#include <StatusMsg.hxx>

namespace Assist
{

class BaseUrl;
class InviteMsg;
class SipCallLeg;
class SipContact;
class SipMsg;
class SipStatusLine;
class SipUrl;
class StatusMsg;

/// Keeps track of the one destination that we are trying
class Contact
{
	public:
		Contact( const Contact& c );
		Contact( const InviteMsg& msg );

		/// A update msg has arrived from this contact, may want to save some stuff
		void update( const StatusMsg& msg );

		/// the location being tried - is just the Request-URI part
		Sptr <BaseUrl> getLocation();

		///get the call sequence number
		unsigned int getCSeqNum()
		{
			return cSeqNum;
		};

		void setCSeqNum(unsigned int cseq);

		InviteMsg getInviteMsg();
		StatusMsg getStatusMsg();

		/// The branch number of this attempt
		int getBranch();

		/// the best 3 digit SIP status code returned - initally 0
		int getStatus();

		/// Call leg for this attempt
		SipCallLeg computeCallLeg() const;

		void setLastInfoCSeq(unsigned int cseq);
		unsigned int getLastInfoCSeq();

		void setRemoteRingback( bool flag );
		bool getRemoteRingback() const;

		/// Check if the CallLeg, Location, and Branch match
		bool operator==( const SipMsg& msg );
		bool operator==( const Contact& other ) const;

		/// oposite of == operator
		bool operator!=( const Contact& other );

		void operator=(const Contact& src );


	private:
		unsigned int 						lastInfoCSeq;
		int 								status;			/* 3 digit status code of SIP msg */
		unsigned int 						cSeqNum;

		InviteMsg 						inviteMsg;
		StatusMsg 						statusMsg;

		bool 							remoteRingback;
};
 
}

#endif

