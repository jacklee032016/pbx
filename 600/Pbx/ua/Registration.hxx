#ifndef Registration_HXX
#define Registration_HXX
/*
 * $Id: Registration.hxx,v 1.3 2007/03/06 18:56:21 lizhijie Exp $
 */

#include <Data.hxx>
#include <list>
#include <RegisterMsg.hxx>
#include <StatusMsg.hxx>

namespace Vocal
{

class Registration
{
    public:
        Registration();
	 Registration(bool isCompart);
        Registration(const RegisterMsg& srcMsg);

	/* added this function for cseq which could make request as retransmit, li Zhijie, 2007.03.06 */
        Registration(const RegisterMsg& srcMsg, unsigned int cseq);
		
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
	  bool isCompart() const{return compart;}
	  void setExpire(){expire = true;}
	  bool isExpired() const {return expire;}

    private:

        SipContact findMyContact(const StatusMsg& msg) const;

        int status;
        RegisterMsg registerMsg;
	 bool compart;
	 bool expire;
};
 
}

#endif
