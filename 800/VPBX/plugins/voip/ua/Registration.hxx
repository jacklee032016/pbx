#ifndef __REGISTRATION_HXX__
#define __REGISTRATION_HXX__
/*
* $Id: Registration.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <Data.hxx>
#include <list>
#include <RegisterMsg.hxx>
#include <StatusMsg.hxx>

namespace Assist
{

class Registration
{
	public:
		Registration();
		Registration(bool isCompart);
		Registration(const RegisterMsg& srcMsg);
		Registration(const Registration& src);

		~Registration();

		RegisterMsg getNextRegistrationMsg();
		RegisterMsg getNextRegistrationCancel();

		RegisterMsg getRegistrationMsg()
		{
			return registerMsg;
		};

		int updateRegistrationMsg(const StatusMsg& msg);
		void setRegistrationMsg(const RegisterMsg& msg);

		Registration& operator =(const Registration&);
		bool operator ==( Registration& src ) const;

		int getStatusCode() const
		{
			return status;
		};

		int getDelay();

		unsigned int seqNum;

		bool isCompart() const
		{
			return compart;
		}

		void setExpire()
		{
			expire = true;
		}

		bool isExpired() const 
		{
			return expire;
		}

	private:

		SipContact findMyContact(const StatusMsg& msg) const;

		int 							status;
		RegisterMsg 					registerMsg;
		bool 						compart;
		bool 						expire;
};
 
}

#endif

