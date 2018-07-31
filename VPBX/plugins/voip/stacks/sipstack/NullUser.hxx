#ifndef NULLUSER_HXX_
#define NULLUSER_HXX_

/*
* $Log: NullUser.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: NullUser.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "BaseUser.hxx"

namespace Assist
{

/// pure virtual class for Url types
class NullUser : public BaseUser
{
public:
    /// Create one with default values
    NullUser();
    ///
    virtual ~NullUser();
    //parser functions
    virtual bool decode(const Data& data);
    ///
    virtual Data encode() const;
    ///
    virtual Sptr<BaseUser> duplicate() const;
    ///
    virtual UserType getType() const;
    ///
    virtual bool areEqual(Sptr<BaseUser>) const;
    ///
    virtual bool isLessThan(Sptr<BaseUser>) const;
};
 
}

#endif
