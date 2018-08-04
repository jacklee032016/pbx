#ifndef BaseUser_HXX
#define BaseUser_HXX

/*
* $Log: BaseUser.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: BaseUser.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
    
#include "global.h"
#include "Data.hxx"
#include "Sptr.hxx"


namespace Assist
{
    
/// enum for type of user class
typedef enum
{
    SIP_USER,
    TEL_USER,
    NULL_USER
}UserType;


/// pure virtual class for Url types
class BaseUser
{
public:
    /// Create one with default values
    BaseUser();
    ///
    virtual ~BaseUser();
    //parser functions
    virtual bool decode(const Data& data) = 0;
    ///
    virtual Data encode() const = 0;
    ///
    virtual Sptr<BaseUser> duplicate() const = 0;
    ///
    virtual UserType getType() const = 0;
    ///
    virtual bool areEqual(Sptr<BaseUser>) const = 0;
    ///
    virtual bool isLessThan(Sptr<BaseUser>) const = 0;
};

bool 
operator<(const Sptr<BaseUser>& left, const Sptr<BaseUser>& right);

}

#endif
