#ifndef NULLUSER_HXX_
#define NULLUSER_HXX_
/*
 * $Id: NullUser.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "BaseUser.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
