#ifndef SipUser_HXX
#define SipUser_HXX
/*
* $Log: SipUser.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipUser.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
    
#include "global.h"
#include "Data.hxx"
#include "BaseUser.hxx"


namespace Assist
{
    
/// class for usernames in sip: URLs
class SipUser : public BaseUser
{
    public:
    /// Create one with default values
    SipUser();
    ///
    virtual ~SipUser();
    ///
    SipUser(const SipUser &);
    ///
    const SipUser & operator=(const SipUser &);
       
    //parser functions
    ///
    SipUser(const Data& data);
    ///
    Sptr<BaseUser> duplicate() const;
    ///
    bool decode(const Data& data);
    ///
    Data encode() const;
    ///
    bool operator==(const SipUser&) const;
    ///
    bool operator < (const SipUser&) const;
    ///
    UserType getType() const;
    ///
    bool areEqual(Sptr<BaseUser> baseUser) const;
    ///
    bool isLessThan(Sptr<BaseUser> baseUser) const;
    
private:
    Data user;
};

 
}

#endif
