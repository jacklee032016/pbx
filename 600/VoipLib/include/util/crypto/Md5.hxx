#if 0
#ifndef MD5_HXX_
#define MD5_HXX_

/*
 * $Id: Md5.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "Data.hxx"
#include "vmd5.h"

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to do cryptographic things.
 */
namespace Crypto
{


/** class to implement the MD5 message digest algorithm.
 */
class Md5
{
    public:
	/// constructor
	Md5();

	/// copy constructor -- returns the same state as the original
	Md5(const Md5& src);

	/// initialize and take a data
	Md5(const Data& data);

	/// equality operator 
	bool operator==(const Md5& src) const;

	/// less than operator
	bool operator<(const Md5& src) const;

	/// greater than operator
	bool operator>(const Md5& src) const;

	/// set the digest in an empty md5
	void digest(const Data& data);

	/// hash function
	size_t hashfn() const;

	/// destructor
	~Md5();

    private:
	MD5Context myContext;
	unsigned char myDigest[16];
};


inline
Md5::Md5()
{
    memset(myDigest, 0, 16);
}


inline
Md5::Md5(const Md5& src)
{
    memcpy(myDigest, src.myDigest, 16);
}


inline
Md5::Md5(const Data& data)
{
    MD5Init(&myContext);
    LocalScopeAllocator lo;
    MD5Update(&myContext, 
	      reinterpret_cast<const unsigned char*>(data.getData(lo)), 
	      static_cast<unsigned int>(data.length()));
    MD5Final(myDigest, &myContext);
}


inline void
Md5::digest(const Data& data)
{
    MD5Init(&myContext);
    LocalScopeAllocator lo;
    MD5Update(&myContext, 
	      reinterpret_cast<const unsigned char*>(data.getData(lo)), 
	      static_cast<unsigned int>(data.length()));
    MD5Final(myDigest, &myContext);
}


inline size_t
Md5::hashfn() const
{
    return *reinterpret_cast<size_t const*>(myDigest);
}


inline
Md5::~Md5()
{
}


inline bool
Md5::operator==(const Md5& src) const
{
    if(&src == this)
    {
	return true;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) == 0);
    }
}


inline bool
Md5::operator<(const Md5& src) const
{
    if(&src == this)
    {
	return false;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) < 0);
    }
}


inline bool
Md5::operator>(const Md5& src) const
{
    if(&src == this)
    {
	return false;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) > 0);
    }
}


} // namespace Crypto
} // namespace Vocal
#endif

#endif

#ifndef MD5_HXX_
#define MD5_HXX_

/*
 * $Id: Md5.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include "Data.hxx"
#include "vmd5.h"
#include <stdio.h>

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to do cryptographic things.
 */
namespace Crypto
{


/** class to implement the MD5 message digest algorithm.
 */
class Md5
{
    public:
	/// constructor
	Md5();

	/// copy constructor -- returns the same state as the original
	Md5(const Md5& src);

	/// initialize and take a data
	Md5(const Data& data);

	/// equality operator 
	bool operator==(const Md5& src) const;

	/// less than operator
	bool operator<(const Md5& src) const;

	/// greater than operator
	bool operator>(const Md5& src) const;

	/// set the digest in an empty md5
	void digest(const Data& data);

	/// hash function
	size_t hashfn() const;

	/// destructor
	~Md5();

	void Print()
	{
		for (int i = 0; i < 16; ++i)
		{
			printf("%02X, ", myDigest[i]);
		}
		printf("\n");
	}

    private:
	MD5Context myContext;
	unsigned char myDigest[16];
};


inline
Md5::Md5()
{
    memset(myDigest, 0, 16);
}


inline
Md5::Md5(const Md5& src)
{
    memcpy(myDigest, src.myDigest, 16);
}


inline
Md5::Md5(const Data& data)
{
    MD5Init(&myContext);
    LocalScopeAllocator lo;
   /* modidy by wuaiwu, 2005/07/12 */
    MD5Update(&myContext,  //reinterpret_cast<
	      (unsigned char*)(data.getData(lo)), 
	      static_cast<unsigned int>(data.length()));
  /* end of modify*/
    MD5Final(myDigest, &myContext);
}


inline void
Md5::digest(const Data& data)
{
    MD5Init(&myContext);
    LocalScopeAllocator lo;
    /* modify by wuaiwu, 2005/07/12 */
    MD5Update(&myContext,  //reinterpret_cast<const 
	      (unsigned char*)(data.getData(lo)), 
	      static_cast<unsigned int>(data.length()));
    /* end of modify */
    MD5Final(myDigest, &myContext);
}


inline size_t
Md5::hashfn() const
{
    return *reinterpret_cast<size_t const*>(myDigest);
}


inline
Md5::~Md5()
{
}


inline bool
Md5::operator==(const Md5& src) const
{
    if(&src == this)
    {
	return true;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) == 0);
    }
}


inline bool
Md5::operator<(const Md5& src) const
{
    if(&src == this)
    {
	return false;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) < 0);
    }
}


inline bool
Md5::operator>(const Md5& src) const
{
    if(&src == this)
    {
	return false;
    }
    else
    {
	return (memcmp(&myDigest, &src.myDigest, 16) > 0);
    }
}


} // namespace Crypto
} // namespace Vocal
#endif
