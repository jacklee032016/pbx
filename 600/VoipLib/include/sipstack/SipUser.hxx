#ifndef SipUser_HXX
#define SipUser_HXX
/*
 * $Id: SipUser.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

    
#include "global.h"
#include "Data.hxx"
#include "BaseUser.hxx"


namespace Vocal
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

 
} // namespace Vocal

#endif
