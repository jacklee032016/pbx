/*
 * $Id: Md5Test.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "Verify.hxx"
#include "Md5.hxx"
#include <strstream>

using Vocal::Crypto::Md5;

int main()
{
    strstream x;

    Data data("asdf");
    Md5 y(data);

//    x << y;

//    test_verify(strcmp(x.str(), "912ec803b2ce49e4a541068d495ab570") == 0);


    {
      Data a("asdf");
      Data b("asdf");

      Md5 x(a);
      Md5 y(b);
      Md5 z(Data("asdfhiasfhd"));

      test_verify(x.hashfn() == y.hashfn());
      test_verify(x.hashfn() != z.hashfn());
    }


    return test_return_code(2);
}
